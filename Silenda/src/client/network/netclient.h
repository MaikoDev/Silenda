#pragma once

#include <string>
#include <thread>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#include "../utils/iobservable.h"
#include "security/netpacker.h"

namespace Silenda
{
	struct NetSession
	{
		bool valid = false;
		std::string address;
		unsigned int listenport;
	};

	class NetClient : public IObservable
	{
	public:
		// NetClient
		// @param1 Server IP Address
		// @param2 Server Listening Port
		// @param3 Size of network buffer (def: 4096)
		NetClient(const std::string serverIP, const unsigned int serverPort, const size_t networkBufferSize = 4096);
		~NetClient();

		void attach(IObserver* obs) override;
		void detach(IObserver* obs) override;
		void notify(const unsigned char controller = 0) override;

		int Connect();
		int Disconnect();

		const inline void config(const std::string& serverIP, const unsigned int& serverPort) { m_ServerSession.address = serverIP; m_ServerSession.listenport = serverPort; m_ServerSession.valid = true; };
	private:
		void OnReceive();

		void decode(const std::string& raw);

		void ExecuteNetServer(const std::string& msg);
		void ExecuteNetAuth(const std::string& msg);
		void ExecuteNetClient(const std::string& msg);
	private:
		NetSession m_ServerSession;

		char* m_NetworkBuffer;
		size_t m_NetworkBufferSize;
		SOCKET m_ClientSocket;
		std::string m_NetworkRaw;

		std::thread m_NetworkWorker;
		bool m_ThreadRunning = false;
	};
}