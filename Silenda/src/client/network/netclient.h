#pragma once

#include "../utils/iobservable.h"
#include "security/netpacker.h"
#include "message/netmessage.h"

#define NETWORK_BUFFER_SIZE 4096

namespace Silenda
{
	struct ClientCallback;

	struct NetSession
	{
		bool valid = false;
		std::string address;
		unsigned int listenport;
	};

	extern bool G_ClientAuth;

	class NetClient : public IObservable
	{
	public:
		static NetClient* GetInstance();
		~NetClient();

		void attach(IObserver* obs) override;
		void detach(IObserver* obs) override;
		void notify(const unsigned char controller = 0) override;

		// Connect
		// @param1 Server IP Address
		// @param2 Server Listening Port
		int Connect(const std::string& serverIP, const unsigned int& serverPort);
		int Disconnect();

		void Send(const NetworkMessage msg);

		const inline void config(const std::string& serverIP, const unsigned int& serverPort) { m_ServerSession.address = serverIP; m_ServerSession.listenport = serverPort; m_ServerSession.valid = true; };
		const void handleCallbacks();

		const inline unsigned short& GetLatency() const& { return m_NetworkLatency; };
	private:
		void OnReceive();

		void decode(const std::string& raw);

		const inline void net_sv_deny(const std::string& param = "");
		const inline void net_sv_pkvalidate(const std::string& param = "");
		const inline void net_sv_pksend(const std::string& param = "");
		const inline void net_sv_pkrequest(const std::string& param = "");
		const inline void net_sv_auth_passed(const std::string& param = "");
		const inline void net_sv_auth_failed(const std::string& param = "");
		const inline void net_sv_uuidrequest(const std::string& param = "");
		const inline void net_sv_uuidsend(const std::string& param = "");

		const inline void net_chatjoin(const std::string& param = "");
		const inline void net_chatlog(const std::string& param = "");
		const inline void net_chatmsg(const std::string& param = "");

		const inline void net_healthstart(const std::string& param = "");
		const inline void net_healthresp(const std::string& param = "");

		const inline void heartbeat(const std::string& param = "");
	private:
		const void scheduleCallback(const std::string& identifier, const float& delay, const unsigned int& repetitions, const void (NetClient::* func)(const std::string&), const std::string& param = "");
		const inline void removeCallback(const std::string& identifier) { m_CallbackList.erase(identifier); };
	private:
		NetSession m_ServerSession;
		std::string m_ServerPublicKey;
		bool m_ServerAuthPassed = false;

		char* m_NetworkBuffer;
		SOCKET m_ClientSocket;
		std::string m_ClientUUID = "";
		std::string m_NetworkRaw;
		unsigned short m_NetworkLatency = 0;

		std::thread m_NetworkWorker;
		bool m_ThreadRunning = false;
	private:
		std::unordered_map<std::string, ClientCallback> m_CallbackList;
	private:
		NetClient();
		static NetClient* m_Instance;
	};

	struct CallbackTime
	{
		std::chrono::high_resolution_clock::time_point initial;
		std::chrono::milliseconds duration;
	};

	struct ClientCallback
	{
		ClientCallback(const CallbackTime delayTime, const unsigned int& repeatCount, const void (NetClient::* func)(const std::string&), const std::string& funcParam = "") : delay(delayTime), repetitions(repeatCount), function(func), param(funcParam) {};

		CallbackTime delay;
		__int64 repetitions;
		const void (NetClient::* function)(const std::string&);
		std::string param;
	};
}