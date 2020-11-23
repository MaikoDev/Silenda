#include "pch.h"
#include "netserver.h"

#include "security/netpacker.h"
#include "utils/byte_cpy.h"
#include "chat/chatroom.h"
#include "utils/uuid.h"

namespace SilendaServer
{
	NetServer* NetServer::m_Instance = nullptr;

	NetServer::NetServer()
	{
		m_NetworkBuffer = new char[NETWORK_BUFFER_SIZE];
	}

	NetServer* NetServer::GetInstance()
	{
		if (m_Instance == nullptr)
			m_Instance = new NetServer();
		return m_Instance;
	}

	NetServer::~NetServer()
	{
		
	}

	const void NetServer::start(unsigned short listeningPort)
	{
		if (!m_IsListening)
		{
			// Init WinSock
			WSADATA wsData;
			WORD ver = MAKEWORD(2, 2);

			int wsOK = WSAStartup(ver, &wsData);
			if (wsOK != 0)
				return;

			// Create listening socket
			m_ListeningSocket = socket(AF_INET, SOCK_STREAM, 0);
			if (m_ListeningSocket == INVALID_SOCKET)
				return;

			// Fill HintStruct
			m_HintStruct.sin_family = AF_INET;
			m_HintStruct.sin_port = htons(listeningPort);
			m_HintStruct.sin_addr.S_un.S_addr = INADDR_ANY;

			bind(m_ListeningSocket, (sockaddr*)& m_HintStruct, sizeof(m_HintStruct));

			// Bind listening socket to WinSock
			listen(m_ListeningSocket, SOMAXCONN);

			// Add listening socket to FD
			FD_ZERO(&m_FDMaster);
			FD_SET(m_ListeningSocket, &m_FDMaster);

			m_IsListening = true;
			handleRequests();

			return;
		}
	}

	const void NetServer::Send(const SOCKET& client, const NetworkMessage message)
	{
		if (m_IsListening)
		{
			// Serialization
			std::string str = serialize(message);

			std::string networkstring;
			NetUserAuth clientAuth = m_ClientAuthMap[client];

			// Packing and encrypting if needed.
			if (clientAuth.authenticated)
				networkstring = NetPacker::GetInstance()->encrypt(str, clientAuth.publickey);
			else
				networkstring = NetPacker::GetInstance()->compress(str);

			// Send networkstring to client
			int sendResult = send(client, networkstring.c_str(), networkstring.size(), 0);
		}
	}

