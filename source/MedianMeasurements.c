#include "MedianMeasurements.h"

#include <malloc.h>

uint32_t MedianOfMeasurements(uint32_t runs, FUNC_SINGLEMEASUREMENT measurementFunction, void *settings)
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
    runTimings[i] = measurementFunction(settings) ;
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