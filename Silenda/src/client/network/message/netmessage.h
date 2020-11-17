#pragma once

#include <string>

#include "types.h"
#include "msgpack.hpp"

// NetworkMessage
// @param1 NetworkMessageType
// @param2 serialized message
typedef msgpack::type::tuple<unsigned int, std::string> NetworkMessage;

