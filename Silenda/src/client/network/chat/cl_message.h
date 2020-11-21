#pragma once

#include "cl_userlevel.h"

//typedef msgpack::type::tuple<std::wstring, unsigned int, __int64, std::wstring> ChatMessage;

/*struct ChatMessage
{
	std::string sender;
	UserLevel role;
	__int64 timesent;
	std::string message;
};*/

namespace Silenda
{
	struct ServerChatMessage
	{
		ServerChatMessage(const std::wstring senderName = L"", const UserLevel senderRole = UserLevel::user, const __int64 sentTime = 0, const std::wstring msg = L"") : sender(senderName), role(senderRole), timesent(sentTime), message(msg) {};

		std::wstring sender;
		UserLevel role;
		__int64 timesent;
		std::wstring message;

		MSGPACK_DEFINE(sender, role, timesent, message);
	};

	struct ClientChatMessage
	{
		ClientChatMessage(const std::wstring senderName = L"", const std::wstring msg = L"") : sender(senderName), message(msg) {};

		std::wstring sender;
		std::wstring message;

		MSGPACK_DEFINE(sender, message);
	};

	//std::vector<ChatMessage> G_ChatLog;
}