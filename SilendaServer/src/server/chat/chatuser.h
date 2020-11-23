#pragma once

#include "network/chat/userlevel.h"

namespace SilendaServer
{
	struct ChatUser
	{
		std::string socketName;
		UserLevel role;
	};
}