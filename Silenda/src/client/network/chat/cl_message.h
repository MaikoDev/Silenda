#pragma once

#include <string>

#include <pods/pods.h>

struct ChatMessage
{
	std::string sender;
	__int64 timesent;
	std::string message;

	PODS_SERIALIZABLE(
		1,
		PODS_MDR(sender),
		PODS_MDR(timesent),
		PODS_MDR(message)
	)
};