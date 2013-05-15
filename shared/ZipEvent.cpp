// ----------------------------------------------------------------------------
// 
// FBConnectEvent.cpp
// Copyright (c) 2013 Corona Labs Inc. All rights reserved.
// 
// Reviewers:
// 		Walter
//
// ----------------------------------------------------------------------------

#include "ZipEvent.h"
#include "CoronaAssert.h"
#include "CoronaEvent.h"
#include "CoronaLua.h"

// ----------------------------------------------------------------------------

namespace Corona
{
	
	// ----------------------------------------------------------------------------
	
	const char ZipEvent::kName[] = "zip";
	
	// ----------------------------------------------------------------------------
	
	const char *
	ZipEvent::StringForType( Type t )
	{
		const char *result = NULL;
		static const char kUnZip[] = "uncompress";
		static const char kZip[] = "compress";
		static const char kFileList[] = "list";
		
		switch( t )
		{
			case kUnZipType:
				result = kUnZip;
				break;
			case kZipType:
				result = kZip;
				break;
			case kFileType:
				result = kFileList;
				break;
			default:
				CORONA_ASSERT_NOT_REACHED();
				break;
		}
		
		return result;
	}
	
	bool
	ZipEvent::IsListener( lua_State *L, int listenerIndex )
	{
		return CoronaLuaIsListener( L, listenerIndex, kName );
	}
	
	ZipEvent::ZipEvent( Type t )
	:fResponse( NULL ),
	fType( t ),
	fIsError( false )
	{
	}
	
	ZipEvent::ZipEvent( Type t, const char *response, bool isError )
	:fResponse( response ),
	fType( t ),
	fIsError( isError )
	{
	}
	
	void
	ZipEvent::Dispatch( lua_State *L, CoronaLuaRef listener ) const
	{
		if ( CORONA_VERIFY( listener ) )
		{
			//Push( L );
			CoronaLuaDispatchEvent( L, listener, 0 );
		}
	}
	
	void
	ZipEvent::Push( lua_State *L ) const
	{
		CoronaLuaNewEvent( L, kName ); CORONA_ASSERT( lua_istable( L, -1 ) );
		
		const char *message = fResponse ? fResponse : "";
		lua_pushstring( L, message );
		lua_setfield( L, -2, "errorMessage" );
		
		const char *value = StringForType( (Type)fType ); CORONA_ASSERT( value );
		lua_pushstring( L, value );
		lua_setfield( L, -2, "type");//CoronaEventTypeKey() ); //These keys are not available on Windows, fix this moving forward
		
		lua_pushboolean( L, fIsError );
		lua_setfield( L, -2, "isError");//CoronaEventIsErrorKey() );
		
	}
	
	
	// ----------------------------------------------------------------------------
	
} // namespace Corona

// ----------------------------------------------------------------------------

