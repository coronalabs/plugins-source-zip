//
//  AsyncZip.cpp
//  Plugin
//
//  Created by CoronaLabs on 4/18/13.
//
//

#include "AsyncZip.h"
#include "LuaReader.h"

namespace Corona
{

	static pthread_mutex_t queueLock = PTHREAD_MUTEX_INITIALIZER;

	AsyncZip::AsyncZip(lua_State *L )
	: _L( L )
	{
		pthread_mutex_init(&queueLock, NULL);
		fFrameCounter = 0;
		
		asyncTask.Start();
	}
	AsyncZip::~AsyncZip()
	{
		
		pthread_mutex_destroy(&queueLock);
	}
	bool AsyncZip::Initialize( char *appId )
	{
		return true;
	}
	void AsyncZip::Finalize( lua_State *L )
	{
	
		//Block/wait, tasks finishing will post their result
		//to TaskFinished within this call
		asyncTask.End();
	
		//If the last thread just posted something to the queue
		pthread_mutex_lock(&queueLock);
		
		for (int i = 0; i < fFinishedTasks.size(); i++)
		{
			ZipTask *zipCmd = static_cast<ZipTask*>(fFinishedTasks.front());
			zipCmd->Finalize(L);
			delete zipCmd;
			fFinishedTasks.pop();
		}
		
		_L = NULL;
				
		pthread_mutex_unlock(&queueLock);
	}
	void AsyncZip::TaskFinished(void *data)
	{
		
		pthread_mutex_lock(&queueLock);
		fFinishedTasks.push(static_cast<ZipTask*>(data));
		pthread_mutex_unlock(&queueLock);
		
	}
	
