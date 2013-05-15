// ----------------------------------------------------------------------------
// 
// FBConnectEvent.h
// Copyright (c) 2013 Corona Labs Inc. All rights reserved.
// 
// Reviewers:
// 		Walter
//
// ----------------------------------------------------------------------------

#ifndef _ZipEvent_H__
#define _ZipEvent_H__

#include "CoronaLua.h"
#include <sys/types.h>

// ----------------------------------------------------------------------------

struct lua_State;

namespace Corona
{

// ----------------------------------------------------------------------------

class ZipEvent
{
	public:
		typedef ZipEvent Self;

	public:
		static const char kName[];
		static bool IsListener( lua_State *L, int listenerIndex );

	public:
		typedef enum Type
		{
			kUnZipType = 0,
			kZipType = 1,
			kFileType = 2,
			kNumTypes
		}
		Type;

		static const char *StringForType( Type t );

	//protected:
		ZipEvent( Type t );
		ZipEvent( Type t, const char *response, bool isError );

	public:
		void Dispatch( lua_State *L, CoronaLuaRef listener ) const;

	//protected:
		virtual void Push( lua_State *L ) const;
		
	private:
		const char *fResponse;
		int fType;
		bool fIsError;

};



// ----------------------------------------------------------------------------

} // namespace Corona

// ----------------------------------------------------------------------------

#endif // _ZipEvent_H__