	const void NetServer::handleRequests()
	{
		while (m_IsListening)
		{
			fd_set FDMaster_cpy = m_FDMaster;

			int socketCount = select(0, &FDMaster_cpy, nullptr, nullptr, nullptr);

			for (int i = 0; i < socketCount; i++)
			{
				SOCKET sock = FDMaster_cpy.fd_array[i];

				if (sock == m_ListeningSocket) // incoming socket is a new connection
				{
					// Accept connection
					SOCKET client = accept(m_ListeningSocket, nullptr, nullptr);

					// Add client to FDMaster
					FD_SET(client, &m_FDMaster);

					// Begin security authentication
					m_ClientAuthMap[client] = {};
					this->Send(client, { NetMessageType::sv_pkrequest });
				}
				else // incoming socket is a previously established client 
				{
					ZeroMemory(m_NetworkBuffer, NETWORK_BUFFER_SIZE);

					// Accept new message from client
					int bytesReceived = recv(sock, m_NetworkBuffer, NETWORK_BUFFER_SIZE, 0);
					if (bytesReceived <= 0)
					{
						// User disconnected

						// Drop the client from FDMaster
						closesocket(sock);
						FD_CLR(sock, &m_FDMaster);
					}
					else
					{
						// NetworkMessage
						byte_cpy(m_NetworkBuffer, bytesReceived, m_NetworkRaw);
						this->decode(sock, m_NetworkRaw);
					}
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}

	void NetServer::decode(const SOCKET& client, const std::string& raw)
	{
		// Unpack message if authentication is passed for client.
		std::string unpacked;
		if (m_ClientAuthMap[client].authenticated)
			unpacked = NetPacker::GetInstance()->decrypt(raw);
		else
			unpacked = NetPacker::GetInstance()->decompress(raw);

		// Deserialization
		NetworkMessage msg;
		msg = deserialize<NetworkMessage>(unpacked);

		// execute specified message
		switch (msg.identifier)
		{
		case NetMessageType::cl_pkvalidate:
			this->net_cl_pkvalidate(client, msg.payload);
			break;
		case NetMessageType::cl_pksend:
			this->net_cl_pksend(client, msg.payload);
			break;
		case NetMessageType::cl_uuidsend:
			this->net_cl_uuidsend(client, msg.payload);
			break;
		case NetMessageType::chatjoin:
			this->net_chatjoin(client, msg.payload);
			break;
		case NetMessageType::chatleave:
			this->net_chatleave(client);
			break;
		case NetMessageType::chatmsg:
			this->net_chatmsgRecv(client, msg.payload);
			break;
		}
	}

	const inline void NetServer::net_broadcast(const void (NetServer::*netFunc)(const SOCKET&, const std::string&), const std::string& param)
	{
		// Broadcast to other clients
		for (int i = 0; i < m_FDMaster.fd_count; i++)
		{
			SOCKET outSock = m_FDMaster.fd_array[i];
			if (outSock != m_ListeningSocket)
				(this->*netFunc)(outSock, param);
		}
	}

	// Clients send's server's public key back for validation
	const inline void NetServer::net_cl_pkvalidate(const SOCKET& client, const std::string& param)
	{
		auto authIter = m_AuthPhraseMap.find(param);
		if (authIter != m_AuthPhraseMap.end())
		{
			if (authIter->second == client)
			{
				this->Send(client, { NetMessageType::sv_auth_passed });
				this->Send(client, { NetMessageType::sv_uuidrequest });

				m_AuthPhraseMap.erase(authIter);
				return;
			}
		}

		this->Send(client, { NetMessageType::sv_auth_failed });
	}

	// Client send's it's own public key 
	const inline void NetServer::net_cl_pksend(const SOCKET& client, const std::string& param)
	{
		m_ClientAuthMap[client].publickey = param;

		this->Send(client, { NetMessageType::sv_pksend, NetPacker::GetInstance()->GetPublicKey() });

		// could also deny connect here with sv_deny if wanted.

		m_ClientAuthMap[client].authenticated = true;
		this->Send(client, { NetMessageType::sv_pkvalidate, genAuthCode(client, 26) });
	}

	// Client sends back a uuid identifying itself.
	const inline void NetServer::net_cl_uuidsend(const SOCKET& client, const std::string& param)
	{
		if (param != "")
		{
			if (ChatRoom::GetInstance()->findUUID(param))
				m_ClientAuthMap[client].uuid = param;
		}

		m_ClientAuthMap[client].uuid = ChatRoom::GetInstance()->genNewUUID();
		this->Send(client, { NetMessageType::sv_uuidsend, m_ClientAuthMap[client].uuid });
	}

	// Client request to join chat room
	const inline void NetServer::net_chatjoin(const SOCKET& client, const std::string& param)
	{
		ChatRoom* chatPtr = ChatRoom::GetInstance();
		std::wstring displayName = deserialize<std::wstring>(param);

		// Attempt to join the chat room and send back the join result to client.
		bool joinResult = chatPtr->join(m_ClientAuthMap[client].uuid, displayName);
		this->Send(client, { NetMessageType::chatjoin, std::to_string(joinResult) });
		
		if (joinResult)
		{
			// Retrieve chatlog and serialize it.
			std::string str = serialize(chatPtr->GetMessageLog());

			this->Send(client, { NetMessageType::chatlog, str });
		}
	}

	// Client request to leave the chat room
	const inline void NetServer::net_chatleave(const SOCKET& client, const std::string& param)
	{
		ChatRoom::GetInstance()->leave(m_ClientAuthMap[client].uuid);
	}

	// Client push's a message to the chat
	const inline void NetServer::net_chatmsgRecv(const SOCKET& client, const std::string& param)
	{
		ChatRoom* chatPtr = ChatRoom::GetInstance();
		if (chatPtr->isActive(m_ClientAuthMap[client].uuid))
		{
			// Deserialize ClientChatMessage;
			ClientChatMessage msg = deserialize<ClientChatMessage>(param);

			chatPtr->push_back(m_ClientAuthMap[client].uuid, msg);

			this->net_broadcast(&NetServer::net_chatmsgSend);
		}
	}

	const inline void NetServer::net_chatmsgSend(const SOCKET& client, const std::string& param)
	{
		// Serialize ServerChatMessage;
		std::string str = serialize(ChatRoom::GetInstance()->back());

		this->Send(client, { NetMessageType::chatmsg, str });
	}

	const std::string NetServer::genAuthCode(const SOCKET& client, const unsigned int& authCodeLength)
	{
		std::random_device rand_dev;
		std::mt19937 generator;

		generator.seed(rand_dev());

		std::uniform_int_distribution<int> distr(32, 126);
		std::string authCode = std::string(authCodeLength, '0');

		do
		{
			for (unsigned int i = 0; i < authCodeLength; i++)
				authCode[i] = (char)distr(generator);

		} while (m_AuthPhraseMap.find(authCode) != m_AuthPhraseMap.end());

		m_AuthPhraseMap[authCode] = client;
		return authCode;
	}
}