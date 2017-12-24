/******************************************************************************
Project: Shared
Description: Nagisa Version Definition
File Name: NagisaVersion.h
License: The MIT License
******************************************************************************/

//#include "CIBuild.h"

#ifndef NAGISA_VER
#define NAGISA_VER

#define NAGISA_VER_MAJOR 0
#define NAGISA_VER_MINOR 1
#define NAGISA_VER_BUILD 4
#define NAGISA_VER_REV 0
#endif

#ifndef NAGISA_VER_FMT_COMMA
#define NAGISA_VER_FMT_COMMA NAGISA_VER_MAJOR,NAGISA_VER_MINOR,NAGISA_VER_BUILD,NAGISA_VER_REV
#endif

#ifndef NAGISA_VER_FMT_DOT
#define NAGISA_VER_FMT_DOT NAGISA_VER_MAJOR.NAGISA_VER_MINOR.NAGISA_VER_BUILD.NAGISA_VER_REV
#endif


#ifndef MACRO_TO_STRING
#define _MACRO_TO_STRING(arg) L#arg
#define MACRO_TO_STRING(arg) _MACRO_TO_STRING(arg)
#endif

#ifndef NAGISA_VERSION
#define NAGISA_VERSION NAGISA_VER_FMT_COMMA
#endif

#ifndef _NAGISA_VERSION_STRING_
#define _NAGISA_VERSION_STRING_ MACRO_TO_STRING(NAGISA_VER_FMT_DOT)
#endif

#ifndef NAGISA_VERSION_STRING
#ifdef NAGISA_CI_BUILD
#define NAGISA_VERSION_STRING _NAGISA_VERSION_STRING_ L" " NAGISA_CI_BUILD
#else
#define NAGISA_VERSION_STRING _NAGISA_VERSION_STRING_
#endif
#endif
