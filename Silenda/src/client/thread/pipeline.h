#pragma once

#include <queue>

namespace Silenda
{
	template<typename T>
	class DataPipeline
	{
	public:
		DataPipeline() = default;
		DataPipeline(const DataPipeline& origin) = delete;
		DataPipeline(DataPipeline&& origin) noexcept
		{
			this->m_DataPipe.swap(origin.m_DataPipe);
		}

		~DataPipeline() = default;

		DataPipeline& operator=(const DataPipeline& other) = delete;
		DataPipeline& operator=(DataPipeline&& other)
		{
			this->m_DataPipe.swap(other.m_DataPipe);
			return *this;
		}

		// push(param1)
		// @param1 Data to be pushed onto the pipeline.
		void push(T& data) { m_DataPipe.push(data); };

		// fetch()
		// Fetches data off pipeline.
		// @IMPORTANT after fetching, data is no longer present on pipeline.
		T fetch() { T feteched = m_DataPipe.front(); m_DataPipe.pop(); return feteched; };

		// flush()
		// Flushes all data off pipeline.
		void flush() { std::queue<T> empty; m_DataPipe.swap(empty); };

		// empty()
		// Returns whether the pipeline is empty or not.
		bool empty() { return m_DataPipe.empty(); };
	private:
		std::queue<T> m_DataPipe;
	};
}