#pragma once
#ifndef _DEBUG_H
#define _DEBUG_H

#include <iostream>
#include <sstream>
#include <Windows.h>

enum STATUS { OFF=0, ON=1 };
#define DEBUG ON

#define DBOUT( s )            \
{                             \
   std::ostringstream os_;    \
   os_ << s;                   \
   OutputDebugString( os_.str().c_str() );  \
}

#define DEFAULT_APP "D:\\dev\\C++\\Chip8\\roms\\games\\Space Invaders [David Winter].ch8"

#endif
