#pragma once

#include <string>

#include "cl_userlevel.h"

#include <pods/pods.h>

struct ChatMessage
{
	std::string sender;
	UserLevel role;
	__int64 timesent;
	std::string message;

	PODS_SERIALIZABLE(
		1,
		PODS_MDR(sender),
		PODS_MDR(role),
		PODS_MDR(timesent),
		PODS_MDR(message)
	)
};