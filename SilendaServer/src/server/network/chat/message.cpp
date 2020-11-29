#include "pch.h"

#include "message.h"

namespace SilendaServer
{
	void to_json(nlohmann::json& j, const ServerChatMessage& message)
	{
		j = nlohmann::json{
			{ "sender", message.sender },
			{ "role", message.role },
			{ "timesent", message.timesent },
			{ "message", message.message }
		};
	}

	void from_json(const nlohmann::json& j, ServerChatMessage& message)
	{
		j.at("sender").get_to(message.sender);
		j.at("role").get_to(message.role);
		j.at("timesent").get_to(message.timesent);
		j.at("message").get_to(message.message);
	}
}