#include "Opcodes.h"

void __attribute__((naked)) OPCODES_NOP()
{
  asm(
    "ADD r8, r8, #0       \n"
    :
    :
  );
}

void __attribute__((naked)) OPCODES_LDR_r3_r2()
{
  asm(
    "LDR r3, [r2]       \n"
    :
    :
  );
}

void __attribute__((naked)) OPCODES_LDRH_r3_r2()
{
  asm(
    "LDRH r3, [r2]       \n"
    :
    :
  );
}

void __attribute__((naked)) OPCODES_LDRH_r3_r2_2()
{
  asm(
    "LDRH r3, [r2, #2]       \n"
    :
    :
  );
}

void __attribute__((naked)) OPCODES_LDRB_r3_r2()
{
  asm(
    "LDRH r3, [r2]       \n"
    :
    :
  );
}

void __attribute__((naked)) OPCODES_LDM_r2__r3_r12()
{
  asm(
    "LDM r2, {r3, r12}       \n"
    :
    :
  );
}

void __attribute__((naked)) OPCODES_LDR_r2_pc()
{
  asm(
    "LDR r2, [pc]       \n"
    :
    :
  );
}

void __attribute__((naked)) OPCODES_SUBS_R0_1()
{
  asm(
    "SUBS R0, R0, #1      \n"
    :
    :
  );
}

void __attribute__((naked)) OPCODES_BXNE_R1()
{
  asm(
    "BXNE R1              \n"
    :
    :
  );
}

void __attribute__((naked)) OPCODES_BX_LR()
{
  asm(
    "BX lr               \n"
    :
    :
  );
}
