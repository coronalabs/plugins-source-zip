//
//  CommandInterface.h
//  Plugin
//
//  Created by CoronaLabs on 4/29/13.
//
//

#ifndef __Plugin__CommandInterface__
#define __Plugin__CommandInterface__

namespace Corona
{
	class CommandInterface
	{
		
	public:
		virtual void Execute(CommandInterface *data) = 0;
		
	};
}

#endif /* defined(__Plugin__CommandInterface__) */
