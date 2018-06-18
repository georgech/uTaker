/******************************************************************************
  Target Script for STM32

  Copyright (c) 2008, 2010, 2011 Rowley Associates Limited.

  This file may be distributed under the terms of the License Agreement
  provided with this software.

  THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE
  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 ******************************************************************************/

function EnableETM()
{
  TargetInterface.pokeWord(0xE0042004, TargetInterface.peekWord(0xE0042004) | 0x000000E0); // DBG_MCU_CR : assign SYNC TRACE I/Os for SIZE=4
  TargetInterface.pokeWord(0xE00400F0, 0x00000000); // TPIU_SPPR : Synchronous Mode
  TargetInterface.pokeWord(0xE0040004, 0x00000008); // TPIU_CSPSR : Port Size = 4
}

function Reset()
{ 
  TargetInterface.pokeWord(0xE000EDF0, 0xA05F0003); // set C_HALT and C_DEBUGEN in DHCSR
  TargetInterface.waitForDebugState(1000);
  var DBGMCU_IDCODE = TargetInterface.peekWord(0xE0042000);
  TargetInterface.pokeWord(0xE000EDFC, 0x1); // set VC_CORERESET in DEMCR
  switch (DBGMCU_IDCODE & 0xffff0fff)
    { 
      case 0x410: // STM32F10xx8/STM32F10xxB Revision A devices
        TargetInterface.pokeWord(0xE000ED0C, 0x05FA0001); // set VECTRESET in AIRCR  
        break;
      default:
        TargetInterface.pokeWord(0xE000ED0C, 0x05FA0004); // set SYSRESETREQ in AIRCR 
        break;
    }
  TargetInterface.waitForDebugState(1000);  
  switch (DBGMCU_IDCODE & 0xfff)
    {
      case 0x411: // STM32F2xx
      case 0x416: // STM32L        
        TargetInterface.pokeWord(0xE0042004, 1<<2|1<<1|1<<0); // enable low-power mode debugging
        TargetInterface.pokeWord(0xE0042008, 1<<12|1<<11); // turn off WWDG and IWDG in debug mode
        break;
      default:
        TargetInterface.pokeWord(0xE0042004, 1<<8|1<<9|1<<2|1<<1|1<<0); // turn off WWDG and IWDG in debug mode and enable low-power mode debugging
    }
}

function SRAMReset()
{
  Reset();
}

function FLASHReset()
{
  Reset();
}

function GetPartName()
{ 
  switch ((TargetInterface.peekWord(0xE0042000) & 0xfff))
    {
      case 0x420: // low and medium density value 
        return "STM32F100x4/STM32F100x6/STM32F100x8/STM32F100xB";
      case 0x428: // high density value
        return "STM32F100xC/STM32F100xD/STM32F100xE";
      case 0x412: // low density
        return "STM32F10xx4/STM32F10xx6";
      case 0x410: // medium density   
        return "STM32F10xx8/STM32F10xxB";
      case 0x414: // high density
        return "STM32F10xxC/STM32F10xxD/STM32F10xxE";
      case 0x430: // xl density
        return "STM32F10xxF/STM32F10xxG";
      case 0x418: // connectivity
        return "STM32F105xx/STM32F107xx";
      case 0x411: // STM32F2xx/STM32F4xx
        if ((TargetInterface.peekWord(0xE000ED00) & 0xf0)==0x40)
          return "STM32F405xx/STM3F407xx/STM32F415xx/STM32F417xx";
        else
          return "STM32F205xx/STM3F207xx/STM32F215xx/STM32F217xx";
      case 0x416: // STM32L
        return "STM32L151xx/STM32L152xx";
    }
}

function MatchPartName(name)
{
  var family = name.substring(0, 9); // STM32F100, STM32F102, STM32F101, STM32F103, STM32F105, STM32F107, STM32F205, STM32F207, STM32F405, STM32F407, STM32L151, STM32L152
  var density = name.substring(10, 11); // 4==16K, 6==32K, 8==64K, B=128K, C=256K, D=384K, E=512K, F=768K, G=1024K
  switch ((TargetInterface.peekWord(0xE0042000) & 0xfff))
    {
      case 0x420: // low and medium density value 
        return (family=="STM32F100") && (density=="4" || density=="6" || density=="8" || density=="B");
      case 0x428: // high density value
        return  (family=="STM32F100") && (density=="C" || density=="D" || density=="E");
      case 0x412: // low density
        return (family=="STM32F101" || family=="STM32F102" || family=="STM32F103") && (density=="4" || density=="6");
      case 0x410: // medium density   
        return (family=="STM32F101" || family=="STM32F102" || family=="STM32F103") && (density =="8" || density=="B");
      case 0x414: // high density
        return (family=="STM32F101" || family=="STM32F102" || family=="STM32F103") && (density =="C" || density=="D" || density=="E");
      case 0x430: // xl density
        return (family=="STM32F101" || family=="STM32F103") && (density =="F" || density=="G");
      case 0x418: // connectivity
        return family=="STM32F105" || family=="STM32F107";
      case 0x411: // STM32F2xx
        return family=="STM32F205" || family=="STM32F207" || family=="STM32F215" || family=="STM32F217" || family == "STM32F405" || family == "STM32F407" || family == "STM32F415" || family == "STM32F417";
      case 0x416: // STM32L
        return family=="STM32L151" || family=="STM32L152";
    }
  return false;
}

