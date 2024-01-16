#ifndef MEDIANMEASUREMENTS_H
#define MEDIANMEASUREMENTS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
  
typedef uint32_t (* FUNC_SINGLEMEASUREMENT)(void *) ;

uint32_t MedianOfMeasurements(uint32_t runs, FUNC_SINGLEMEASUREMENT measurementFunction, void *settings) ;
  
#ifdef __cplusplus
}
#endif

#endif