	//Only dispatch events from the main queue!
	void AsyncZip::ProcessFrame(lua_State *L)
	{
		//Only process this every 15 frames
		fFrameCounter++;
		if (fFrameCounter >= 50)
		{
			pthread_mutex_lock(&queueLock);
			if (fFinishedTasks.size() > 0)
			{
				ZipTask *zipCmd = static_cast<ZipTask*>(fFinishedTasks.front());
				
				if (_L != NULL)
				{
					zipCmd->DoDispatch(L);
					zipCmd->Finalize(L);
				}
				delete zipCmd;
				
				fFinishedTasks.pop();
			}
			pthread_mutex_unlock(&queueLock);
			
			fFrameCounter = 0;
		}
	}
	void AsyncZip::Uncompress( lua_State *L )
	{
		
		if (lua_istable( L, -1 ) == false)
		{
			CoronaLog("Uncompress Error: Missing input options\n");
			return;
		}

		LMap paramsMap		= LuaReader::GetDataMap(L,1);
		
		
		LDataString *archive = static_cast<LDataString*>(paramsMap.GetData("zipFile"));
		
		if (archive == NULL)
		{
			CoronaLog("Uncompress Error: Missing zipFile parameter\n");
			return;
		}
		
		
		LDataBool *flattenOutput = static_cast<LDataBool*>(paramsMap.GetData("flattenOutput"));
		
		bool bFlattenOutput = false;
		if (flattenOutput != NULL)
		{
			bFlattenOutput = flattenOutput->GetBool();
		}
		
		LDataLUD *srcPath = static_cast<LDataLUD*>(paramsMap.GetData("zipBaseDir"));
		
		if (srcPath == NULL)
		{
			CoronaLog("Uncompress Error: Missing zipBaseDir parameter\n");
			return;
		}
		
		std::string fileName = archive->GetStr();
		const char *path = LuaReader::GetPathForFileBaseDir(L, srcPath->GetData(),fileName.c_str());
		
		if (path == NULL)
		{
			CoronaLog("Uncompress Error: Path to Zip file is invalid\n");
			return;
		}
		
		LDataLUD *dstPath = static_cast<LDataLUD*>(paramsMap.GetData("dstBaseDir"));
		
		if (dstPath == NULL)
		{
			CoronaLog("Uncompress Error: Missing dstBaseDir parameter\n");
			return;
		}
		
		LDataListener *listener = static_cast<LDataListener*>(paramsMap.GetData("listener"));
		
		if (listener == NULL)
		{
			CoronaLog("Uncompress Error: Missing listener parameter\n");
			return;
		}
		

		
		LMap *fileNames = NULL;
		
		//Optional parameter
		LMap *files = static_cast<LMap*>(paramsMap.GetData("files"));
		if (files != NULL)
		{
			fileNames = static_cast<LMap*>(files->GetCopy());
		}
		
		

		
		std::string pathSource	= path;
		
		std::string outputDir	= LuaReader::GetPathForFileBaseDir(L, dstPath->GetData(),NULL);
		CoronaLuaRef coronaListener = listener->GetListener();
		

		
		ZipTaskExtract *zipTask = new ZipTaskExtract(	pathSource,
														outputDir,
														fileNames,
														bFlattenOutput,
														coronaListener);
		
		AsyncTaskWithProxy *taskWithProxy = new AsyncTaskWithProxy(zipTask,this);
		
		asyncTask.AddTask(taskWithProxy);
		
	}
	void AsyncZip::List( lua_State *L )
	{
		
		if (lua_istable( L, -1 ) == false)
		{
			CoronaLog("List Error: Missing input options\n");
			return;
		}
		
		LMap paramsMap		= LuaReader::GetDataMap(L,1);
		
		LDataString *archive = static_cast<LDataString*>(paramsMap.GetData("zipFile"));
		
		if (archive == NULL)
		{
			CoronaLog("List Error: Missing zipFile parameter\n");
			return;
		}
		
		
		LDataLUD *srcPath = static_cast<LDataLUD*>(paramsMap.GetData("zipBaseDir"));
		
		if (srcPath == NULL)
		{
			CoronaLog("List Error: Missing zipBaseDir parameter\n");
			return;
		}
		
		
		std::string fileName = archive->GetStr();
		const char *path = LuaReader::GetPathForFileBaseDir(L, srcPath->GetData(),fileName.c_str());
		
		if (path == NULL)
		{
			CoronaLog("List Error: Path to Zip file is invalid\n");
			return;
		}
		
		
		LDataListener *listener = static_cast<LDataListener*>(paramsMap.GetData("listener"));
		
		if (listener == NULL)
		{
			CoronaLog("List Error: Missing listener parameter\n");
			return;
		}
		

		std::string pathSource	= path;
		CoronaLuaRef coronaListener = listener->GetListener();
		
		ZipTaskListAllFilesInZip *zipTask = new ZipTaskListAllFilesInZip(	pathSource,
																		 coronaListener);
		
		AsyncTaskWithProxy *taskWithProxy = new AsyncTaskWithProxy(zipTask,this);
		
		asyncTask.AddTask(taskWithProxy);
		
	}
	void AsyncZip::Compress( lua_State *L )
	{
		
		if (lua_istable( L, -1 ) == false)
		{
			CoronaLog("Compress Error: Missing input options\n");
			return;
		}
		
		LMap paramsMap		= LuaReader::GetDataMap(L,1);
		LDataString *archive = static_cast<LDataString*>(paramsMap.GetData("zipFile"));
		
		if (archive == NULL)
		{
			CoronaLog("Compress Error: Missing zipFile parameter\n");
			return;
		}
		
		LDataLUD *srcFilesPath = static_cast<LDataLUD*>(paramsMap.GetData("srcBaseDir"));
		
		if (srcFilesPath == NULL)
		{
			CoronaLog("Compress Error: Missing srcBaseDir parameter\n");
			return;
		}
		
		LDataLUD *srcPath = static_cast<LDataLUD*>(paramsMap.GetData("zipBaseDir"));
		
		if (srcPath == NULL)
		{
			CoronaLog("Compress Error: Missing zipBaseDir parameter\n");
			return;
		}
		
		LDataListener *listener = static_cast<LDataListener*>(paramsMap.GetData("listener"));
		
		if (listener == NULL)
		{
			CoronaLog("Compress Error: Missing listener parameter\n");
			return;
		}
		
		std::string fileName = archive->GetStr();
		std::string archivePath	= LuaReader::GetPathForFileBaseDir(L,srcPath->GetData(),fileName.c_str());
		
		
		LVector fileList;
		LMap *fileNames	= static_cast<LMap*>(paramsMap.GetData("srcFiles"));
		
		if (fileNames != NULL)
		{
			std::vector<std::string> keys = fileNames->GetKeys();
			for (int i = 0; i < keys.size(); i++)
			{
				LDataString *curFile = static_cast<LDataString*>(fileNames->GetData(keys[i]));
				
				const char *path =  LuaReader::GetPathForFileBaseDir(L,srcFilesPath->GetData(),curFile->GetStr().c_str());
				if (path != NULL)
				{
					std::string fullPath = path;
					fileList.Push(fullPath);
				}
				else
				{
					CoronaLog("Compress Error: Missing one or more source files\n");
				}
			}
		}
		else
		{
			CoronaLog("Compress Error: Missing srcFiles parameter\n");
			return;
		}
		
		CoronaLuaRef coronaListener = listener->GetListener();
		
		ZipTaskAddFileToZip *zipTask = new ZipTaskAddFileToZip( archivePath,
															   fileList,
															   coronaListener);
		
		AsyncTaskWithProxy *taskWithProxy = new AsyncTaskWithProxy(zipTask,this);
		
		
		asyncTask.AddTask(taskWithProxy);
		
	}
	
	void
	AsyncZip::Dispatch( const ZipEvent& e,  CoronaLuaRef lRef) const
	{
		e.Dispatch( _L, lRef);
	}
	
	// -----
	
} // namespace Corona

