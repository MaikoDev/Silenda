#pragma once

#define NETWORK_BUFFER_SIZE 4096
#define SERVER_TICKRATE 120

#include "security/netuserauth.h"
#include "message/netmessage.h"
#include "utils/timing.h"

namespace SilendaServer
{
	// Forward declaration
	struct ServerCallback;

	class NetServer
	{
	public:
		static NetServer* GetInstance();
		~NetServer();

		const void start(unsigned short listeningPort);
		const void Send(const SOCKET& client, const NetworkMessage message);
	private:
		const void handleRequests();
		const void handleCallbacks();

		void decode(const SOCKET& client, const std::string& raw);

		const inline void net_broadcast(const void (NetServer::*)(const SOCKET&, const std::string&), const std::string& param = "");

		const inline void net_cl_pkvalidate(const SOCKET& client, const std::string& param = "");
		const inline void net_cl_pksend(const SOCKET& client, const std::string& param = "");
		const inline void net_cl_uuidsend(const SOCKET& client, const std::string& param = "");
		const inline void net_chatjoin(const SOCKET& client, const std::string& param = "");
		const inline void net_chatleave(const SOCKET& client, const std::string& param = "");
		const inline void net_chatmsgRecv(const SOCKET& client, const std::string& param = "");
		const inline void net_chatmsgSend(const SOCKET& client, const std::string& param = "");

		const inline void net_healthstart(const SOCKET& client, const std::string& param = "");
		const inline void net_healthresp(const SOCKET& client, const std::string& param = "");
		const inline void healthmissed(const SOCKET& client, const std::string& param = "");
		const inline void healthtimedout(const SOCKET& client, const std::string& param = "");

		const inline void save(const SOCKET& client, const std::string& param = "");

		// scheduleCallback
		// @param1 Unique identifier for specific callback.
		// @param2 Client to apply callback to.
		// @param3 The delay interval in seconds. If delay is too short callback will be called on next tick.
		// @param4 Number of times to repeat callback. 0 for infinite repeats.
		// @param5 Function pointer to be called.
		// @param6 Opt. parameters to be passed to function.
		const void scheduleCallback(const std::string& identifier, const SOCKET& client, const float& delay, const unsigned int& repetitions, const void (NetServer::* func)(const SOCKET&, const std::string&), const std::string& param = "");
		const inline void removeCallback(const std::string& identifier) { m_CallbackList.erase(identifier); };

		const void updateTitle();
		const void tickSync();

		const void socketCleanup(const SOCKET& client);

		const std::string genAuthCode(const SOCKET& client, const unsigned int& authCodeLength);
		const unsigned int getTicksPerSec(const float& smoothing = 0.5f);
	private:
		std::unordered_map<SOCKET, NetUserAuth> m_ClientAuthMap;
		std::unordered_map<std::string, SOCKET> m_AuthPhraseMap;

		SOCKET m_ListeningSocket;
		sockaddr_in m_HintStruct;
		fd_set m_FDMaster;
		TIMEVAL m_SelectTimeout = { 0 };

		char* m_NetworkBuffer;
		std::string m_NetworkRaw;

		bool m_IsListening = false;
	private:
		std::unordered_map<std::string, ServerCallback> m_CallbackList;
	private:
		NetServer();
		static NetServer* m_Instance;

		BasicTimer m_ServerClock;
		float m_TickRateTime;
		float m_TicksPerSec = SERVER_TICKRATE;
	};

	struct CallbackTime
	{
		std::chrono::high_resolution_clock::time_point initial;
		std::chrono::milliseconds duration;
	};

	struct ServerCallback
	{
		ServerCallback(const SOCKET& clientSocket, const CallbackTime delayTime, const unsigned int& repeatCount, const void (NetServer::* func)(const SOCKET&, const std::string&), const std::string& funcParam = "") : client(clientSocket), delay(delayTime), repetitions(repeatCount), function(func), param(funcParam) {};

		SOCKET client;
		CallbackTime delay;
		__int64 repetitions;
		const void (NetServer::*function)(const SOCKET&, const std::string&);
		std::string param;
	};

}