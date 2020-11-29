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
		~ChatRoom();

		const void push_back(const std::string& userUUID, const ClientChatMessage message);
		const inline ServerChatMessage& back() const& { return m_ChatLog.back(); };

		const bool join(const std::string& userUUID, const std::wstring& username);
		const void leave(const std::string& userUUID);
		const void save();

		const inline bool isActive(const std::string& userUUID) const& { return m_UserList.find(userUUID) != m_UserList.end(); };

		const std::string genNewUUID(const UserLevel& role = UserLevel::user);
		const bool findUUID(const std::string& uuid) { return (m_UserPermsList.find(uuid) != m_UserPermsList.end()); };

		const inline std::vector<ServerChatMessage>& GetMessageLog() const& { return m_ChatLog; };
		const inline unsigned int GetConnectionCount() const & { return m_UserList.size(); };
		const inline unsigned short GetMaxConnectCount() const & { return m_MaxConnections; };
	private:
		std::string m_RoomName;

		unsigned short m_MaxConnections;
		std::vector<ServerChatMessage> m_ChatLog;
		std::unordered_map<std::string, UserLevel> m_UserPermsList;

		std::unordered_map<std::string, std::wstring> m_UserList;
	private:
		ChatRoom();

		static ChatRoom* m_Instance;
	};

	struct FileChatLog
	{
		std::vector<ServerChatMessage> log;
	};

	struct FilePermList
	{
		std::unordered_map<std::string, UserLevel> permlist;
	};

}