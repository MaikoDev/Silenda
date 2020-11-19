#include "cl_chat.h"

namespace Silenda
{
	G_ChatLog* G_ChatLog::m_Instance = nullptr;

	G_ChatLog* G_ChatLog::GetInstance()
	{
		if (m_Instance == nullptr)
			m_Instance = new G_ChatLog();
		return m_Instance;
	}

	const ChatMessage G_ChatLog::operator[](const size_t index)
	{
		m_Mutex.lock();
		ChatMessage ret = m_ChatLog[index];
		m_Mutex.unlock();

		return ret;
	}

	const void G_ChatLog::operator=(const std::vector<ChatMessage>& log)
	{
		m_Mutex.lock();
		m_ChatLog = log;
		m_Mutex.unlock();
	}

	const void G_ChatLog::push_back(const ChatMessage& message)
	{
		m_Mutex.lock();
		m_ChatLog.push_back(message);
		m_Mutex.unlock();
	}

	const void G_ChatLog::reserve(const size_t& size)
	{
		m_Mutex.lock();
		m_ChatLog.reserve(size);
		m_Mutex.unlock();
	}
}