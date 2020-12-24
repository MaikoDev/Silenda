#include "pch.h"

#include "pipedworker.h"

namespace Silenda
{
	void ThreadPipedWorker::workerThread()
	{
		auto pipePtr = m_FunctionLine.get();
		while (m_isActive)
		{
			// Determine if the function pipeline is empty or not
			if (!pipePtr->empty())
			{
				// Retrieve functions within pipline
				auto processFunc = pipePtr->fetch();

				// Invoke function
				processFunc();
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}