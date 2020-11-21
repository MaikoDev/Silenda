#pragma once

#include "cl_message.h"

namespace Silenda
{
	// Global static chatlog. Thread-safe wrapper for: std::vector<ChatMessage>
	class G_ChatLog
	{
	public:
		static G_ChatLog* GetInstance();
		~G_ChatLog() {};

		const ServerChatMessage operator[](const size_t index);
		const void operator=(const std::vector<ServerChatMessage>& log);

		const void push_back(const ServerChatMessage& message);
		const void reserve(const size_t& size);

		const inline size_t& size() const& { return m_ChatLog.size(); };
	private:
		std::vector<ServerChatMessage> m_ChatLog;

		G_ChatLog() {};
		static G_ChatLog* m_Instance;

		std::mutex m_Mutex;
	};
}