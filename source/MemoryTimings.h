#ifndef MEMORY_TIMINGS_H
#define MEMORY_TIMINGS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#define DATAACCESS_READ         0x01
#define DATAACCESS_WRITE        0x02

#define DATAACCESS_8            0x04
#define DATAACCESS_16           0x08
#define DATAACCESS_32           0x0C
#define DATAACCESS_64           0x10
#define DATAACCESS_16x2         0x14

#define MEASUREMENT_INCONSISTENT_REFERENCE        -1
#define MEASUREMENT_INCONSISTENT                  -2


void __attribute__((naked)) OPCODES_NOP() ;
void __attribute__((naked)) OPCODES_LDR_r2_pc() ;


typedef struct STRUCT_CACHELINE_TEST_SETTINGS
{
  uint32_t lineSize;
  uint32_t repeats;
} STRUCT_CACHELINE_TEST_SETTINGS ;

typedef struct STRUCT_INSTRUCTION_TIMING_TEST_SETTINGS
{
  uint32_t stripeSize;
  uint32_t repeats; 
  uint32_t arg0; 
  uint32_t arg1;
  uint32_t instructionA;
  uint32_t instructionB;
} STRUCT_INSTRUCTION_TIMING_TEST_SETTINGS;

typedef struct STRUCT_DMA_TIMING_TEST_SETTINGS
{
  uint32_t repeats;
  uint32_t address;
  uint32_t directionsUp;
  uint32_t bitWidth;
} STRUCT_DMA_TIMING_TEST_SETTINGS;
  
uint32_t ExecuteInstructionTimingTest(STRUCT_INSTRUCTION_TIMING_TEST_SETTINGS *settings);
uint32_t ExecuteICacheLineTest(STRUCT_CACHELINE_TEST_SETTINGS *settings);
uint32_t ExecuteDCacheLineTest(STRUCT_CACHELINE_TEST_SETTINGS *settings);
uint32_t ExecuteDMATimingTest(STRUCT_DMA_TIMING_TEST_SETTINGS *settings);


int16_t MeasureDataAccess(uint32_t address, uint32_t mode) ;
uint32_t MedianMeasurement(uint32_t runs, uint32_t stripeSize, uint32_t repeats, uint32_t arg0, uint32_t arg1, uint32_t instruction) ;


#ifdef __cplusplus
}
#endif

#endif