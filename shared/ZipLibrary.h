// ----------------------------------------------------------------------------
// 
// ZipLibrary.h
// Copyright (c) 2013 Corona Labs Inc. All rights reserved.
// 
// ----------------------------------------------------------------------------

#ifndef _ZipLibraryLibrary_H__
#define _ZipLibraryLibrary_H__

#include "CoronaLua.h"
#include "CoronaMacros.h"

// This corresponds to the name of the library, e.g. [Lua] require "plugin.zip"
// where the '.' is replaced with '_'
CORONA_EXPORT int luaopen_plugin_zip( lua_State *L );

#endif // _ZipLibrary_H__
