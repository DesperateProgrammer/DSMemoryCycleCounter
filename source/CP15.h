#ifndef CP15_H
#define CP15_H

#include <stdint.h>
#include <nds.h>

#ifdef __cplusplus
extern "C"
{
#endif
  
void DisableICache();
void DisableDCache();
void EnableICache();
void EnableDCache() ; 

void UnlockAllDCache() ;
void UnlockAllICache() ;

void WriteDCacheLockDown(uint32_t val);
uint32_t ReadDCacheLockDown();
void WriteICacheLockDown(uint32_t val);
uint32_t ReadICacheLockDown();

uint32_t CP15_ReadRegister(uint8_t num, uint8_t opcode) ;
uint32_t CP15_GetMainID();
uint32_t CP15_GetCacheType();
uint32_t CP15_GetTCM();
uint32_t CP15_GetTLB();
uint32_t CP15_GetMPUType();
uint32_t CP15_GetRegion0() ;
uint32_t CP15_GetRegion1() ;
uint32_t CP15_GetRegion2() ;
uint32_t CP15_GetRegion3() ;
uint32_t CP15_GetRegion4() ;
uint32_t CP15_GetRegion5() ;
uint32_t CP15_GetRegion6() ;
uint32_t CP15_GetRegion7() ;

uint32_t CP15_SetRegion6(uint32_t val);

#ifdef __cplusplus
}
#endif

#endif