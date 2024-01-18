#include "MemoryTimings.h"
#include "MedianMeasurements.h"
#include "Opcodes.h"
#include "Menu.h"
#include "CP15.h"

#include <stdlib.h>

#include <stdio.h>
#include <nds.h>

typedef void (* ZeroArgsFunc)();
typedef void (* FourArgsFunc)(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3);

uint32_t ExecuteDMATimingTest(STRUCT_DMA_TIMING_TEST_SETTINGS *settings)
{
  // transfer from cycle counter to memory (consecutive)
  // from the pattern of data we can derive the timing

  DMA2_SRC = (uint32_t)&TIMER0_DATA;
  DMA2_CR = settings->repeats | DMA_SRC_FIX; 
  if (settings->bitWidth == 32)
  {
    DMA2_CR |= DMA_32_BIT;
  } else
  {
    DMA2_CR |= DMA_16_BIT;    
  }
  if (settings->directionsUp)
  {
    DMA2_CR |= DMA_DST_INC;
    DMA2_DEST = settings->address;
  } else
  {
    DMA2_CR |= DMA_DST_DEC;
    DMA2_DEST = settings->address + (settings->bitWidth / 8) * (settings->repeats-1);    
  }

  DC_FlushAll();
  IC_InvalidateAll();
  REG_IME = 0 ;
  cpuEndTiming();
  TIMER0_DATA = 0;
  TIMER1_DATA = 0;
  cpuStartTiming(0) ;
  DMA2_CR |= DMA_ENABLE;
  do 
  {
    asm volatile ("mov r8, r8\nmov r8, r8\n");
  } while (DMA2_CR & DMA_BUSY);
  // for more then 16 bits resolution, we take the coarse value
  // from the timers after the run by the cpu
  // and lower 16 bit from the dma'd values
  uint32_t end = cpuEndTiming();
  REG_IME = 1 ;

  DC_FlushAll();

  uint16_t first;
  uint16_t last ;
  if (settings->directionsUp)
  {
    if (settings->bitWidth == 16)
    {
      first = ((uint16_t *)settings->address)[0];
      last = ((uint16_t *)settings->address)[settings->repeats-1];
    } else
    {
      first = ((uint32_t *)settings->address)[0];
      last = ((uint32_t *)settings->address)[settings->repeats-1];      
    }
  } else
  {
    if (settings->bitWidth == 16)
    {
      last = ((uint16_t *)settings->address)[0];
      first = ((uint16_t *)settings->address)[settings->repeats-1];
    } else
    {
      last = ((uint32_t *)settings->address)[0];
      first = ((uint32_t *)settings->address)[settings->repeats-1];      
    }
  }

  if ((end & ~0xFFFF) && (last > (end & 0xFFFF)))
  {
    end = ((end & ~0xffff) - 0x10000) + last - first;
  } else
  {
    end = (end & ~0xffff) + last - first;
  }

  return end;
}

uint32_t ExecuteInstructionTimingTest(STRUCT_INSTRUCTION_TIMING_TEST_SETTINGS *settings)
{
  /* Create a loop of the given instruction in given stripeSize
     Execute this loop n-Times and calculate the cycles consumed */

  uint32_t *stripeMem = (uint32_t *)malloc(settings->stripeSize + 12 + 32768);
  if (stripeMem == 0)
    return 0;
  // align to 32kB
  uint32_t *stripe = (uint32_t *)((uint32_t)stripeMem + (32768 - ((uint32_t)stripeMem & 32767))) ;

  stripe[(settings->stripeSize / 4)+0] = *(uint32_t *)&OPCODES_SUBS_R0_1 ;
  stripe[(settings->stripeSize / 4)+1] = *(uint32_t *)&OPCODES_BXNE_R1 ;
  stripe[(settings->stripeSize / 4)+2] = *(uint32_t *)&OPCODES_BX_LR ;
  
  for (int i=0;i< (settings->stripeSize / 4); i++)
    stripe[i] = (i & 1)?settings->instructionB:settings->instructionA ;

  DC_FlushAll();
  IC_InvalidateAll();
  REG_IME = 0 ;
  FourArgsFunc func = (FourArgsFunc)stripe ;
  cpuStartTiming(0) ;
  func(settings->repeats, (uint32_t)stripe, settings->arg0, settings->arg1);
  REG_IME = 1 ;
  
  free(stripeMem);
  return cpuEndTiming();
}

