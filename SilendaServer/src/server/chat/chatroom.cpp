#include "pch.h"

#include "chatroom.h"
#include "utils/uuid.h"

namespace SilendaServer
{
	ChatRoom* ChatRoom::m_Instance = nullptr;

	ChatRoom::ChatRoom() : m_NumOfConnections(30)
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

	}

	const std::string ChatRoom::genNewUUID(const UserLevel& role)
	{
		std::string userUUID;

		do
		{
			userUUID = generate_uuid();

		} while (m_UserList.find(userUUID) != m_UserList.end());

		m_UserList[userUUID] = role;
		return userUUID;
	}

}