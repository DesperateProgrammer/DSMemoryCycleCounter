#include "CacheTests.h"
#include "CP15.h"
#include "Opcodes.h"
#include "MedianMeasurements.h"
#include "MemoryTimings.h"
#include "Menu.h"
#include <stdint.h>
#include <nds.h>
#include <stdio.h>
#include <stdbool.h>

/* 
    We measure the instruction cache size by running chains of NOP instructions
    from instruction cached main memory for several repeats. Once the chain does
    not fit completely into the instruction cache, the instructions must be 
    fetched from a much slower main memory, which will be measured.
    The increase of more than 20% will indicate that the current run is too
    long, thus the previous run was the largest fitting into the cache-size

    A length from 2^8 = 256 to 2^16 = 65kB are checked. Each run doubles the
    checked length.
*/
uint32_t MeasureICacheSize()
{
  EnableICache();
  EnableDCache();
  UnlockAllDCache() ;
  UnlockAllICache() ;
  
  STRUCT_INSTRUCTION_TIMING_TEST_SETTINGS settings ;
  settings.stripeSize = (1 << 8)-12 ;
  settings.repeats = 1000;
  settings.arg0 = 0;
  settings.arg1 = 0;
  settings.instructionA = settings.instructionB = *(uint32_t *)&OPCODES_NOP;  

  uint32_t timeeNeededLast = MedianOfMeasurements(7, (FUNC_SINGLEMEASUREMENT)&ExecuteInstructionTimingTest, &settings)  / (1 << 8);
  
  for (int i=9;i<16;i++)
  {
    settings. stripeSize = (1 << i)-12;
    uint32_t timeNeeded = MedianOfMeasurements(7, (FUNC_SINGLEMEASUREMENT)&ExecuteInstructionTimingTest, &settings)  / (1 << i);
    if (timeNeeded > timeeNeededLast * 12 / 10)
    {
      return 1 << (i-1) ;
    }
    timeeNeededLast = timeNeeded;
  }
  return 0;
}

/*
  return the instruction cache size encoded into the Cache-Type Register
  of the CP15.
*/
uint32_t GetReportedICacheSize()
{
  uint32_t cacheTypeRegister = CP15_GetCacheType();
  uint16_t ISize = cacheTypeRegister & 0x0fff; 
  uint8_t sizeIndex = (ISize >> 6) &0x0F ;
  bool M = ISize & 0x04 ;
  
  if (!M)
    return 512 * (1 << sizeIndex);
  else
    return 768 * (1 << sizeIndex);
}

/*
  return the instruction cache line size encoded into the Cache-Type Register
  of the CP15.
*/
uint32_t GetReportedICacheLineSize()
{
  uint32_t cacheTypeRegister = CP15_GetCacheType();
  uint16_t ISize = cacheTypeRegister & 0x0fff; 
  uint8_t len = ISize & 0x03;
  
  return 8 * (1 << len);
}

/* 
    We measure the instruction cache line size by invalidating a cache line
    at a given address and call multiple times into this address containing
    a simple return 'bx lr'
    
    If the instruction was not cleared with the cache line that was evicted the
    execution time increases as the instruction needs to be fetched from main
    memory instead of the cache

    A length from 2^2 = 4 to 2^10 = 1024 Bytes are checked. Each run doubles the
    checked length.
*/
uint32_t MeasureICacheLineSize()
{
  EnableICache();
  EnableDCache();
  
  STRUCT_CACHELINE_TEST_SETTINGS settings ;
  settings.repeats = 10000 ;
  
  uint32_t prevRun = 0;
  for (int i=2;i<10;i++)
  {
    settings.lineSize = 1 << i;
    uint32_t measure = MedianOfMeasurements(7, (FUNC_SINGLEMEASUREMENT)&ExecuteICacheLineTest, &settings);
    if (measure * 2 < prevRun)
    {
      return (1 << i);
    }
    prevRun = measure;
  }
  return 0;
}

uint8_t MeasureICacheAssociativity()
{
  for (int i=0;i<31;i++)
  {
    WriteICacheLockDown(0x80000000 | (1 << i));
    if ((ReadICacheLockDown() & (1 << i)) == 0)
    {
      // this bit was not setable.
      // so the associative way does not exist
      WriteICacheLockDown(0);
      return i;
    }
  }
  WriteICacheLockDown(0);
  return 31;
}

