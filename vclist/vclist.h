/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

#define IDC_ARCHITECTURE_BASE_X86 0x100
#define IDC_ARCHITECTURE_BASE_X64 0x110

#define IDC_INTERFACE_OFFSET_VCM 0
#define IDC_INTERFACE_OFFSET_DMO 1
#define IDC_INTERFACE_OFFSET_DSF 2
#define IDC_INTERFACE_OFFSET_END 3

#ifdef _WIN64
 #ifdef _M_X64
  #ifndef _WIN64_X64
   #define _WIN64_X64
  #endif
 #else
  #error This platform is not supported.
 #endif
#else
#ifdef _WIN32
 #ifdef _M_IX86
  #ifndef _WIN32_X86
   #define _WIN32_X86
  #endif
 #else
  #error This platform is not supported.
 #endif
#endif
#endif

#ifdef _WIN32_X86
#define IDC_ARCHITECTURE_BASE IDC_ARCHITECTURE_BASE_X86
#endif

#ifdef _WIN64_X64
#define IDC_ARCHITECTURE_BASE IDC_ARCHITECTURE_BASE_X64
#endif
