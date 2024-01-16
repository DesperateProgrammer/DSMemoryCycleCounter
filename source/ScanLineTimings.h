#ifndef SCANLINE_TIMINGS_H
#define SCANLINE_TIMINGS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

uint32_t MeasureTenScanLines();
uint32_t MedianTenScanlineMeasurement(uint32_t runs);

#ifdef __cplusplus
}
#endif

#endif