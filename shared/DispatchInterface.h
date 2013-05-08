//
//  DispatchInterface.h
//  Plugin
//
//  Created by CoronaLabs on 5/3/13.
//
//
#include "CoronaLua.h"

#ifndef Plugin_DispatchInterface_h
#define Plugin_DispatchInterface_h


namespace Corona
{
	
	class DispatchInterface
	{
	public:
		virtual void DoDispatch(lua_State *L) = 0;
	};
	
}

#endif
