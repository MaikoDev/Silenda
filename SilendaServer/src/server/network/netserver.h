#pragma once

#define NETWORK_BUFFER_SIZE 4096

#include "security/netuserauth.h"
#include "message/netmessage.h"

namespace SilendaServer
{
	class NetServer
	{
	public:
		static NetServer* GetInstance();
		~NetServer();

		const void start(unsigned short listeningPort);
		const void Send(const SOCKET& client, const NetworkMessage message);
	private:
		const void handleRequests();

		void decode(const SOCKET& client, const std::string& raw);

		const inline void net_cl_pkvalidate(const SOCKET& client, const std::string& param = "");
		const inline void net_cl_pksend(const SOCKET& client, const std::string& param = "");
		const inline void net_cl_uuidsend(const SOCKET& client, const std::string& param = "");
		const inline void net_chatmsg(const SOCKET& client, const std::string& param = "");

		const std::string genAuthCode(const SOCKET& client, const unsigned int& authCodeLength);
	private:
		std::unordered_map<SOCKET, NetUserAuth> m_ClientAuthMap;
		std::unordered_map<std::string, SOCKET> m_AuthPhraseMap;

		SOCKET m_ListeningSocket;
		sockaddr_in m_HintStruct;
		fd_set m_FDMaster;

		char* m_NetworkBuffer;
		std::string m_NetworkRaw;

		bool m_IsListening = false;
	private:
		NetServer();
		static NetServer* m_Instance;
	};
}