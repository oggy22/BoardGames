//
// pch.h
// Header for standard system include files.
//

#pragma once

#include "gtest/gtest.h"
#include <iostream>
#include <fstream>

#ifdef _DEBUG
#define DebugRelease(x,y) x
#else
#define DebugRelease(x,y) y
#endif
