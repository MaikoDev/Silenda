#include "pch.h"
#include "netclient.h"

#include "../utils/iobserver.h"
#include "utils/byte_cpy.h"
#include "chat/cl_chat.h"
#include "fileio/fio.h"
#include "page/base.h"

namespace Silenda
{
	bool G_ClientAuth = false;
	NetClient* NetClient::m_Instance = nullptr;

	NetClient::NetClient()
	{
		m_NetworkBuffer = new char[NETWORK_BUFFER_SIZE];
		FIO::GetInstance()->readFromFile("uuid.txt", m_ClientUUID);
	}

	NetClient* NetClient::GetInstance()
	{
		if (m_Instance == nullptr)
			m_Instance = new NetClient();
		return m_Instance;
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

	int NetClient::Connect(const std::string& serverIP, const unsigned int& serverPort)
	{
		config(serverIP, serverPort);

		if (m_ServerSession.valid && !m_ThreadRunning)
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
		if (m_ThreadRunning)
		{
			m_ThreadRunning = false;
			m_NetworkWorker.join();

			G_ClientAuth = false;
			closesocket(m_ClientSocket);

			// Notify any pages there has been a disconnect
			notify(5);
			return 1;
		}

		return 0;
	}

	void NetClient::Send(const NetworkMessage msg)
	{
		if (m_ServerSession.valid && m_ThreadRunning)
		{
			// Serialization
			std::string str = serialize(msg);

			std::string networkstring;
			// Packing and encrypting if needed.
			if (m_ServerAuthPassed)
				networkstring = NetPacker::GetInstance()->encrypt(str, m_ServerPublicKey);
			else
				networkstring = NetPacker::GetInstance()->compress(str);

			// Send networkstring to server
			int sendResult = send(m_ClientSocket, networkstring.c_str(), networkstring.size(), 0);
		}
	}

	void NetClient::OnReceive()
	{
		do
		{
			ZeroMemory(m_NetworkBuffer, NETWORK_BUFFER_SIZE);
			int bytesReceived = recv(m_ClientSocket, m_NetworkBuffer, NETWORK_BUFFER_SIZE, 0);

			if (bytesReceived > 0)
			{
				byte_cpy(m_NetworkBuffer, bytesReceived, m_NetworkRaw);
				this->decode(m_NetworkRaw);
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(1));

		} while (m_ThreadRunning);
	}

	void NetClient::decode(const std::string& raw)
	{
		// Unpack message if authentication is passed.
		std::string unpacked;
		if (m_ServerAuthPassed)
			unpacked = NetPacker::GetInstance()->decrypt(raw);
		else
			unpacked = NetPacker::GetInstance()->decompress(raw);

		// Deserialization
		NetworkMessage msg;
		msg = deserialize<NetworkMessage>(unpacked);

		// execute the specified message
		switch (msg.identifier) 
		{
		case NetMessageType::sv_deny:
			this->net_sv_deny();
			break;
		case NetMessageType::sv_pkvalidate:
			this->net_sv_pkvalidate(msg.payload);
			break;
		case NetMessageType::sv_pksend:
			this->net_sv_pksend(msg.payload);
			break;
		case NetMessageType::sv_pkrequest:
			this->net_sv_pkrequest();
			break;
		case NetMessageType::sv_auth_passed:
			this->net_sv_auth_passed();
			break;
		case NetMessageType::sv_auth_failed:
			this->net_sv_auth_failed();
			break;
		case NetMessageType::sv_uuidrequest:
			this->net_sv_uuidrequest(msg.payload);
			break;
		case NetMessageType::sv_uuidsend:
			this->net_sv_uuidsend(msg.payload);
			break;
		case NetMessageType::chatlog:
			this->net_chatlog(msg.payload);
			break;
		case NetMessageType::chatmsg:
			this->net_chatmsg(msg.payload);
			break;
		}
	}

	// Authentication from server was denied
	const inline void NetClient::net_sv_deny(const std::string& param)
	{
		m_ServerAuthPassed = false;
		this->Disconnect();
	}

	// Authentication from server was validated
	const inline void NetClient::net_sv_pkvalidate(const std::string& param)
	{
		m_ServerAuthPassed = true;

		this->Send({ NetMessageType::cl_pkvalidate, param });
	}

	// Receiving server's public key
	const inline void NetClient::net_sv_pksend(const std::string& param)
	{
		m_ServerPublicKey = param;
		m_ServerAuthPassed = true;
	}

	// Server is requesting client to send public key
	const inline void NetClient::net_sv_pkrequest(const std::string& param)
	{
		// Send the client public key to server.
		this->Send({ NetMessageType::cl_pksend, NetPacker::GetInstance()->GetPublicKey() });
	}

	// Server's final authentication message confirming the handshake completion
	const inline void NetClient::net_sv_auth_passed(const std::string& param)
	{
		// notify all Pages that NetClient is ready to start processing data to the server.
		G_ClientAuth = true;
	}

	// Server's final authentication message showing that handshake failed
	const inline void NetClient::net_sv_auth_failed(const std::string& param)
	{
		G_ClientAuth = false;
		BaseRunningState = false;
	}

	// Server is requesting client to send a uuid to confirm identity
	const inline void NetClient::net_sv_uuidrequest(const std::string& param)
	{
		this->Send({ NetMessageType::cl_uuidsend, m_ClientUUID });
	}

	// Server is sending a newly generated uuid
	const inline void NetClient::net_sv_uuidsend(const std::string& param)
	{
		FIO::GetInstance()->writeToFile("uuid.txt", param);
		m_ClientUUID = param;
	}

	// Server sends a vector of ChatMessages
	const inline void NetClient::net_chatlog(const std::string& param)
	{
		// Deserialize the vector of ChatMessage
		std::vector<ServerChatMessage> log;
		log = deserialize<std::vector<ServerChatMessage>>(param);

		// Update changes to global
		*G_ChatLog::GetInstance() = log;
	}

	// Server sends a single ChatMessage
	const inline void NetClient::net_chatmsg(const std::string& param)
	{
		// Deserialize the ChatMessage
		ServerChatMessage message;
		message = deserialize<ServerChatMessage>(param);

		// Update changes to global
		G_ChatLog::GetInstance()->push_back(message);
	}

}