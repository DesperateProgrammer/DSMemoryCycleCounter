#ifndef MENU_H
#define MENU_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

void PrintMainMenu();
void ShowSelector(int selected, int startLine, int startRow, int num);
uint8_t WaitForSelection(uint8_t selection, int startLine, int startRow, int num);
void WaitForAnyKey();
void RunMainMenu();

#ifdef __cplusplus
}
#endif

#endif