#pragma once

#include "cl_message.h"

#include <mutex>

namespace Silenda
{
	// Global static chatlog. Thread-safe wrapper for: std::vector<ChatMessage>
	class G_ChatLog
	{
	public:
		static G_ChatLog* GetInstance();
		~G_ChatLog() {};

		const ChatMessage operator[](const size_t index);
		const void operator=(const std::vector<ChatMessage>& log);

		const void push_back(const ChatMessage& message);
		const void reserve(const size_t& size);

		const inline size_t& size() const& { return m_ChatLog.size(); };
	private:
		std::vector<ChatMessage> m_ChatLog;

		G_ChatLog() {};
		static G_ChatLog* m_Instance;

		std::mutex m_Mutex;
	};
}