#include "Menu.h"
#include <nds.h>
#include <stdio.h>
#include "CacheTests.h"
#include "MainMemoryTests.h"
#include "TCMTests.h"
#include "WRAMTests.h"
#include "VRAMTests.h"
#include "GBATests.h"

void PrintMainMenu()
{
  consoleClear();
  iprintf("            Main Menu           ");
  iprintf("--------------------------------");
  iprintf("\n\n\n");
  iprintf("   [ ] Caches\n");
  iprintf("   [ ] Main Memory\n");
  iprintf("   [ ] ITCM & DTCM\n");
  iprintf("   [ ] WRAM\n");
  iprintf("   [ ] VRAM\n");
  iprintf("   [ ] GBA Slot ROM\n");
  iprintf("   [ ] GBA Slot RAM\n");
  iprintf("\n\n\n") ;
}

void ShowSelector(int selected, int startLine, int startRow, int num)
{
  for (int i=0;i<num;i++)
  {
    iprintf("\33[%d;%dH", startLine + i, startRow) ;
    if (i == selected)
      iprintf("*") ;
    else
      iprintf(" ") ;
  }
}

uint8_t WaitForSelection(uint8_t selection, int startLine, int startRow, int num)
{
  ShowSelector(selection, startLine, startRow, num);
  do
  {
    swiWaitForVBlank();
    scanKeys() ;
    uint16_t keys_down = keysDown() ;
    if (!keys_down)
      continue;
    if (keys_down & KEY_UP)
    {
      selection = (selection + num - 1) % num ; 
    }

    if (keys_down & KEY_DOWN)
    {
      selection = (selection + 1) % num ; 
    }

    if (keys_down & KEY_A)
    {
      return selection;
    }
    
    ShowSelector(selection, startLine, startRow, num);
  } while (true);
}

void WaitForAnyKey()
{
  do
  {
    swiWaitForVBlank();
    scanKeys() ;
    uint16_t keys_down = keysDown() ;
    if (keys_down)
      return;
  } while (true) ;
}

void RunMainMenu()
{
  uint8_t selection = 0;
  do
  {
    PrintMainMenu() ;
    selection = WaitForSelection(selection, 5, 4, 7) ;
    consoleClear();

    switch (selection)
    {
      case 0: 
        RunCacheTests();
        break;
      case 1:
        RunMainMemoryTests();
        break;
      case 2:
        RunTCMTests();
        break;
      case 3:
        RunWRAMTests();
        break;
      case 4:
        RunVRAMTests();
        break;
      case 5:
        RunGBAROMTests();
        break;
      case 6:
        RunGBARAMTests();
        break;
    }
  } while (true);
}