//
// STM32F1xx option byte support
//
var FLASH_KEYR=0x40022004;
var FLASH_OPTKEYR=0x40022008;
var FLASH_SR=0x4002200C;
var FLASH_CR=0x40022010;

//
// Note this function will erase the RDP byte i.e. turn on read protection.
// Call STM32F_Unprotect() to turn off read protection after this has been called.
//
function STM32F_EraseOptionBytes()
{
  if (!TargetInterface.isStopped())
    TargetInterface.stopAndReset(400);
  // unlock FPEC
  TargetInterface.pokeWord(FLASH_KEYR, 0x45670123);
  TargetInterface.pokeWord(FLASH_KEYR, 0xCDEF89AB);
  // unlock OPTWRE
  TargetInterface.pokeWord(FLASH_OPTKEYR, 0x45670123);
  TargetInterface.pokeWord(FLASH_OPTKEYR, 0xCDEF89AB);
  // Erase option bytes
  var cr = TargetInterface.peekWord(FLASH_CR);
  cr |= (1<<5); // OPTER
  TargetInterface.pokeWord(FLASH_CR, cr);
  cr |= (1<<6); // STRT
  TargetInterface.pokeWord(FLASH_CR, cr);
  while (TargetInterface.peekWord(FLASH_SR) & 1);
  cr &= ~(1<<5); // ~OPTER
  TargetInterface.pokeWord(FLASH_CR, cr);
}

//
// Set the option byte at address to value
// STM32F_EraseOptionBytes() should been called before calling this function.
//
function STM32F_SetOptionByte(address, value)
{ 
  var cr = TargetInterface.peekWord(FLASH_CR);
  cr |= (1<<4); // OPTPG
  TargetInterface.pokeWord(FLASH_CR, cr);
  TargetInterface.pokeUint16(address, value);
  while (TargetInterface.peekWord(FLASH_SR) & 1);
  cr &= ~(1<<4); // ~FLASH_OPTPG
  TargetInterface.pokeWord(FLASH_CR, cr);
}

// 
// Turn off read protection
//
function STM32F_Unprotect()
{
  STM32F_SetOptionByte(0x1FFFF800, 0xA5);
}

// 
// Set user byte
//
function STM32F_SetUSER(b)
{
  STM32F_SetOptionByte(0x1FFFF802, b);
}

// 
// Set user data0
//
function STM32F_SetData0(b)
{
  STM32F_SetOptionByte(0x1FFFF804, b);
}

// 
// Set user data1
//
function STM32F_SetData1(b)
{
  STM32F_SetOptionByte(0x1FFFF806, b);
}

//
// STM32L option byte support
//
var FLASH2_PECR=0x40023C04;
var FLASH2_PEKEYR=0x40023C0C;
var FLASH2_OPTKEYR=0x40023C14;
var FLASH2_SR=0x40023C18;

function STM32L_SetOptionByte(address, value)
{
  if (!TargetInterface.isStopped())
    TargetInterface.stopAndReset(400);
  // Unlock PECR
  TargetInterface.pokeWord(FLASH2_PEKEYR, 0x89ABCDEF);
  TargetInterface.pokeWord(FLASH2_PEKEYR, 0x02030405);
  // Unlock option byte block
  TargetInterface.pokeWord(FLASH2_OPTKEYR, 0xFBEAD9C8);
  TargetInterface.pokeWord(FLASH2_OPTKEYR, 0x24252627);
  // Erase/Write option byte word
  var pecr = TargetInterface.peekWord(FLASH2_PECR);
  pecr |= (1<<8); // FTDW
  TargetInterface.pokeWord(FLASH2_PECR, pecr);
  TargetInterface.pokeWord(address, value);
  while (TargetInterface.peekWord(FLASH2_SR) & 1);
  pecr &= ~(1<<8); // ~FTDW
  TargetInterface.pokeWord(FLASH2_PECR, pecr);
}

function STM32L_MakeOptionWord(b0, b1)
{
  return (b0|b1<<8|((~b0)&0xff)<<16|((~b1)&0xff)<<24);
}

function STM32L_SetUSER(u)
{
  STM32L_SetOptionByte(0x1FF80004, STM32L_MakeOptionWord(u, 0));
}

//
// STM32F2xx option byte support
//
var FLASH3_KEYR=0x40023C04;
var FLASH3_OPTKEYR=0x40023C08;
var FLASH3_SR=0x40023C0C;
var FLASH3_CR=0x40023C10;
var FLASH3_OPTCR=0x40023C14;

//
// Set the USER option byte
//
function STM32F2xx_SetUSER(value)
{
  if (!TargetInterface.isStopped())
    TargetInterface.stopAndReset(400);
  // unlock OPTCR
  TargetInterface.pokeWord(FLASH3_OPTKEYR, 0x08192A3B);
  TargetInterface.pokeWord(FLASH3_OPTKEYR, 0x4C5D6E7F);

  // write USER option byte and then the OPTSTRT bit
  TargetInterface.pokeByte(FLASH3_OPTCR, value & 0xFC);
  TargetInterface.pokeByte(FLASH3_OPTCR, (value & 0xFC) | 2);
  // wait
  while (TargetInterface.peekWord(FLASH3_SR) & (1<<16));
}
