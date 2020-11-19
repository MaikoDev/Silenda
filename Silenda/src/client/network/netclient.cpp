#include "netclient.h"

#include <sstream>

#include "../utils/iobserver.h"
#include "chat/cl_chat.h"

namespace Silenda
{
	NetClient::NetClient(const std::string serverIP, const unsigned int serverPort, const size_t networkBufferSize) : m_NetworkBufferSize(networkBufferSize)
	{
		this->config(serverIP, serverPort);
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

			closesocket(m_ClientSocket);
			return 1;
		}

		return 0;
	}

	void NetClient::Send(const NetworkMessage msg)
	{
		if (m_ServerSession.valid && m_ThreadRunning)
		{
			// Serialization
			std::stringstream buffer;
			msgpack::pack(buffer, msg);

			buffer.seekg(0);
			std::string str(buffer.str());

			std::string networkstring;
			// Packing and encrypting if needed.
			if (m_ServerAuthPassed)
				networkstring = NetPacker::GetInstance()->encrypt(str, m_ServerPublicKey);
			else
				networkstring = NetPacker::GetInstance()->compress(str);

			// Send networkstring to server
			int sendResult = send(m_ClientSocket, networkstring.c_str(), networkstring.size() + 1, 0);
		}
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
				this->decode(m_NetworkRaw);
			}

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
		msgpack::object_handle objectHandle = msgpack::unpack(unpacked.data(), unpacked.size());
		msgpack::object deserialized = objectHandle.get();
		NetworkMessage msg;
		deserialized.convert(msg);

		// execute the specified message
		switch (msg.identifier) 
		{
		case NetMessageType::sv_deny:
			net_sv_deny();
			break;
		case NetMessageType::sv_pkvalidate:
			net_sv_pkvalidate(msg.payload);
			break;
		case NetMessageType::sv_pksend:
			net_sv_pksend(msg.payload);
			break;
		case NetMessageType::sv_pkrequest:
			net_sv_pkrequest();
			break;
		case NetMessageType::sv_auth_passed:
			net_sv_auth_passed();
			break;
		case NetMessageType::chatlog:
			net_chatlog(msg.payload);
			break;
		case NetMessageType::chatmsg:
			net_chatmsg(msg.payload);
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
		m_ServerAuthPassed = NetPacker::GetInstance()->GetPublicKey() == param;
		if (!m_ServerAuthPassed) // if server sent the wrong client public key back or server auth message failed.
		{
			this->Send({ NetMessageType::cl_auth_failed });
			this->Disconnect();
			return;
		}

		this->Send({ NetMessageType::cl_pkvalidate, NetPacker::GetInstance()->encrypt(m_ServerPublicKey, m_ServerPublicKey) });
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
	}

	// Server sends a vector of ChatMessages
	const inline void NetClient::net_chatlog(const std::string& param)
	{
		// Deserialize the vector of ChatMessage
		msgpack::object_handle objectHandle = msgpack::unpack(param.data(), param.size());
		msgpack::object deserialized = objectHandle.get();
		std::vector<ChatMessage> log;
		deserialized.convert(log);

		// Update changes to global
		*G_ChatLog::GetInstance() = log;
	}

	// Server sends a single ChatMessage
	const inline void NetClient::net_chatmsg(const std::string& param)
	{
		// Deserialize the ChatMessage
		msgpack::object_handle objectHandle = msgpack::unpack(param.data(), param.size());
		msgpack::object deserialized = objectHandle.get();
		ChatMessage message;
		deserialized.convert(message);

		// Update changes to global
		G_ChatLog::GetInstance()->push_back(message);
	}
}