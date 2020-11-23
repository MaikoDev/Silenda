#pragma once

namespace SilendaServer
{
	struct NetUserAuth
	{
		NetUserAuth(const std::string& publicKey = "", const bool& auth = false, const std::string& clientUUID = "") : publickey(publicKey), authenticated(auth), uuid(clientUUID) {};

		std::string publickey;
		bool authenticated;

		std::string uuid;
	};
}