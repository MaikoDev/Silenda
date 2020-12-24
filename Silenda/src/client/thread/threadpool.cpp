#include "pch.h"

#include "threadpool.h"
#include "utils/iobserver.h"

namespace Silenda
{
	ThreadPoolExecutor* ThreadPoolExecutor::m_Instance = nullptr;

	ThreadPoolExecutor* ThreadPoolExecutor::GetInstance()
	{
		if (m_Instance == nullptr)
			m_Instance = new ThreadPoolExecutor();
		return m_Instance;
	}

	void ThreadPoolExecutor::initWorkers()
	{
		if (m_WorkerPool.get() == nullptr)
		{
			const unsigned int processorCount = std::thread::hardware_concurrency();
			if (processorCount == 0)
				m_WorkerThreadCount = 1;
			else
				m_WorkerThreadCount = processorCount;

			m_WorkerPool = std::make_unique<ThreadPipedWorker[]>(m_WorkerThreadCount);
		}
	}

	void ThreadPoolExecutor::controllerThread()
	{
		while (m_ExecutionRunning)
		{
			if (!m_ProcessQueue.empty())
			{
				if (m_WorkerAssignmentPointer >= m_WorkerThreadCount)
					m_WorkerAssignmentPointer = 0;

				m_WorkerPool.get()[m_WorkerAssignmentPointer].getPipeLine()->push(m_ProcessQueue.front());

				m_ProcessQueue.pop();
				m_WorkerAssignmentPointer++;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}