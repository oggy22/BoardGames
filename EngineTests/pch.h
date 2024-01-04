//
// pch.h
// Header for standard system include files.
//

#pragma once

#include "gtest/gtest.h"

#ifdef _DEBUG
#define DebugRelease(x,y) x
#else
#define DebugRelease(x,y) y
#endif
