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

  uint32_t timeeNeededLast = MedianOfMeasurements(3, (FUNC_SINGLEMEASUREMENT)&ExecuteInstructionTimingTest, &settings)  / (1 << 8);
  
  for (int i=9;i<16;i++)
  {
    settings. stripeSize = (1 << i)-12;
    uint32_t timeNeeded = MedianOfMeasurements(3, (FUNC_SINGLEMEASUREMENT)&ExecuteInstructionTimingTest, &settings)  / (1 << i);
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

DTCM_DATA uint32_t tags[256];

void CacheOnSingleCycleMemTest()
{
  REG_IME = 0;
  DC_FlushAll();
  DisableDCache();
  // enable data cache only on DTCM (Region 5)
  // DTCM can be accessed by 1 cycle, the cache fill should take 8 cycles

  asm("mcr p15, 0, %0, c2, c0, 0        \n"
      :
      : "r" (0b01111100)
      :
      );
  register uint32_t *ptr = (uint32_t *)((uint32_t)tags & ~0x3FFF);
  register uint32_t val;
  // access 1kB as fast as possible. That is 1024 / 32 = 32 lines
  // we make sure, that the code is loaded into icache too. so we do some runs before we start evaluating the DCache Tags
  EnableICache();
  for (register uint32_t i=0;i<1000;i++)
  {
    DC_FlushAll();
    asm volatile (
             "LDR %0, [%1],#32    \n"
             "LDR %0, [%1],#32    \n"
             "LDR %0, [%1],#32    \n"
             "LDR %0, [%1],#32    \n"
             "LDR %0, [%1],#32    \n"
             "LDR %0, [%1],#32    \n"
             "LDR %0, [%1],#32    \n"
             "LDR %0, [%1],#32    \n"
             "LDR %0, [%1],#32    \n"
             "LDR %0, [%1],#32    \n"
             "LDR %0, [%1],#32    \n"
             "LDR %0, [%1],#32    \n"
             "LDR %0, [%1],#32    \n"
             "LDR %0, [%1],#32    \n"
             "LDR %0, [%1],#32    \n"
             "LDR %0, [%1],#32    \n"
             : "=r" (val)
             : "r" (ptr)
             :
    );
  }
  // now we can get the used set (and our PRNG bits)
  for (uint32_t i=0;i<32;i++)
  {
    for (uint32_t q=0;q<5;q++)
    {
      if (q == 4)
      {
        iprintf("-");
        break;
      }
      WriteCacheDebugIndexRegister(((q & 0x03) << 30) | (((uint32_t)&ptr[i*8]) & 0x03E0));
      uint32_t tag = ReadDataCacheTagRegister();
      iprintf("%08lx", tag);
      if ((ReadDataCacheTagRegister() & ~0x0F) == ((((uint32_t)&ptr[i*8]) & ~0x1F) | 0x10))
      {
        iprintf("%lu",q);
      }
    }
  }
  REG_IME = 1;
}

void CachePRNG0SeriesStats()
{
  /*
    Find 0-serries and count them against the sample count
    Since we get 2-bits at once we will take these and count them.

    The output of an lfsr does not include the all-0 pattern. Thus
    all 0-series pattern will be slightly underrepresentated. The greater the
    distance between the length of the 0-pattern and the length of the
    lfsr, the smaller this underrepresentation will become.

    The amount of 2-length 0-series for the lfsr lengths:
      2:    0 of 3            0,000000000
      3:    1 of 7            0,142857143
      4:    3 of 15           0,200000000
      5:    7 of 31           0,225806452
      6:   15 of 63           0,238095238
      7:   31 of 127          0,244094488
      8:   63 of 255          0,247058824
      9:  127 of 511          0,24853229
     10:  255 of 1023         0,249266862
     11:  511 of 2047         0,24963361
     12: 1023 of 4095         0,24981685
     13: 2047 of 8191         0,249908436
     14: 4095 of 16383        0,249954221
     15: 8191 of 32767        0,249977111
     ...

    If we do enough runs we will get closer to the ratio
    for the given length, even if we do not sample all or regularly

    Instead we get a value that is depenend on the amount of cycles we do
    Base is the code below without any NOPs (MOV r8, r8)
        Base      ~0.3145
        Base+1    ~0.2263
        Base+2    ~0.2573
        Base+3    ~0.1970
        Base+4    ~0.2630
        Base+5    ~0.2439
        Base+6    ~0.2149
        Base+7    ~0.2557       -> Up to +7 mean: 0.246575
        Base+8    ~0.2718
        Base+9    ~0.1569
        Base+10   ~0.2084
        Base+11   ~0.1638
        Base+12   ~0.2824
        Base+13   ~0.2678 (more volatile, last digit not stable)
        Base+14   ~0.2595
        Base+15   ~0.2180       -> Up to +15 mean: 0.237575
        Base+16   ~0.1987
        Base+17   ~0.2697
        Base+18   ~0.2314
        ...
        Base+127  ~0.3083
        ...
        Base+255  ~0.2755
        Base+256  ~0.2147
        ...
        Base+511  ~0.2624
  */
  memset(&tags, 0x00, sizeof(tags));
  EnableICache();
  EnableDCache();
  DC_FlushAll();
  register uint32_t *ptr = (uint32_t *)0x0200001C;
  register uint32_t i = 0, q = 0;
  register uint32_t val;
  for (q=0;q<10000;q++)
  {
    ptr = (uint32_t *)0x0200001C;
    for (i=0;i<4096*512;i+=32)
    {
        // fetch from address
        asm volatile (
            "MOV r8, r8      \n"
            "MOV r8, r8      \n"
            "MOV r8, r8      \n"
            "MOV r8, r8      \n"
            "MOV r8, r8      \n"
            "MOV r8, r8      \n"
            "MOV r8, r8      \n"
            "MOV r8, r8      \n"
            "MOV r8, r8      \n"
            "MOV r8, r8      \n"
            "MOV r8, r8      \n"
            "MOV r8, r8      \n"
            "MOV r8, r8      \n"
            "MOV r8, r8      \n"
            "MOV r8, r8      \n"
            "MOV r8, r8      \n"
            "MOV r8, r8      \n"
            "MOV r8, r8      \n"
            "LDR %0, [%1]    \n"
             : "=r" (val)
             : "r" (ptr)
             :
        );
        // find where it was stored
        WriteCacheDebugIndexRegister(((0 & 0x03) << 30) | ((uint32_t)ptr & 0x03E0));
        // if it was stored in set 0, we have a double-0 in the output of the lfsr
        // This if then adds cycles to one result but not the other
        if ((ReadDataCacheTagRegister() & ~0x1F) == (((uint32_t)ptr & ~0x1F)))
        {
            tags[0]++;
        } else
        {
          tags[2]++;
        }
        // and forget that flash entry
        WriteDataCacheTagRegister(0x00000000);
        tags[1]++;        
        ptr+=8;
    }
    iprintf("%lu of %lu -> 0,%06llu\n", tags[0], tags[1], (tags[0]*1000000ull) / tags[1]);
  }

}

ITCM_CODE void ReverseCachePRGN()
{
  memset(&tags, 0xff, sizeof(tags));
  EnableICache();
  EnableDCache();
  DC_FlushAll();
  register uint32_t *ptr = (uint32_t *)0x0200001C;
  register uint32_t i = 0, q = 0;
  register uint32_t val;
  for (i=0;i<4096;i+=32)
  {
      // fetch from address
      asm volatile ("LDR %0, [%1]    \n"
           : "=r" (val)
           : "r" (ptr)
           :
      );
      // find where it was stored
      for (q=0;q<4;q++)
      {
        WriteCacheDebugIndexRegister(((q & 0x03) << 30) | ((uint32_t)ptr & 0x03E0));
        register uint32_t tag = ReadDataCacheTagRegister();
        if ((tag & ~0x0F) == (((uint32_t)ptr & ~0x1F) | 0x10))
          tags[i >> 5] = q;
      }
      ptr+=8;
  }

#if 0
  for (uint32_t polynom = 1; polynom != 0; polynom++)
  {
    bool AOK = true;
    bool BOK = true;
    for (uint32_t position = 0; position < 256 - 32; position++)
    {
      uint32_t curValA = 0, curValB = 0;
      bool expectedA = false;
      for (int i=position;i<position+32;i++)
      {
        // get the bits from the sets 
        // low bit first
        if (tags[i] & 1)
        {
          curValA |= (1 << i);
        }
      }
      expectedA =(tags[position+32] & 1);
      // apply the polynom
      curValA &= polynom;
      curValA ^= curValA >> 16;
      curValA ^= curValA >> 8;
      curValA ^= curValA >> 4;
      curValA ^= curValA >> 2;
      curValA ^= curValA >> 1;
      curValA &= 1; 
      curValB &= polynom;
      if (!!curValA != expectedA)
        AOK = false;
      if (!!curValA != !expectedA)
        BOK = false;
      if (!AOK && !BOK)
      {
        if (!(polynom % 10000))
        {
          iprintf("Failed polynom %08lx\n", polynom);
        }
        break;
      }
    }
    if (AOK)
    {
      iprintf("AOK: %08lx\n", polynom);
      while (true);
    }
    if (BOK)
    {
      iprintf("AOK: %08lx\n", polynom);
      while (true);
    }
  }
  
#endif
  for (i=0;i<128;i++)
  {
    switch(tags[i])
    {
      case 0:
      case 1:
      case 2:
      case 3:
        iprintf("%lu", tags[i]);
        break;
      default:
        iprintf("-");
        break;
    }
  }

}

ITCM_CODE uint32_t ReadDataCacheSizeByTags()
{
  REG_IME = 0;
  EnableICache();
  EnableDCache();
  DC_FlushAll();
  /* make sure to fill up
     the assumed data cache of 4kB by reading from the
     last word within each cacheline. This ensures
     that the complete cache is filled as soon as the 
     read finishs.

     This and the readout should be done in asm
     to ensure that really no cached data read is
     executed inbetween.

  */
  register uint32_t *ptr = (uint32_t *)0x0200001C;
  register uint32_t i = 0;
  {
    for (i=0;i<4096;i+=8)
    {
      register uint32_t val;
      asm volatile ("LDR %0, [%1]    \n"
           : "=r" (val)
           : "r" (ptr)
           :
      );
      ptr += 2;
    }
    ptr = (uint32_t *)0x0200001C;
    for (i=0;i<4096;i+=8)
    {
      register uint32_t val;
      asm volatile ("LDR %0, [%1]    \n"
           : "=r" (val)
           : "r" (ptr)
           :
      );
      ptr += 2;
    }
    ptr = (uint32_t *)0x0200001C;
    for (i=0;i<4096;i+=8)
    {
      register uint32_t val;
      asm volatile ("LDR %0, [%1]    \n"
           : "=r" (val)
           : "r" (ptr)
           :
      );
      ptr += 2;
    }
    ptr = (uint32_t *)0x0200001C;
    for (i=0;i<4096;i+=8)
    {
      register uint32_t val;
      asm volatile ("LDR %0, [%1]    \n"
           : "=r" (val)
           : "r" (ptr)
           :
      );
      ptr += 2;
    }
    /* after the whole cache is filled and the tag memory
       contains known values, we will copy them to a non cached
       location (DTCM) to not modify our cache again.
       The functions called are inlined so we should not have
       stacked operations here.
       All PC relative loads are from ITCM and not cached.
    */
    for (i=0;i<123;i++)
    {
      WriteCacheDebugIndexRegister(((i & 0x60) << 25) | ((i & 0x1F) << 5));
      tags[i] = ReadDataCacheTagRegister();
    }
  }
  /* once we got all this, we can use other functions again and read from
     cached memory again */
  REG_IME = 1;
  for (int i=0;i<128;i++)
  {
    iprintf("Entry %02x: %08lx\n", i, tags[i]);
    if (((i+1) % 16) == 0)
    {
      WaitForAnyKey();
    }
  }

  return 0;
}

bool TestCacheDirtyStatus()
{
  EnableICache();
  EnableDCache();
  volatile int target = 0 ;
  REG_IME = 0;
  DC_FlushAll();
  register uint32_t statusPRE = GetCacheDirtyStatus() ;
  target = 1;
  register uint32_t statusPOST = GetCacheDirtyStatus() ;
  REG_IME = 1;
  return ((statusPRE & 1) == 0) && ((statusPOST & 1) != 0) && (target == 1);
}

bool TestCacheDataWrite()
{
  EnableICache();
  DisableDCache();
  DC_FlushAll();
  WriteCacheDebugIndexRegister(((0 & 0x60) << 25) | ((0 & 0x1F) << 5));
  uint32_t a = ReadDataCacheRegister();
  uint32_t b = ~a;
  WriteDataCacheRegister(b);
  uint32_t c = ReadDataCacheRegister();
  iprintf("%08lx, %08lx\n", a, c);
  return a == ~c;
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

//  ReadDataCacheSizeByTags();
  
  iprintf("Reported ICache Size: %lu\n", GetReportedICacheSize());
  iprintf("Measured ICache Size: %lu\n", MeasureICacheSize());
  iprintf("Reported ICache Line: %lu\n", GetReportedICacheLineSize());
  iprintf("Measured ICache Line: %lu\n", MeasureICacheLineSize());
  iprintf("CP Bits ICache Ways : %u\n", MeasureICacheAssociativity());
  iprintf("Reported DCache Size: %lu\n", GetReportedDCacheSize());
  DisableICache();
  DisableDCache();
  DisableRoundRobinCache();
//  TestCacheDataWrite();
//  CacheOnSingleCycleMemTest();
//  CachePRNG0SeriesStats();
//  WaitForAnyKey();
  ReverseCachePRGN();
//  WaitForAnyKey();
//  ReadDataCacheSizeByTags();
  iprintf("Measured DCache Size (RN): %lu\n", MeasureDCacheSize());
//  ReadDataCacheSizeByTags();
  DisableICache();
  DisableDCache();
  EnableRoundRobinCache();
//  ReverseCachePRGN();
  iprintf("Measured DCache Size (RR): %lu\n", MeasureDCacheSize());
//  ReadDataCacheSizeByTags();
  iprintf("Reported DCache Line: %lu\n", GetReportedDCacheLineSize());
  iprintf("Measured DCache Line: %lu\n", MeasureDCacheLineSize());
  iprintf("CP Bits DCache Ways : %u\n", MeasureDCacheAssociativity());
  iprintf("\n");
  iprintf("Cache Dirty bit: %s\n", TestCacheDirtyStatus()?"OK":"N.A.");
      
  WaitForAnyKey();
}