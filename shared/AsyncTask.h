//
//  AsyncTask.h
//  Plugin
//
//  Created by CoronaLabs on 4/29/13.
//
//

#ifndef __Plugin__AsyncTask__
#define __Plugin__AsyncTask__

#include <iostream>

#include "CommandInterface.h"
#include "TaskCompleteProxy.h"
#include <queue>

#ifdef _WIN32
#include <pthread.h>
#endif

namespace Corona
{
	
	class AsyncTaskWithProxy : public CommandInterface
	{
		
	public:
		AsyncTaskWithProxy(CommandInterface *taskToRun, TaskCompleteProxy *proxy)
		{
			fTaskToRun = taskToRun;
			fProxy = proxy;
		}
		virtual ~AsyncTaskWithProxy()
		{
			fTaskToRun = NULL;
			fProxy = NULL;
		}
		
		virtual void Execute(CommandInterface *data)
		{
			fTaskToRun->Execute(data);
		}
		
		TaskCompleteProxy *GetProxy()
		{
			return fProxy;
		}
		
		CommandInterface *GetTask()
		{
			return fTaskToRun;
		}
		
		
	private:
		CommandInterface *fTaskToRun;
		TaskCompleteProxy *fProxy;
	};
	
	class AsynctaskQueueInterface
	{
		public:
			virtual int GetConditionToExit() = 0;
			virtual bool HasTasks() = 0;
			virtual void ExecuteFrontTask() = 0;
			virtual void WaitForCondition() = 0;
	};
	
	
	static void *AsyncTaskQueue_function(void *taskQueue);
	static void *AsyncTaskQueue_function(void *taskQueue)
	{
		
		//Execute the task
		AsynctaskQueueInterface *queue = static_cast<AsynctaskQueueInterface*>(taskQueue);
		
		while(queue->GetConditionToExit() == 0)
		{
			if (queue->HasTasks() > 0)
			{
				queue->ExecuteFrontTask();
			}
			else
			{
				queue->WaitForCondition();
			}
		}
		
		pthread_exit(0);
		
		return NULL;
		
	};
	

	class AsyncTaskQueue : public AsynctaskQueueInterface //public CommandInterface
	{

	public:
		AsyncTaskQueue();
		virtual ~AsyncTaskQueue();
	public:
		virtual void WaitForCondition();
		void SetCondition(int flag);
		virtual int GetConditionToExit();
		void Start();
		void End();
		void AddTask(AsyncTaskWithProxy* task);
		void Cancel();
		virtual bool HasTasks();
		int GetQueueCount();
		virtual void ExecuteFrontTask();

	private:
		pthread_t fThread_id;
		std::queue<AsyncTaskWithProxy*> fTasks;
		
	};
	
	
}
#endif /* defined(__Plugin__AsyncTask__) */