uint32_t ExecuteTest(uint32_t stripeSize, uint32_t repeats, uint32_t arg0, uint32_t arg1, uint32_t instruction)
{
  /* Create a loop of the given instruction in given stripeSize
     Execute this loop n-Times and calculate the cycles consumed */

  uint32_t *stripeMem = (uint32_t *)malloc(stripeSize + 12 + 32768);
  if (stripeMem == 0)
    return 0;
  // align to 32kB
  uint32_t *stripe = (uint32_t *)((uint32_t)stripeMem + (32768 - ((uint32_t)stripeMem & 32767))) ;

  stripe[(stripeSize / 4)+0] = *(uint32_t *)&OPCODES_SUBS_R0_1 ;
  stripe[(stripeSize / 4)+1] = *(uint32_t *)&OPCODES_BXNE_R1 ;
  stripe[(stripeSize / 4)+2] = *(uint32_t *)&OPCODES_BX_LR ;
  
  for (int i=0;i< (stripeSize / 4); i++)
    stripe[i] = instruction ;

  DC_FlushAll();
  IC_InvalidateAll();
  REG_IME = 0 ;
  FourArgsFunc func = (FourArgsFunc)stripe ;
  cpuStartTiming(0) ;
  func(repeats, (uint32_t)stripe, arg0, arg1);
  REG_IME = 1 ;
  
  free(stripeMem);
  return cpuEndTiming();
}

uint32_t ExecuteDCacheLineTest(STRUCT_CACHELINE_TEST_SETTINGS *settings)
{
    uint32_t lineSize = settings->lineSize;
    uint32_t repeats = settings->repeats;
    // get a 1kB aligned buffer
    uint32_t *stripe = (uint32_t *)malloc(1024 + lineSize + (lineSize-1));
    uint32_t *stripeAligned = (uint32_t *)(((uint32_t)stripe + (1024-1)) & ~(1024-1)) ;
    stripeAligned[0] = *(uint32_t *)&OPCODES_LDR_r3_r2;
    stripeAligned[1] = *(uint32_t *)&OPCODES_BX_LR;
    stripeAligned[lineSize / sizeof(lineSize)] = *(uint32_t *)&OPCODES_BX_LR;
    
    uint32_t * dataPtr = &stripeAligned[lineSize / sizeof(lineSize)];
    FourArgsFunc func1 = (FourArgsFunc)stripeAligned;

    EnableICache();
    DC_FlushAll();
    IC_InvalidateAll();
    REG_IME = 0 ;
    cpuStartTiming(0) ;

    // Execute!
    uint32_t i;
    for (i=0;i<repeats;i++)
    {
      func1(0, 0, (uint32_t)dataPtr, 0);
      asm("mcr p15, 0, %0, c7, c6, 1      \n"
          :
          : "r" (stripeAligned)
      );
    }
    
    REG_IME = 1 ;    
    free(stripe);
    return cpuEndTiming();

}

