#include "TCMTests.h"
#include "CP15.h"
#include "Opcodes.h"
#include "MedianMeasurements.h"
#include "MemoryTimings.h"
#include "Menu.h"
#include <stdint.h>
#include <nds.h>
#include <stdio.h>
#include <stdbool.h>

DTCM_DATA volatile uint32_t test = 0;

static uint8_t GetCPUClockMultiplier() 
{
  return ((REG_SCFG_CLK & 1) + 1) * 2;
}

static void SetFastClock(bool set) 
{
  REG_SCFG_CLK = (REG_SCFG_CLK & ~1) | (set?1:0) ;
}


/* Run all ITCM and DTCM-Tests and dispay their result */
void RunTCMTests()
{
  EnableICache();
  EnableDCache();
  UnlockAllDCache() ;
  UnlockAllICache() ;

  consoleClear();
  iprintf("       ITCM & DTCM Timings      ");
  iprintf("--------------------------------");  
  iprintf("\n\n");
  
  int16_t iR08_66 = 0;
  int16_t iR16_66 = 0;
  int16_t iR16x2_66 = 0;
  int16_t iR32_66 = 0;
  int16_t iR32x2_66 = 0;
  int16_t iR08_133 = 0;
  int16_t iR16_133 = 0;
  int16_t iR16x2_133 = 0;
  int16_t iR32_133 = 0;
  int16_t iR32x2_133 = 0;
  int16_t dR08_66 = 0;
  int16_t dR16_66 = 0;
  int16_t dR16x2_66 = 0;
  int16_t dR32_66 = 0;
  int16_t dR32x2_66 = 0;
  int16_t dR08_133 = 0;
  int16_t dR16_133 = 0;
  int16_t dR16x2_133 = 0;
  int16_t dR32_133 = 0;
  int16_t dR32x2_133 = 0;  

  iprintf("Measuring @ 133MHz ...\n");
  SetFastClock(true);
  if (GetCPUClockMultiplier() == 4)
  {
    iR08_133 = MeasureDataAccess(0x01000000, DATAACCESS_READ | DATAACCESS_8) * GetCPUClockMultiplier() / 4;
    iprintf(" 8b   Reads: %i\n", iR08_133);
    iR16_133 = MeasureDataAccess(0x01000000, DATAACCESS_READ | DATAACCESS_16) * GetCPUClockMultiplier() / 4;
    iprintf("16b   Reads: %i\n", iR16_133);
    iR16x2_133 = MeasureDataAccess(0x01000000, DATAACCESS_READ | DATAACCESS_16x2) * GetCPUClockMultiplier() / 4;
    iprintf("16bx2 Reads: %i\n", iR16x2_133);
    iR32_133 = MeasureDataAccess(0x01000000, DATAACCESS_READ | DATAACCESS_32) * GetCPUClockMultiplier() / 4;
    iprintf("32b   Reads: %i\n", iR32_133);
    iR32x2_133 = MeasureDataAccess(0x01000000, DATAACCESS_READ | DATAACCESS_64) * GetCPUClockMultiplier() / 4;
    iprintf("32bx2 Reads: %i\n", iR32x2_133);
    dR08_133 = MeasureDataAccess((uint32_t)&test, DATAACCESS_READ | DATAACCESS_8) * GetCPUClockMultiplier() / 4;
    iprintf(" 8b   Reads: %i\n", dR08_133);
    dR16_133 = MeasureDataAccess((uint32_t)&test, DATAACCESS_READ | DATAACCESS_16) * GetCPUClockMultiplier() / 4;
    iprintf("16b   Reads: %i\n", dR16_133);
    dR16x2_133 = MeasureDataAccess((uint32_t)&test, DATAACCESS_READ | DATAACCESS_16x2) * GetCPUClockMultiplier() / 4;
    iprintf("16bx2 Reads: %i\n", dR16x2_133);
    dR32_133 = MeasureDataAccess((uint32_t)&test, DATAACCESS_READ | DATAACCESS_32) * GetCPUClockMultiplier() / 4;
    iprintf("32b   Reads: %i\n", dR32_133);
    dR32x2_133 = MeasureDataAccess((uint32_t)&test, DATAACCESS_READ | DATAACCESS_64) * GetCPUClockMultiplier() / 4;
    iprintf("32bx2 Reads: %i\n", dR32x2_133);
  } else
  {
    iprintf("133MHz not available (DS?)\n");
  }
  iprintf("Measuring @ 66MHz ...\n");
  SetFastClock(false);
  if (GetCPUClockMultiplier() == 2)
  {
    iR08_66 = MeasureDataAccess(0x01000000, DATAACCESS_READ | DATAACCESS_8) * GetCPUClockMultiplier() / 4;
    iprintf(" 8b   Reads: %i\n", iR08_66);
    iR16_66 = MeasureDataAccess(0x01000000, DATAACCESS_READ | DATAACCESS_16) * GetCPUClockMultiplier() / 4;
    iprintf("16b   Reads: %i\n", iR16_66);
    iR16x2_66 = MeasureDataAccess(0x01000000, DATAACCESS_READ | DATAACCESS_16x2) * GetCPUClockMultiplier() / 4;
    iprintf("16bx2 Reads: %i\n", iR16x2_66);
    iR32_66 = MeasureDataAccess(0x01000000, DATAACCESS_READ | DATAACCESS_32) * GetCPUClockMultiplier() / 4;
    iprintf("32b   Reads: %i\n", iR32_66);
    iR32x2_66 = MeasureDataAccess(0x01000000, DATAACCESS_READ | DATAACCESS_64) * GetCPUClockMultiplier() / 4;
    iprintf("32bx2 Reads: %i\n", iR32x2_66);
    dR08_66 = MeasureDataAccess((uint32_t)&test, DATAACCESS_READ | DATAACCESS_8) * GetCPUClockMultiplier() / 4;
    iprintf(" 8b   Reads: %i\n", dR08_66);
    dR16_66 = MeasureDataAccess((uint32_t)&test, DATAACCESS_READ | DATAACCESS_16) * GetCPUClockMultiplier() / 4;
    iprintf("16b   Reads: %i\n", dR16_66);
    dR16x2_66 = MeasureDataAccess((uint32_t)&test, DATAACCESS_READ | DATAACCESS_16x2) * GetCPUClockMultiplier() / 4;
    iprintf("16bx2 Reads: %i\n", dR16x2_66);
    dR32_66 = MeasureDataAccess((uint32_t)&test, DATAACCESS_READ | DATAACCESS_32) * GetCPUClockMultiplier() / 4;
    iprintf("32b   Reads: %i\n", dR32_66);
    dR32x2_66 = MeasureDataAccess((uint32_t)&test, DATAACCESS_READ | DATAACCESS_64) * GetCPUClockMultiplier() / 4;
    iprintf("32bx2 Reads: %i\n", dR32x2_66);
  } else
  {
    iprintf("66MHz not available (unknown HW)\n");
  }
  SetFastClock(true);  

  consoleClear();
  iprintf("       ITCM & DTCM Timings      ");
  iprintf("--------------------------------");  
  iprintf("\n\n");
  iprintf("as CPU-Cycles\n\n");
  iprintf("         N16   S16   N32   S32\n");
  iprintf(" 66Mhz I %3i   %3i   %3i   %3i\n", iR16_66, (iR16_66 - 2 *(iR16_66-iR16x2_66)), iR32_66, iR32x2_66-iR32_66);
  iprintf("133Mhz I %3i   %3i   %3i   %3i\n", iR16_133, (iR16_133 - 2 *(iR16_133-iR16x2_133)), iR32_133, iR32x2_133-iR32_133);
  iprintf(" 66Mhz D %3i   %3i   %3i   %3i\n", dR16_66, (dR16_66 - 2 *(dR16_66-dR16x2_66)), dR32_66, dR32x2_66-dR32_66);
  iprintf("133Mhz D %3i   %3i   %3i   %3i\n", dR16_133, (dR16_133 - 2 *(dR16_133-dR16x2_133)), dR32_133, dR32x2_133-dR32_133);

  iprintf("\n");
  iprintf("as Bus-Cycles\n\n");
  iprintf("         N16   S16   N32   S32\n");
  printf(" 66Mhz I %0.1f   %0.1f   %0.1f   %0.1f\n", iR16_66 / 2.0f, (iR16_66 - 2 *(iR16_66-iR16x2_66)) / 2.0f, iR32_66 / 2.0f, (iR32x2_66-iR32_66) / 2.0f);
  printf("133Mhz I %0.1f   %0.1f   %0.1f   %0.1f\n", iR16_133 / 4.0f, (iR16_133 - 2 *(iR16_133-iR16x2_133)) / 4.0f, iR32_133 / 4.0f, (iR32x2_133-iR32_133) / 4.0f);
  printf(" 66Mhz D %0.1f   %0.1f   %0.1f   %0.1f\n", dR16_66 / 2.0f, (dR16_66 - 2 *(dR16_66-dR16x2_66)) / 2.0f, dR32_66 / 2.0f, (dR32x2_66-dR32_66) / 2.0f);
  printf("133Mhz D %0.1f   %0.1f   %0.1f   %0.1f\n", dR16_133 / 4.0f, (dR16_133 - 2 *(dR16_133-dR16x2_133)) / 4.0f, dR32_133 / 4.0f, (dR32x2_133-dR32_133) / 4.0f);
  
  WaitForAnyKey();
}