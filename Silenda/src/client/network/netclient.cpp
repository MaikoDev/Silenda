#include "netclient.h"

#include <sstream>

#include "../utils/iobserver.h"
#include "security/netpacker.h"
#include "message/netmessage.h"
//#include "message/types.h"

namespace Silenda
{
	NetClient::NetClient(const std::string serverIP, const unsigned int serverPort, const size_t networkBufferSize) : m_NetworkBufferSize(networkBufferSize)
	{
		config(serverIP, serverPort);
		m_NetworkBuffer = new char[m_NetworkBufferSize];


	}

	NetClient::~NetClient()
	{
		m_ThreadRunning = false;
		m_NetworkWorker.join();

		delete m_NetworkBuffer;
	}

	void NetClient::attach(IObserver* obs)
	{
		// Convert address to string
		std::stringstream ss;
		ss << obs;

		std::string addressName = ss.str();

		// Lookup to see if name is already registered
		if (_Olist.find(addressName) == _Olist.end())
			_Olist[addressName] = obs;
	}

	void NetClient::detach(IObserver* obs)
	{
		// Convert address to string
		std::stringstream ss;
		ss << obs;

		std::string addressName = ss.str();

		auto querry = _Olist.find(addressName);
		if (querry != _Olist.end())
			_Olist.erase(querry);
	}

	void NetClient::notify(const unsigned char controller)
	{
		// notify and update all observer on list
		for (auto iter = _Olist.begin(); iter != _Olist.end(); iter++)
			iter->second->update(this, controller);
	}

	int NetClient::Connect()
	{
		if (m_ServerSession.valid)
		{
			// Initialize WinSock
			WSAData data;
			WORD ver = MAKEWORD(2, 2);
			int wsResult = WSAStartup(ver, &data);
			if (wsResult != 0)
			{
				WSACleanup();
				return wsResult;
			}

			// Create client socket
			m_ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
			if (m_ClientSocket == INVALID_SOCKET)
			{
				wsResult = WSAGetLastError();
				WSACleanup();
				return wsResult;
			}

			// Fill hint struct
			sockaddr_in hint;
			hint.sin_family = AF_INET;
			hint.sin_port = htons(m_ServerSession.listenport);
			inet_pton(AF_INET, m_ServerSession.address.c_str(), &hint.sin_addr);

			// Connect to server
			int conResult = connect(m_ClientSocket, (sockaddr*)& hint, sizeof(hint));
			if (conResult == SOCKET_ERROR)
			{
				conResult = WSAGetLastError();
				closesocket(m_ClientSocket);
				WSACleanup();
				return conResult;
			}

			m_ThreadRunning = true;
			m_NetworkWorker = std::thread(&NetClient::OnReceive, this);
			return 0;
		}

		return 0;
	}

	int NetClient::Disconnect()
	{
		return 0;
	}

	void NetClient::OnReceive()
	{
		do
		{
			ZeroMemory(m_NetworkBuffer, m_NetworkBufferSize);
			int bytesReceived = recv(m_ClientSocket, m_NetworkBuffer, m_NetworkBufferSize, 0);

			if (bytesReceived > 0)
			{
				m_NetworkRaw = std::string(m_NetworkBuffer, 0, bytesReceived);
			}

		} while (m_ThreadRunning);
	}

	void NetClient::decode(const std::string& raw)
	{
		// Unpack message
		std::string unpacked = Silenda::NetPacker::GetInstance()->decrypt(raw);

		// Deserialization
		msgpack::object_handle objectHandle = msgpack::unpack(unpacked.data(), unpacked.size());
		msgpack::object deserialized = objectHandle.get();
		NetworkMessage msg;
		deserialized.convert(msg);

		// execute the specified message
		switch (msg.get<0>()) 
		{
		case NetworkMessageType::NETWORK_SERVER:
			ExecuteNetServer(msg.get<1>());
			break;
		case NetworkMessageType::NETWORK_AUTH:
			ExecuteNetAuth(msg.get<1>());
			break;
		case NetworkMessageType::NETWORK_CLIENT:
			ExecuteNetClient(msg.get<1>());
			break;
		}
	}

	void NetClient::ExecuteNetServer(const std::string& msg)
	{

	}

	void NetClient::ExecuteNetAuth(const std::string& msg)
	{

	}

	void NetClient::ExecuteNetClient(const std::string& msg)
	{

	}
}