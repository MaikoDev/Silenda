#pragma once

#include "pipedworker.h"

namespace Silenda
{
	class ThreadPoolExecutor
	{
	public:
		static ThreadPoolExecutor* GetInstance();

		~ThreadPoolExecutor()
		{
			m_ExecutionRunning = false;
			m_ControlThread.join();
		}

		template<typename Func, class... ArgTypes>
		void scheduleProcess(Func&& funcPtr, ArgTypes&& ... args)
		{
			std::function<void()> processFunc = std::bind(std::forward<Func>(funcPtr), std::ref(args)...);

			m_ProcessQueue.push(processFunc);
		}
	private:
		void initWorkers();
		void controllerThread();
	private:
		std::thread m_ControlThread;
		bool m_ExecutionRunning = true;

		std::unique_ptr<ThreadPipedWorker[]> m_WorkerPool;
		unsigned int m_WorkerThreadCount = 0;
		unsigned int m_WorkerAssignmentPointer = 0;

		std::queue<std::function<void()>> m_ProcessQueue;
	private:
		ThreadPoolExecutor() 
		{
			this->initWorkers();
			m_ControlThread = std::thread(&ThreadPoolExecutor::controllerThread, this);
		};
		static ThreadPoolExecutor* m_Instance;
	};
}