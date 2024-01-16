#include "ScanLineTimings.h"
#include "CP15.h"
#include <nds.h>

uint32_t MeasureTenScanLines()
{
  EnableICache();
  EnableDCache();

  uint32_t scanLineTime_10 = 0 ;
  for (int i=0;i<10;i++)
  {
    uint32_t scanLineTime = 0 ;
    uint32_t scanLineStart = 0 ;
    uint32_t scanLineMeasured = 0 ;
    uint32_t currentScanLine = 0 ;
    uint32_t lastScanLine = 0 ;
    cpuStartTiming(0) ;
    REG_IME = 0;
    while (scanLineTime == 0)
    {
      currentScanLine = (REG_VCOUNT >> 0) & 0x1FF;
      if (currentScanLine != lastScanLine)
      {
        if (scanLineMeasured == 0)
        {
          scanLineMeasured = currentScanLine ;
          scanLineStart = cpuGetTiming();
        } else
        {
          scanLineTime = cpuGetTiming() - scanLineStart;
        }
      }
      lastScanLine = currentScanLine ;
    }
    scanLineTime_10 += scanLineTime;
  }
  REG_IME = 1;
  return scanLineTime_10 ;
}

uint32_t MedianTenScanlineMeasurement(uint32_t runs)
{
  uint32_t *runTimings = (uint32_t *)malloc(runs * sizeof(uint32_t)) ;
  if (runTimings == 0)
  {
    return 0 ;
  }
  
  int i,q;
  for (i = 0;i<runs;i++)
  { 
    runTimings[i] = MeasureTenScanLines() ;
  }
  
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
  
  uint32_t timing = runTimings[runs / 2];
  free(runTimings);
  return timing;
}