uint32_t ExecuteICacheLineTest(STRUCT_CACHELINE_TEST_SETTINGS *settings)
{
    uint32_t lineSize = settings->lineSize;
    uint32_t repeats = settings->repeats;
    // get a 1kB aligned buffer
    uint32_t *stripe = (uint32_t *)malloc(1024 + lineSize + (lineSize-1));
    uint32_t *stripeAligned = (uint32_t *)(((uint32_t)stripe + (1024-1)) & ~(1024-1)) ;
    stripeAligned[0] = *(uint32_t *)&OPCODES_BX_LR;
    stripeAligned[lineSize / sizeof(lineSize)] = *(uint32_t *)&OPCODES_BX_LR;
    
    ZeroArgsFunc func1 = (ZeroArgsFunc)stripeAligned;
    ZeroArgsFunc func2 = (ZeroArgsFunc)&stripeAligned[lineSize / sizeof(lineSize)];
  
    EnableICache();
    DC_FlushAll();
    IC_InvalidateAll();
    REG_IME = 0 ;
    cpuStartTiming(0) ;

    // Execute!
    uint32_t i;
    for (i=0;i<repeats;i++)
    {
      func2() ;
      asm("mcr p15, 0, %0, c7, c5, 1      \n"
          :
          : "r" (func1)
      );
    }
    
    REG_IME = 1 ;    
    free(stripe);
    return cpuEndTiming();
}


uint32_t MedianMeasurement(uint32_t runs, uint32_t stripeSize, uint32_t repeats, uint32_t arg0, uint32_t arg1, uint32_t instruction)
{
  /* for a clean measurement we do multiple runs and take the median 
     this should remove any outlier measurements
  */
  uint32_t *runTimings = (uint32_t *)malloc(runs * sizeof(uint32_t)) ;
  if (runTimings == 0)
  {
    return 0 ;
  }
  
  // do all measurements
  int i,q;
  for (i = 0;i<runs;i++)
  { 
    runTimings[i] = ExecuteTest(stripeSize, repeats, arg0, arg1, instruction) ;
  }
  
  // sort by amount of cycles needed
  for (i = 0; i<runs;i++)
  {
    for (q=i+1; q < runs; q++)
    {
      if (runTimings[i] > runTimings[q])
      {
        uint32_t tmp = runTimings[i] ;
        runTimings[i] = runTimings[q] ;
        runTimings[q] = tmp ;
      }
    }
  }
  
  // and take the median
  uint32_t timing = runTimings[runs / 2];
  free(runTimings);
  return timing;
}


