#ifndef OPCODES_H
#define OPCODES_H

#ifdef __cplusplus
extern "C"
{
#endif

void OPCODES_NOP();
void OPCODES_LDR_r3_r2();
void OPCODES_LDRH_r3_r2();
void OPCODES_LDRH_r3_r2_2();
void OPCODES_LDRB_r3_r2();
void OPCODES_LDM_r2__r3_r12();
void OPCODES_LDR_r2_pc();
void OPCODES_SUBS_R0_1();
void OPCODES_BXNE_R1();
void OPCODES_BX_LR();

#ifdef __cplusplus
}
#endif

#endif