#include "CP15.h"

uint32_t __attribute__((naked)) DoMRC()
{
  asm(
    "eor	r0, r0, r0                  \n"
    "mrc	p15, 0, r0, c0, c0, 0       \n"
    "bx lr                            \n"
    : 
    :
  );
}

uint32_t __attribute__((naked)) CP15_GetMainID()
{
  uint32_t value = 0xDEADBEEF;
  asm(
    "mrc	p15, 0, %0, c0, c0, 0       \n"
    "bx lr                            \n"
    : "=r" (value)
    :
  );
  return value;
}


uint32_t CP15_GetCacheType()
{
  uint32_t value = 0xDEADBEEF;
  asm(
    "mrc	p15, 0, %0, c0, c0, 1       \n"
    "bx lr                            \n"
    : "=r" (value)
    :
  );
  return value;
}


uint32_t __attribute__((naked)) CP15_GetTCM()
{
  asm(
    "eor	r0, r0, r0                  \n"
    "mrc	p15, 0, r0, c0, c0, 2       \n"
    "bx lr                            \n"
    : 
    :
  );
}

uint32_t __attribute__((naked)) CP15_GetTLB()
{
  asm(
    "eor	r0, r0, r0                  \n"
    "mrc	p15, 0, r0, c0, c0, 3       \n"
    "bx lr                            \n"
    : 
    :
  );
}

uint32_t __attribute__((naked)) CP15_GetMPUType()
{
  asm(
    "eor	r0, r0, r0                  \n"
    "mrc	p15, 0, r0, c0, c0, 4       \n"
    "bx lr                            \n"
    : 
    :
  );
}


uint32_t __attribute__((naked)) CP15_GetRegion0()
{
  asm(
    "eor	r0, r0, r0                  \n"
    "mrc	p15, 0, r0, c6, c0, 0       \n"
    "bx lr                            \n"
    : 
    :
  );
}

uint32_t __attribute__((naked)) CP15_GetRegion1()
{
  asm(
    "eor	r0, r0, r0                  \n"
    "mrc	p15, 0, r0, c6, c1, 0       \n"
    "bx lr                            \n"
    : 
    :
  );
}

uint32_t __attribute__((naked)) CP15_GetRegion2()
{
  asm(
    "eor	r0, r0, r0                  \n"
    "mrc	p15, 0, r0, c6, c2, 0       \n"
    "bx lr                            \n"
    : 
    :
  );
}

uint32_t __attribute__((naked)) CP15_GetRegion3()
{
  asm(
    "eor	r0, r0, r0                  \n"
    "mrc	p15, 0, r0, c6, c3, 0       \n"
    "bx lr                            \n"
    : 
    :
  );
}

uint32_t __attribute__((naked)) CP15_GetRegion4()
{
  asm(
    "eor	r0, r0, r0                  \n"
    "mrc	p15, 0, r0, c6, c4, 0       \n"
    "bx lr                            \n"
    : 
    :
  );
}

uint32_t __attribute__((naked)) CP15_GetRegion5()
{
  asm(
    "eor	r0, r0, r0                  \n"
    "mrc	p15, 0, r0, c6, c5, 0       \n"
    "bx lr                            \n"
    : 
    :
  );
}

uint32_t __attribute__((naked)) CP15_GetRegion6()
{
  asm(
    "eor	r0, r0, r0                  \n"
    "mrc	p15, 0, r0, c6, c6, 0       \n"
    "bx lr                            \n"
    : 
    :
  );
}

uint32_t __attribute__((naked)) CP15_GetRegion7()
{
  asm(
    "eor	r0, r0, r0                  \n"
    "mrc	p15, 0, r0, c6, c7, 0       \n"
    "bx lr                            \n"
    : 
    :
  );
}

uint32_t __attribute__((naked)) CP15_SetRegion6(uint32_t val)
{
  asm(
    "mcr	p15, 0, r0, c6, c6, 0      \n"
    "mrc	p15, 0, r0, c6, c6, 0       \n"
    "bx lr                            \n"
    : 
    :
  );
}


uint32_t CP15_ReadRegister(uint8_t num, uint8_t opcode) 
{
  volatile uint32_t value = 0 ;
  switch ((num << 4) + opcode)
  {
    default:
    case 0x00:
      asm(
        "MCR p15, 0, %0, c0, c0, 0    \n"
        : "=r" (value)
        : 
      );
      break;
    case 0x01:
      asm(
        "MCR p15, 0, %0, c0, c0, 1    \n"
        : "=r" (value)
        : 
      );
      break;
    case 0x02:
      asm(
        "MCR p15, 0, %0, c0, c0, 2    \n"
        : "=r" (value)
        : 
      );
      break;
    case 0x03:
      asm(
        "MCR p15, 0, %0, c0, c0, 3    \n"
        : "=r" (value)
        : 
      );
      break;
    case 0x10:
      asm(
        "MCR p15, 0, %0, c1, c0, 0    \n"
        : "=r" (value)
        : 
      );
      break;
    case 0x11:
      asm(
        "MCR p15, 0, %0, c1, c0, 1    \n"
        : "=r" (value)
        : 
      );
      break;
    case 0x12:
      asm(
        "MCR p15, 0, %0, c1, c0, 2    \n"
        : "=r" (value)
        : 
      );
      break;
    case 0x13:
      asm(
        "MCR p15, 0, %0, c1, c0, 3    \n"
        : "=r" (value)
        : 
      );
      break;
  }
  return value;
}

void FlushAndDisableCaches()
{
  asm("mov	r0, #0                      \n" 
      "mcr	p15, 0, r0, c7, c5, 0       \n"
      "mcr	p15, 0, r0, c7, c6, 0       \n"
      "mcr	p15, 0, r0, c7, c10, 4      \n"
      :
      :
      : "r0"
      );
}

void DisableICache()
{
  asm("mrc	p15, 0, r0, c1, c0, 0       \n"
      "bic	r0, r0, #0x1000             \n"
      "mcr	p15, 0, r0, c1, c0, 0       \n"
      :
      :
      : "r0"
    );
}

void DisableDCache()
{
  asm("mrc	p15, 0, r0, c1, c0, 0       \n"
      "bic	r0, r0, #0x0004             \n"
      "mcr	p15, 0, r0, c1, c0, 0       \n"
      :
      :
      : "r0"
    );
}

void EnableICache()
{
  asm("mrc	p15, 0, r0, c1, c0, 0       \n"
      "orr	r0, r0, #0x1000             \n"
      "mcr	p15, 0, r0, c1, c0, 0       \n"
      :
      :
      : "r0"
    );
}

void EnableDCache()
{
  asm("mrc	p15, 0, r0, c1, c0, 0       \n"
      "orr	r0, r0, #0x0004             \n"
      "mcr	p15, 0, r0, c1, c0, 0       \n"
      :
      :
      : "r0"
    );
}

void UnlockAllDCache() 
{
  asm("eor	r0, r0, r0                  \n"
      "mcr	p15, 0, r0, c9, c0, 0       \n"
      :
      :
      : "r0"
    );
}

void UnlockAllICache() 
{
  asm("eor	r0, r0, r0                  \n"
      "mcr	p15, 0, r0, c9, c0, 1       \n"
      :
      :
      : "r0"
    );
}
