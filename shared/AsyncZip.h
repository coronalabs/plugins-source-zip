//
//  AsyncZip.h
//  Plugin
//
//  Created by CoronaLabs on 4/18/13.
//
//



#ifndef __Plugin__AsyncZip__
#define __Plugin__AsyncZip__

#include "ZipTask.h"
#include "CoronaLua.h"
#include "AsyncTask.h"
#include "TaskCompleteProxy.h"
#include "ZipEvent.h"
#include <queue>

namespace Corona
{
	
	class AsyncZip : public TaskCompleteProxy
	{
	public:
		typedef AsyncZip Self;
		
	public:
		AsyncZip( lua_State *L );
		virtual ~AsyncZip();
		
	protected:
		const char *GetPath(lua_State *L, void* baseDir);
		bool Initialize( char *appId );
		
	public:
		void Dispatch( const ZipEvent& e,  CoronaLuaRef lRef) const;
		void Finalize( lua_State *L );
		
	public:
		virtual void ProcessFrame(lua_State *L);
		virtual void Compress( lua_State *L );
		virtual void Uncompress( lua_State *L );
		virtual void List( lua_State *L );
		

		virtual void TaskFinished(void *data);
		
	private:
		AsyncTaskQueue asyncTask;
		std::queue<ZipTask*> fFinishedTasks;
		lua_State *_L;
		int fFrameCounter;

	};
}



#endif /* defined(__Plugin__AsyncZip__) */
