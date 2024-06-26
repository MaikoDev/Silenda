#pragma once

#include "types.h"

namespace Silenda
{
	struct NetworkMessage
	{
		NetworkMessage(const NetMessageType id = NetMessageType::count, const std::string msg = "") : identifier(id), payload(msg) {};

		NetMessageType identifier;
		std::string payload;

		MSGPACK_DEFINE(identifier, payload);
	};
}