int16_t MeasureDataAccess(uint32_t address, uint32_t mode) 
{
    // To Measure memory speed, we make sure that we cache the instructions
    // so the prefetches are not interfering with the data read/writes
    // but we prevent data caching as we want to measure the underlying data
    // access rather than the caching
    
    // For non data accessing instructions like 'mov r8, r8' we get a cycle
    // count of 1 for the instruction if ran from icache.
    
    // This cycle overlaps with the memory access time of the previous 
    // previous instruction on a 'ldr r?, [r?]' 
    // so the whole time an memory accessing instruction is using up can be
    // accounted to the data access
    
    // This is only true, when the instructions are run from icache. if the same
    // memory is accessed for the instruction fetch and data access both times
    // add up. I.e. when running uncached from main ram, 36 cycles for the
    // instruction fetch and the 36 cycles for the data read result in 72 cycles
    // per instruction.
    
    EnableICache();
    DisableDCache();
    
    STRUCT_INSTRUCTION_TIMING_TEST_SETTINGS settings ;
    settings.stripeSize = 128 ;
    settings.repeats = 10000;
    settings.arg0 = address;
    settings.arg1 = address;
    settings.instructionA = *(uint32_t *)&OPCODES_NOP;
    settings.instructionB = *(uint32_t *)&OPCODES_NOP;

    
    // Mesuring reference timing
    uint32_t nopReference128_10000 = MedianOfMeasurements(7, (FUNC_SINGLEMEASUREMENT)&ExecuteInstructionTimingTest, &settings) ;
    settings.stripeSize = 256 ;
    uint32_t nopReference256_10000 = MedianOfMeasurements(7, (FUNC_SINGLEMEASUREMENT)&ExecuteInstructionTimingTest, &settings) ;
    settings.stripeSize = 128 ;
    settings.repeats = 20000;
    uint32_t nopReference128_20000 = MedianOfMeasurements(7, (FUNC_SINGLEMEASUREMENT)&ExecuteInstructionTimingTest, &settings) ;
    
    // we have some other instructions that were executed in this chunk too and 
    // some overhead because of the measurement. We will calculate this effect 
    // and normalize to the instructions
    
    // mesurement = stripeSize * cyclesPerInstruction * repeats + overhead
    // stripeSize are known, cyclesperInstruction is constant as well as the repeats
    // so we can calculate the overhead per call
    // if we keep the stipe size constant and vary the repeats
    
    int32_t overHeadCall = nopReference128_20000 - 2 * nopReference128_10000 ;
    
    // we also have an overhead per repeat because of the additional looping
    // instructions. We can calculate this overhead by varying the stripe size but keeping
    // the repeats.
    
    uint32_t nopReference128 = nopReference128_10000 - overHeadCall ;
    uint32_t nopReference256 = nopReference256_10000 - overHeadCall ;
    
    uint32_t overheadRepeats = nopReference256 - 2 * nopReference128 ;

    nopReference128 -= overheadRepeats ;
    nopReference256 -= overheadRepeats ;

    // calculate the cycles per instruction
    nopReference128 = nopReference128 * 16 / (128 * 10000) ;
    nopReference256 = nopReference256 * 16 / (256 * 10000) ;

    // both values should be equal
    if ((nopReference128 & ~0x03) != (nopReference256 & ~0x03))
      return MEASUREMENT_INCONSISTENT_REFERENCE ;
      
#if 0      
    if (nopReference128 != nopReference256)
    {
      iprintf("NOP: %lu - %lu\n\n", nopReference128, nopReference256) ;
      return MEASUREMENT_INCONSISTENT_REFERENCE ;
    }
#endif
    
    // Debug output
    //iprintf("\n\nOVERHEAD: %li - %li\n", overHeadCall, overheadRepeats) ;
    //iprintf("NOP10: %lu - %lu\n", nopReference128_10000, nopReference256_10000) ;
    //iprintf("NOP20: %lu - %lu\n", nopReference128_20000, nopReference256_20000) ;
    //iprintf("NOP: %lu - %lu\n\n", nopReference128, nopReference256) ;
    
    uint32_t opcodeA = 0 ;
    uint32_t opcodeB = 0 ;
    uint32_t data = 0;
    switch (mode & 0x1F)
    {
      default:
      case DATAACCESS_READ | DATAACCESS_64:
        opcodeA = opcodeB = *(uint32_t *)&OPCODES_LDM_r2__r3_r12;
        break;
      case DATAACCESS_READ | DATAACCESS_32:
        opcodeA = opcodeB = *(uint32_t *)&OPCODES_LDR_r3_r2;
        break;
      case DATAACCESS_READ | DATAACCESS_16:
        opcodeA = opcodeB = *(uint32_t *)&OPCODES_LDRH_r3_r2;
        break;
      case DATAACCESS_READ | DATAACCESS_16x2:
        opcodeA = *(uint32_t *)&OPCODES_LDRH_r3_r2;
        opcodeB = *(uint32_t *)&OPCODES_LDRH_r3_r2_2;
        break;
      case DATAACCESS_READ | DATAACCESS_8:
        opcodeA = opcodeB = *(uint32_t *)&OPCODES_LDRB_r3_r2;
        break;
      case 0:
        opcodeA = opcodeB = *(uint32_t *)&OPCODES_NOP;
        break;
    }
    
    settings.repeats = 10000;
    settings.arg0 = address;
    settings.arg1 = data;
    settings.instructionA = opcodeA;
    settings.instructionB = opcodeB;
    settings.stripeSize = 256 ;
    uint32_t measure256 = MedianOfMeasurements(7, (FUNC_SINGLEMEASUREMENT)&ExecuteInstructionTimingTest, &settings) ;

    // Debug output
    //iprintf("DATARAW: %lu - %lu\n", measure128, measure256) ;

    // normalize again
    // with the already measured overhead

    measure256 -= overheadRepeats + overHeadCall;

    // calculate the cycles per instruction
    measure256 = measure256 * 16 / (256 * 10000) ;
    
    return (int)measure256;
}