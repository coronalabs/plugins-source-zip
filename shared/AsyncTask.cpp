//
//  AsyncTask.cpp
//  Plugin
//
//  Created by CoronaLabs on 4/29/13.
//
//

#include "AsyncTask.h"


#ifdef _WIN32
#include <pthread.h>
#endif

#include <sys/types.h>

namespace Corona
{

	static int condition_to_exit = 0;
	static pthread_cond_t processCondition	= PTHREAD_COND_INITIALIZER;
	static pthread_mutex_t processMutex		= PTHREAD_MUTEX_INITIALIZER;
	static pthread_mutex_t exitCondition	= PTHREAD_MUTEX_INITIALIZER;
	static pthread_mutex_t queueMutex		= PTHREAD_MUTEX_INITIALIZER;
	
	AsyncTaskQueue::AsyncTaskQueue()
	{
	}
	AsyncTaskQueue::~AsyncTaskQueue()
	{
		
		pthread_mutex_destroy(&processMutex);
		pthread_mutex_destroy(&exitCondition);
		pthread_mutex_destroy(&queueMutex);
	}
	
	void AsyncTaskQueue::WaitForCondition()
	{
		pthread_cond_wait(&(processCondition), &(processMutex));
	}
	
	void AsyncTaskQueue::SetCondition(int flag)
	{
		pthread_mutex_lock(&exitCondition);
		condition_to_exit = flag;
		pthread_mutex_unlock(&exitCondition);
		
		//Signal
		
		pthread_mutex_lock(&processMutex);
		pthread_cond_signal(&processCondition);
		pthread_mutex_unlock(&processMutex);
	}
	int AsyncTaskQueue::GetConditionToExit()
	{
		int condition = 0;
		pthread_mutex_lock(&exitCondition);
		condition = condition_to_exit;
		pthread_mutex_unlock(&exitCondition);
		return condition;
	}
	
	void AsyncTaskQueue::Start()
	{
		SetCondition(0);
		pthread_create( &fThread_id, NULL, AsyncTaskQueue_function, this );
	}
	void AsyncTaskQueue::End()
	{
		Cancel();
	}
	
	void AsyncTaskQueue::AddTask(AsyncTaskWithProxy* task)
	{
		pthread_mutex_lock(&queueMutex);
		fTasks.push(task);
		pthread_mutex_unlock(&queueMutex);
		
		pthread_cond_signal(&processCondition);
	}
	void AsyncTaskQueue::Cancel()
	{
		SetCondition(1);
		
		pthread_mutex_lock(&queueMutex);
		for (int i = 0; i < fTasks.size(); i++)
		{
			AsyncTaskWithProxy *task = fTasks.front();
			delete task;
			fTasks.pop();
		}
		pthread_mutex_unlock(&queueMutex);
		
		void* ret = NULL;
		pthread_join(fThread_id, &ret);
	}
	bool AsyncTaskQueue::HasTasks()
	{
		if (GetQueueCount() > 0)
			return true;
		
		return false;
	}
	int AsyncTaskQueue::GetQueueCount()
	{
		int tasks = 0;
		
		pthread_mutex_lock(&queueMutex);
		tasks = fTasks.size();
		pthread_mutex_unlock(&queueMutex);
		
		return tasks;
	}
	void AsyncTaskQueue::ExecuteFrontTask()
	{
		pthread_mutex_lock(&queueMutex);
		
		AsyncTaskWithProxy *cmd = static_cast<AsyncTaskWithProxy*>(fTasks.front());
		
		CommandInterface *currentTask = cmd->GetTask();
		currentTask->Execute(NULL);
		
		TaskCompleteProxy *taskProxy = cmd->GetProxy();
		taskProxy->TaskFinished(currentTask);
		
		delete cmd;
		
		fTasks.pop();
		
		pthread_mutex_unlock(&queueMutex);
	}
	
}
