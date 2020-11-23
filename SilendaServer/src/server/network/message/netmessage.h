#pragma once

#include "types.h"

namespace SilendaServer
{
	struct NetworkMessage
	{
		NetworkMessage(const NetMessageType id = NetMessageType::count, const std::string msg = "") : identifier(id), payload(msg) {};

		NetMessageType identifier;
		std::string payload;

		MSGPACK_DEFINE(identifier, payload);
	};
}