#include "pch.h"

#include "chatroom.h"
#include "fileio/fio.h"
#include "utils/uuid.h"

namespace SilendaServer
{
	ChatRoom* ChatRoom::m_Instance = nullptr;

	ChatRoom::ChatRoom() : m_MaxConnections(30)
	{
		FIO* IOptr = FIO::GetInstance();

		FileChatLog chatlog;
		FilePermList perms;

		IOptr->readObjFromFile("messagelog.txt", chatlog);
		IOptr->readObjFromFile("perms.cfg", perms);

		m_ChatLog = chatlog.log;
		m_UserPermsList = perms.permlist;
	}

	ChatRoom::~ChatRoom()
	{
	}

	ChatRoom* ChatRoom::GetInstance()
	{
		if (m_Instance == nullptr)
			m_Instance = new ChatRoom();
		return m_Instance;
	}

	const void ChatRoom::push_back(const std::string& userUUID, const ClientChatMessage message)
	{
		if (m_UserPermsList.find(userUUID) != m_UserPermsList.end())
			m_ChatLog.push_back({ message.sender, m_UserPermsList[userUUID], time(0), message.message });
	}

	const bool ChatRoom::join(const std::string& userUUID, const std::wstring& username)
	{
		auto iter = m_UserPermsList.find(userUUID);
		if (iter != m_UserPermsList.end())
		{
			if (m_UserList.find(userUUID) == m_UserList.end())
			{
				m_UserList[userUUID] = username;
				return 1;
			}
		}

		return 0;
	}

	const void ChatRoom::leave(const std::string& userUUID)
	{
		auto iter = m_UserList.find(userUUID);
		if (iter != m_UserList.end())
			m_UserList.erase(iter);
	}

	const void ChatRoom::save()
	{
		FIO* IOptr = FIO::GetInstance();

		FileChatLog chatlog;
		chatlog.log = m_ChatLog;

		FilePermList perms;
		perms.permlist = m_UserPermsList;

		IOptr->writeObjToFile("messagelog.txt", chatlog);
		IOptr->writeObjToFile("perms.cfg", perms);
	}

	const std::string ChatRoom::genNewUUID(const UserLevel& role)
	{
		std::string userUUID;

		do
		{
			userUUID = generate_uuid();

		} while (m_UserPermsList.find(userUUID) != m_UserPermsList.end());

		m_UserPermsList[userUUID] = role;
		return userUUID;
	}

	void to_json(nlohmann::json& j, const FileChatLog& chatlog)
	{
		j = nlohmann::json{
			{ "log", chatlog.log }
		};
	}

	void to_json(nlohmann::json& j, const FilePermList& perms)
	{
		j = nlohmann::json{
			{ "perms", perms.permlist }
		};
	}

	void from_json(const nlohmann::json& j, FileChatLog& chatlog)
	{
		j.at("log").get_to(chatlog.log);
	}

	void from_json(const nlohmann::json& j, FilePermList& perms)
	{
		j.at("perms").get_to(perms.permlist);
	}
}