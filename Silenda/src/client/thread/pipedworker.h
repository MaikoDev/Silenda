#pragma once

#include "pipeline.h"

namespace Silenda
{
	class ThreadPipedWorker
	{
		typedef std::shared_ptr<DataPipeline<std::function<void()>>> pipePtrType;
		typedef std::unique_ptr<std::thread> threadPtrType;
		typedef std::function<void()> processFuncType;
	public:
		ThreadPipedWorker()
		{
			m_FunctionLine = pipePtrType(new DataPipeline<std::function<void()>>);

			// Start working thread here
			m_WorkerThreadPtr = threadPtrType(new std::thread(&ThreadPipedWorker::workerThread, this));
		}

		~ThreadPipedWorker()
		{
			m_isActive = false;
			m_WorkerThreadPtr.get()->join();
		}

		void operator()(processFuncType processFunc) { m_FunctionLine.get()->push(processFunc); };

		std::shared_ptr<DataPipeline<processFuncType>> getPipeLine() { return m_FunctionLine; };
	private:
		void workerThread();
	private:
		pipePtrType m_FunctionLine;
		threadPtrType m_WorkerThreadPtr = nullptr;
		bool m_isActive = true;
	};
}