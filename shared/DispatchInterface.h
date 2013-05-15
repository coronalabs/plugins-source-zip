//
//  DispatchInterface.h
//  Plugin
//
//  Created by CoronaLabs on 5/3/13.
//
//

#ifndef Plugin_DispatchInterface_h
#define Plugin_DispatchInterface_h

#include "CoronaLua.h"

namespace Corona
{
	
	class DispatchInterface
	{
	public:
		virtual void DoDispatch(lua_State *L) = 0;
	};
}

#endif
