// ----------------------------------------------------------------------------
// 
// ZipLibrary.cpp
// Copyright (c) 2013 Corona Labs Inc. All rights reserved.
// 
// ----------------------------------------------------------------------------

#include "ZipLibrary.h"
#include "AsyncZip.h"
#include "CoronaLibrary.h"
#include "CoronaLua.h"

#include <stdlib.h>

// ----------------------------------------------------------------------------

namespace Corona
{

// ----------------------------------------------------------------------------

class ZipLibrary
{
	public:
		typedef ZipLibrary Self;

	public:
		static const char kName[];

	protected:
		ZipLibrary( lua_State *L );
		virtual ~ZipLibrary();

	public:
		static int Open( lua_State *L );

	protected:
		static int Initialize( lua_State *L );
		static int Finalizer( lua_State *L );

		static int ProcessFrame( lua_State *L );
		
		AsyncZip *GetTaskHandler(){return fZipHandler;};
		

	public:
		static Self *ToLibrary( lua_State *L );

	public:
		static int zipCompress( lua_State *L );
		static int zipUncompress( lua_State *L );
		static int zipList( lua_State *L );
	
	private:
		AsyncZip *fZipHandler;
		
	

};

// ----------------------------------------------------------------------------

} // namespace Corona

// ----------------------------------------------------------------------------




// ----------------------------------------------------------------------------

namespace Corona
{

// ----------------------------------------------------------------------------

// This corresponds to the name of the library, e.g. [Lua] require "plugin.library"
const char ZipLibrary::kName[] = "plugin.zip";

static int callbackRef = 0;

ZipLibrary::ZipLibrary( lua_State *L )
{
	fZipHandler = new AsyncZip(L);
}
ZipLibrary::~ZipLibrary()
{
	delete fZipHandler;
}

int
ZipLibrary::Open( lua_State *L )
{
	// Register __gc callback
	const char kMetatableName[] = __FILE__; // Globally unique string to prevent collision
	CoronaLuaInitializeGCMetatable( L, kMetatableName, Finalizer );

	// Functions in library
	const luaL_Reg kVTable[] =
	{
		{ "compress", zipCompress },
		{ "uncompress", zipUncompress },
		{ "list", zipList },

		{ NULL, NULL }
	};

	// Set library as upvalue for each library function
	Self *library = new Self( L );

	// Store the library singleton in the registry so it persists
	// using kMetatableName as the unique key.
	CoronaLuaPushUserdata( L, library, kMetatableName );
	lua_pushstring(L, kMetatableName);
	lua_settable(L, LUA_REGISTRYINDEX);

	// Does the equivalent of the following Lua code:
	//   Runtime:addEventListener( "enterFrame", ProcessFrame )
	// which is equivalent to:
	//   local f = Runtime.addEventListener
	//   f( Runtime, "enterFrame", ProcessFrame )
	//
	// Now invoke above from C:
	// Lua stack order (from lowest index to highest):
	//   f
	//   Runtime
	//   "enterFrame"
	//   ProcessFrame (closure)
	{
		CoronaLuaPushRuntime( L ); // push 'Runtime'
		lua_getfield( L, -1, "addEventListener" ); // push 'f', i.e. Runtime.addEventListener
		lua_insert( L, -2 ); // swap so 'f' is below 'Runtime'
		lua_pushstring( L, "enterFrame" );

		// Push ProcessFrame as closure so it has access
		lua_pushlightuserdata( L, library );
		lua_pushcclosure( L, & ProcessFrame, 1 );
		
		lua_pushvalue( L, -1 );
		callbackRef = luaL_ref( L, LUA_REGISTRYINDEX ); // r = clo
	}
	CoronaLuaDoCall( L, 3, 0 );

	// Leave "library" on top of stack
	// Set library as upvalue for each library function
	int result = CoronaLibraryNew( L, kName, "com.coronalabs", 1, 1, kVTable, library );

	return result;
}

int
ZipLibrary::Finalizer( lua_State *L )
{
	
	//Get rid of closure
		CoronaLuaPushRuntime( L ); // push 'Runtime'
		
		if ( lua_type( L, -1 ) == LUA_TTABLE )
		{
			lua_getfield( L, -1, "removeEventListener" ); // push 'f', i.e. Runtime.addEventListener
			lua_insert( L, -2 ); // swap so 'f' is below 'Runtime'
			lua_pushstring( L, "enterFrame" );
			lua_rawgeti( L, LUA_REGISTRYINDEX, callbackRef );// pushes closure
			CoronaLuaDoCall( L, 3, 0 );
			luaL_unref(L, LUA_REGISTRYINDEX,  callbackRef);
		}
		else
		{
			lua_pop( L, 1 ); // pop nil
		}
	
	

	Self *library = (Self *)CoronaLuaToUserdata( L, 1 );

	library->GetTaskHandler()->Finalize( L );

	delete library;

	return 0;
}

int
ZipLibrary::ProcessFrame( lua_State *L )
{
	Self *library = ToLibrary( L );
	
	AsyncZip *handler = library->GetTaskHandler();
	handler->ProcessFrame(L);

	return 0;
}

ZipLibrary *
ZipLibrary::ToLibrary( lua_State *L )
{
	// library is pushed as part of the closure
	Self *library = (Self *)lua_touserdata( L, lua_upvalueindex( 1 ) );
	return library;
}

// [Lua] zip()
int
ZipLibrary::zipCompress( lua_State *L )
{
	Self *library = ToLibrary( L );
	
	AsyncZip *handler = library->GetTaskHandler();

	handler->Compress(L);


	return 0;
}

// [Lua] zip( path [, httpMethod, params] )
int
ZipLibrary::zipUncompress( lua_State *L )
{
	Self *library = ToLibrary( L );
	
	AsyncZip *handler = library->GetTaskHandler();
	
	handler->Uncompress(L);

	return 0;
}

int
ZipLibrary::zipList( lua_State *L )
{
	Self *library = ToLibrary( L );
	
	AsyncZip *handler = library->GetTaskHandler();

	handler->List(L);

	return 0;
}


// ----------------------------------------------------------------------------

} // namespace Corona

// ----------------------------------------------------------------------------

CORONA_EXPORT int luaopen_plugin_zip( lua_State *L )
{
	return Corona::ZipLibrary::Open( L );
}
