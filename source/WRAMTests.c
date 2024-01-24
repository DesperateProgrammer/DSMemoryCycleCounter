#include "WRAMTests.h"
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


/* Run all ITCM and DTCM-Tests and dispay their result */
void RunWRAMTests()
{
  EnableICache();
  EnableDCache();
  UnlockAllDCache() ;
  UnlockAllICache() ;

  CP15_SetRegion3(0x03000035); // 128MB


  consoleClear();
  iprintf("           WRAM Timings         ");
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
    R08_133 = MeasureDataAccess(0x37F8000, DATAACCESS_READ | DATAACCESS_8) * GetCPUClockMultiplier() / 4;
    iprintf(" 8b   Reads: %i\n", R08_133);
    R16_133 = MeasureDataAccess(0x37F8000, DATAACCESS_READ | DATAACCESS_16) * GetCPUClockMultiplier() / 4;
    iprintf("16b   Reads: %i\n", R16_133);
    R16x2_133 = MeasureDataAccess(0x37F8000, DATAACCESS_READ | DATAACCESS_16x2) * GetCPUClockMultiplier() / 4;
    iprintf("16bx2 Reads: %i\n", R16x2_133);
    R32_133 = MeasureDataAccess(0x37F8000, DATAACCESS_READ | DATAACCESS_32) * GetCPUClockMultiplier() / 4;
    iprintf("32b   Reads: %i\n", R32_133);
    R32x2_133 = MeasureDataAccess(0x37F8000, DATAACCESS_READ | DATAACCESS_64) * GetCPUClockMultiplier() / 4;
    iprintf("32bx2 Reads: %i\n", R32x2_133);
  } else
  {
    iprintf("133MHz not available (DS?)\n");
  }
  iprintf("Measuring @ 66MHz ...\n");
  SetFastClock(false);
  if (GetCPUClockMultiplier() == 2)
  {
    R08_66 = MeasureDataAccess(0x37F8000, DATAACCESS_READ | DATAACCESS_8) * GetCPUClockMultiplier() / 4;
    iprintf(" 8b   Reads: %i\n", R08_66);
    R16_66 = MeasureDataAccess(0x37F8000, DATAACCESS_READ | DATAACCESS_16) * GetCPUClockMultiplier() / 4;
    iprintf("16b   Reads: %i\n", R16_66);
    R16x2_66 = MeasureDataAccess(0x37F8000, DATAACCESS_READ | DATAACCESS_16x2) * GetCPUClockMultiplier() / 4;
    iprintf("16bx2 Reads: %i\n", R16x2_66);
    R32_66 = MeasureDataAccess(0x37F8000, DATAACCESS_READ | DATAACCESS_32) * GetCPUClockMultiplier() / 4;
    iprintf("32b   Reads: %i\n", R32_66);
    R32x2_66 = MeasureDataAccess(0x37F8000, DATAACCESS_READ | DATAACCESS_64) * GetCPUClockMultiplier() / 4;
    iprintf("32bx2 Reads: %i\n", R32x2_66);
  } else
  {
    iprintf("66MHz not available (unknown HW)\n");
  }
  SetFastClock(true);  
  
  consoleClear();
  iprintf("           WRAM Timings         ");
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
  
  WaitForAnyKey();
}