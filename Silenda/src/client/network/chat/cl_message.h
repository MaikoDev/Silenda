#pragma once

#include <string>

#include "cl_userlevel.h"
#include "msgpack.hpp"

typedef msgpack::type::tuple<std::wstring, unsigned int, __int64, std::wstring> ChatMessage;

/*struct ChatMessage
{
	std::string sender;
	UserLevel role;
	__int64 timesent;
	std::string message;
};*/