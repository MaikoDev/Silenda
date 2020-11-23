#pragma once

#include "network/chat/userlevel.h"
#include "network/chat/message.h"
#include "chatuser.h"

namespace SilendaServer
{
	class ChatRoom
	{
	public:
		static ChatRoom* GetInstance();
		~ChatRoom() {};

		const void push_back(const std::string& userUUID, const ClientChatMessage message);

		const std::string genNewUUID(const UserLevel& role = UserLevel::user);
		const bool findUUID(const std::string& uuid) { return (m_UserList.find(uuid) != m_UserList.end()); };
	private:
		std::string m_RoomName;

		unsigned short m_NumOfConnections;
		std::vector<ServerChatMessage> m_ChatLog;
		std::unordered_map<std::string, UserLevel> m_UserList;
	private:
		ChatRoom();

		static ChatRoom* m_Instance;
	};
}