#pragma once

#include "userlevel.h"

namespace SilendaServer
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

	void to_json(nlohmann::json& j, const ServerChatMessage& message);
	void from_json(const nlohmann::json& j, ServerChatMessage& message);
}