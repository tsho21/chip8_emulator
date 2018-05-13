#pragma once
#ifndef _DEBUG_H
#define _DEBUG_H

#include <iostream>
#include <sstream>
#include <Windows.h>
#define DBOUT( s )            \
{                             \
   std::ostringstream os_;    \
   os_ << s;                   \
   OutputDebugString( os_.str().c_str() );  \
}

#endif
