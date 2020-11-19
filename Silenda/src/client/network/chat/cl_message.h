#pragma once

#include <string>
#include <vector>

#include "cl_userlevel.h"
#include "msgpack.hpp"

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
	struct ChatMessage
	{
		ChatMessage(const std::wstring senderName = L"", const UserLevel senderRole = UserLevel::user, const __int64 sentTime = 0, const std::wstring msg = L"") {};

		std::wstring sender;
		UserLevel role;
		__int64 timesent;
		std::wstring message;

		MSGPACK_DEFINE(sender, role, timesent, message);
	};

	//std::vector<ChatMessage> G_ChatLog;
}