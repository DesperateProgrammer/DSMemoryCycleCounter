#include "MainMemoryTests.h"
#include "CP15.h"
#include "Opcodes.h"
#include "MedianMeasurements.h"
#include "MemoryTimings.h"
#include "Menu.h"
#include <stdint.h>
#include <nds.h>
#include <stdio.h>
#include <stdbool.h>

static uint8_t GetCPUClockMultiplier() 
{
  return ((REG_SCFG_CLK & 1) + 1) * 2;
}

static void SetFastClock(bool set) 
{
  REG_SCFG_CLK = (REG_SCFG_CLK & ~1) | (set?1:0) ;
}

/* Run all MainMemory-Tests and dispay their result */
void RunMainMemoryTests()
{
  EnableICache();
  EnableDCache();
  UnlockAllDCache() ;
  UnlockAllICache() ;

  consoleClear();
  iprintf("        Main Memory Timings     ");
  iprintf("--------------------------------");  
  iprintf("\n\n");
  
  int16_t R08_133 = 0;
  int16_t R16_133 = 0;
  int16_t R16x2_133 = 0;
  int16_t R32_133 = 0;
  int16_t R32x2_133 = 0;

  int16_t R08_66 = 0;
  int16_t R16_66 = 0;
  int16_t R16x2_66 = 0;
  int16_t R32_66 = 0;
  int16_t R32x2_66 = 0;
  
  iprintf("Measuring @ 133MHz ...\n");
  SetFastClock(true);
  if (GetCPUClockMultiplier() == 4)
  {
    R08_133 = MeasureDataAccess(0x02000000, DATAACCESS_READ | DATAACCESS_8) * GetCPUClockMultiplier() / 4;
    iprintf(" 8b   Reads: %i\n", R08_133);
    R16_133 = MeasureDataAccess(0x02000000, DATAACCESS_READ | DATAACCESS_16) * GetCPUClockMultiplier() / 4;
    iprintf("16b   Reads: %i\n", R16_133);
    R16x2_133 = MeasureDataAccess(0x02000000, DATAACCESS_READ | DATAACCESS_16x2) * GetCPUClockMultiplier() / 4;
    iprintf("16bx2 Reads: %i\n", R16x2_133);
    R32_133 = MeasureDataAccess(0x02000000, DATAACCESS_READ | DATAACCESS_32) * GetCPUClockMultiplier() / 4;
    iprintf("32b   Reads: %i\n", R32_133);
    R32x2_133 = MeasureDataAccess(0x02000000, DATAACCESS_READ | DATAACCESS_64) * GetCPUClockMultiplier() / 4;
    iprintf("32bx2 Reads: %i\n", R32x2_133);
  } else
  {
    iprintf("133MHz not available (DS?)\n");
  }
  iprintf("Measuring @ 66MHz ...\n");
  SetFastClock(false);
  if (GetCPUClockMultiplier() == 2)
  {
    R08_66 = MeasureDataAccess(0x02000000, DATAACCESS_READ | DATAACCESS_8) * GetCPUClockMultiplier() / 4;
    iprintf(" 8b   Reads: %i\n", R08_66);
    R16_66 = MeasureDataAccess(0x02000000, DATAACCESS_READ | DATAACCESS_16) * GetCPUClockMultiplier() / 4;
    iprintf("16b   Reads: %i\n", R16_66);
    R16x2_66 = MeasureDataAccess(0x02000000, DATAACCESS_READ | DATAACCESS_16x2) * GetCPUClockMultiplier() / 4;
    iprintf("16bx2 Reads: %i\n", R16x2_66);
    R32_66 = MeasureDataAccess(0x02000000, DATAACCESS_READ | DATAACCESS_32) * GetCPUClockMultiplier() / 4;
    iprintf("32b   Reads: %i\n", R32_66);
    R32x2_66 = MeasureDataAccess(0x02000000, DATAACCESS_READ | DATAACCESS_64) * GetCPUClockMultiplier() / 4;
    iprintf("32bx2 Reads: %i\n", R32x2_66);
  } else
  {
    iprintf("66MHz not available (unknown HW)\n");
  }
  SetFastClock(true);  

  uint32_t data = (uint32_t)malloc(16*1024);
  STRUCT_DMA_TIMING_TEST_SETTINGS settings;
  settings.repeats = 4096;
  settings.address = data;
  settings.directionsUp = 1;
  settings.bitWidth = 16;
  int16_t DMAseq16 =MedianOfMeasurements(7, (FUNC_SINGLEMEASUREMENT)&ExecuteDMATimingTest, &settings) / 4095;
  printf("DMA 16 up: %3i\n", DMAseq16);
  settings.directionsUp = 1;
  settings.bitWidth = 32;
  int16_t DMAnon16 =MedianOfMeasurements(7, (FUNC_SINGLEMEASUREMENT)&ExecuteDMATimingTest, &settings) / 4095;
  printf("DMA 16 down: %3i\n", DMAnon16);
  settings.directionsUp = 0;
  settings.bitWidth = 16;
  int16_t DMAseq32 =MedianOfMeasurements(7, (FUNC_SINGLEMEASUREMENT)&ExecuteDMATimingTest, &settings) / 4095;
  printf("DMA 32 up: %3i\n", DMAseq32);
  settings.directionsUp = 0;
  settings.bitWidth = 32;
  int16_t DMAnon32 =MedianOfMeasurements(7, (FUNC_SINGLEMEASUREMENT)&ExecuteDMATimingTest, &settings) / 4095;
  printf("DMA 32 down: %3i\n", DMAnon32);
  free((void *)data);
  
  consoleClear();
  iprintf("        Main Memory Timings     ");
  iprintf("--------------------------------");  
  iprintf("\n\n");
  iprintf("as CPU-Cycles\n\n");
  iprintf("        N16   S16   N32   S32\n");
  iprintf(" 66Mhz  %3i   %3i   %3i   %3i\n", R16_66, (R16_66 - 2 *(R16_66-R16x2_66)), R32_66, R32x2_66-R32_66);
  iprintf("133Mhz  %3i   %3i   %3i   %3i\n", R16_133, (R16_133 - 2 *(R16_133-R16x2_133)), R32_133, R32x2_133-R32_133);

  iprintf("\n\n");
  iprintf("as Bus-Cycles\n\n");
  iprintf("        N16   S16   N32   S32\n");
  iprintf(" 66Mhz  %3i   %3i   %3i   %3i\n", R16_66 / 2, (R16_66 - 2 *(R16_66-R16x2_66)) / 2, R32_66 / 2, (R32x2_66-R32_66) / 2);
  iprintf("133Mhz  %3i   %3i   %3i   %3i\n", R16_133 / 4, (R16_133 - 2 *(R16_133-R16x2_133)) / 4, R32_133 / 4, (R32x2_133-R32_133) / 4);
  iprintf("  DMA   %3i   %3i   %3i   %3i\n", DMAnon16, DMAseq16, DMAnon32, DMAseq32);
  
  WaitForAnyKey();

}