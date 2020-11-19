#pragma once

#include <string>

#include "types.h"
#include "msgpack.hpp"

// NetworkMessage
// @param1 NetMessageType
// @param2 serialized payload
//typedef msgpack::type::tuple<unsigned int, std::string> NetworkMessage;

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