/* 
    We measure the data cache size by running chains of data read instructions
    from non instruction cached main memory for several repeats. The 
    instructions will need to be fetched from memory with a constant timing
    requirement. 
    Once the data that is read does not fit completely into the data cache, the 
    data must be fetched from a much slower main memory, which will be measured.
    The increase of more than 20% will indicate that the current run is too
    long, thus the previous run was the largest fitting into the cache-size
    
    A length from 2^8 = 256 to 2^16 = 65kB are checked. Each run doubles the
    checked length.
*/
uint32_t MeasureDCacheSize()
{
  DisableICache();
  EnableDCache();
  
  STRUCT_INSTRUCTION_TIMING_TEST_SETTINGS settings ;
  settings.stripeSize = (1 << 8)-12 ;
  settings.repeats = 1000;
  settings.arg0 = 0;
  settings.arg1 = 0;
  settings.instructionA = settings.instructionB = *(uint32_t *)&OPCODES_LDR_r2_pc;
  
  uint32_t timeeNeededLast = MedianOfMeasurements(7, (FUNC_SINGLEMEASUREMENT)&ExecuteInstructionTimingTest, &settings)  / (1 << 8);

  for (int i=9;i<16;i++)
  {
    settings. stripeSize = (1 << i)-12;
    uint32_t timeNeeded = MedianOfMeasurements(7, (FUNC_SINGLEMEASUREMENT)&ExecuteInstructionTimingTest, &settings)  / (1 << i);
    if (timeNeeded > timeeNeededLast * 12 / 10)
    {
      EnableICache();
      return (1 << (i-1)) ;
    }
    timeeNeededLast = timeNeeded;
  }
  EnableICache() ;
  return 0;
}

/*
  return the data cache size encoded into the Cache-Type Register
  of the CP15.
*/
uint32_t GetReportedDCacheSize()
{
  uint32_t cacheTypeRegister = CP15_GetCacheType();
  uint16_t DSize = (cacheTypeRegister >> 12) & 0x0fff; 
  uint8_t sizeIndex = (DSize >> 6) &0x0F ;
  bool M = DSize & 0x04 ;
  
  if (!M)
    return 512 * (1 << sizeIndex);
  else
    return 768 * (1 << sizeIndex);
}


/*
  return the data cache line size encoded into the Cache-Type Register
  of the CP15.
*/
uint32_t GetReportedDCacheLineSize()
{
  uint32_t cacheTypeRegister = CP15_GetCacheType();
  uint16_t DSize = (cacheTypeRegister >> 12) & 0x0fff; 
  uint8_t len = DSize & 0x03;
  
  return 8 * (1 << len);
}

/* 
    We measure the data cache line size by invalidating a cache line
    at a given address and read multiple times from this address.
    
    If the data was not cleared with the cache line that was evicted the
    execution time increases as the data needs to be fetched from main
    memory instead of the cache

    A length from 2^2 = 4 to 2^10 = 1024 Bytes are checked. Each run doubles the
    checked length.
*/
uint32_t MeasureDCacheLineSize()
{
  EnableICache();
  EnableDCache();
  STRUCT_CACHELINE_TEST_SETTINGS settings ;
  settings.repeats = 10000 ;
  
  uint32_t prevRun = 0;
  for (int i=2;i<10;i++)
  {
    settings.lineSize = 1 << i;
    uint32_t measure = MedianOfMeasurements(7, (FUNC_SINGLEMEASUREMENT)&ExecuteDCacheLineTest, &settings);
    if (measure * 2 < prevRun)
    {
      return (1 << i);
    }
    prevRun = measure;
  }
  return 0;
}

uint8_t MeasureDCacheAssociativity()
{
  for (int i=0;i<31;i++)
  {
    WriteDCacheLockDown(0x80000000 | (1 << i));
    if ((ReadDCacheLockDown() & (1 << i)) == 0)
    {
      // this bit was not setable.
      // so the associative way does not exist
      WriteDCacheLockDown(0);
      return i;
    }
  }
  WriteDCacheLockDown(0);
  return 31;
}

/* Run all Cache-Tests and dispay their result */
void RunCacheTests()
{
  /* Measure Cache Size */
  EnableICache();
  EnableDCache();
  UnlockAllDCache() ;
  UnlockAllICache() ;

  iprintf("  Instruction and data caches:  ");
  iprintf("--------------------------------");  
  iprintf("\n\n");
  
  iprintf("Reported ICache Size: %lu\n", GetReportedICacheSize());
  iprintf("Measured ICache Size: %lu\n", MeasureICacheSize());
  iprintf("Reported ICache Line: %lu\n", GetReportedICacheLineSize());
  iprintf("Measured ICache Line: %lu\n", MeasureICacheLineSize());
  iprintf("CP Bits ICache Ways : %u\n", MeasureICacheAssociativity());
  iprintf("Reported DCache Size: %lu\n", GetReportedDCacheSize());
  iprintf("Measured DCache Size: %lu\n", MeasureDCacheSize());
  iprintf("Reported DCache Line: %lu\n", GetReportedDCacheLineSize());
  iprintf("Measured DCache Line: %lu\n", MeasureDCacheLineSize());
  iprintf("CP Bits DCache Ways : %u\n", MeasureDCacheAssociativity());
      
  WaitForAnyKey();
}