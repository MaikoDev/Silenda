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

		m_SelectTimeout.tv_usec = 1000;
		m_TickRateTime = 1000.0f / SERVER_TICKRATE;
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
		scheduleCallback("ServerSave", NULL, 30.0f, 0, &NetServer::save);
		while (m_IsListening)
		{
			m_ServerClock.start();
			fd_set FDMaster_cpy = m_FDMaster;

			int socketCount = select(0, &FDMaster_cpy, nullptr, nullptr, &m_SelectTimeout);

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

			}
			this->handleCallbacks();
			this->tickSync();

			this->updateTitle();
		}
	}

	const void NetServer::handleCallbacks()
	{
		auto iter = m_CallbackList.begin();
		while (iter != m_CallbackList.end())
		{
			const __int64 currentTime = std::chrono::high_resolution_clock::now().time_since_epoch().count();


			// Call the function pointer when call time is reached or past.
			ServerCallback call = iter->second;
			const __int64 callTime = (call.delay.initial + call.delay.duration).time_since_epoch().count();

			if (callTime <= currentTime)
			{
				switch (call.repetitions)
				{
				case -1: // Done repeating, remove callback from list and continue.
					iter = m_CallbackList.erase(iter);
					continue;
				case 0: // Infinite repeat do nothing.
					break;
				case 1: // If repeat has reached one set to -1 to end cycle.
					iter->second.repetitions = -1;
					break;
				default: // Continue decrementing.
					iter->second.repetitions--;
					break;
				}

				(this->*(call.function))(call.client, call.param);

				// Reset initial time for delay
				iter->second.delay.initial = std::chrono::high_resolution_clock::now();
			}

			iter++;
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
		case NetMessageType::healthresp:
			this->net_healthresp(client, msg.payload);
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
			{
				m_ClientAuthMap[client].uuid = param;
				net_healthstart(client);
				return;
			}
		}

		m_ClientAuthMap[client].uuid = ChatRoom::GetInstance()->genNewUUID();
		this->Send(client, { NetMessageType::sv_uuidsend, m_ClientAuthMap[client].uuid });
		net_healthstart(client);
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

	// Sending a heart-beat message to client to respond.
	const inline void NetServer::net_healthstart(const SOCKET& client, const std::string& param)
	{
		std::ostringstream ss;
		ss << client << "hBeat";

		scheduleCallback(ss.str(), client, 4.0f, 0, &NetServer::healthmissed);

		this->Send(client, { NetMessageType::healthstart });
	}

	// Receiving a heart-beat message response from client.
	const inline void NetServer::net_healthresp(const SOCKET& client, const std::string& param)
	{
		// Adjust time for processing delay.
		std::chrono::high_resolution_clock::time_point clientTime(
			std::chrono::high_resolution_clock::duration(deserialize<long long>(param))
		);

		std::chrono::nanoseconds processingTime(std::chrono::high_resolution_clock::now() - m_ServerClock.GetStartPoint());

		clientTime = clientTime + processingTime;

		std::string str = serialize(clientTime.time_since_epoch().count());

		// Echo back to client
		this->Send(client, { NetMessageType::healthresp, str });

		// Reset hBeat callback
		std::ostringstream ss;
		ss << client << "hBeat";

		auto iter = m_CallbackList.find(ss.str());
		if (iter != m_CallbackList.end())
			iter->second.delay.initial = std::chrono::high_resolution_clock::now();
	}

	// Called if client misses the 4 second heatbeat window.
	const inline void NetServer::healthmissed(const SOCKET& client, const std::string& param)
	{
		std::ostringstream ss;
		ss << client << "hTimeout";

		scheduleCallback(ss.str(), client, 15.0f, 1, &NetServer::healthtimedout);
	}

	// Called if client fails to respond in 15 second timeout.
	const inline void NetServer::healthtimedout(const SOCKET& client, const std::string& param)
	{
		// Disconnect client socket and clean-up.
		std::ostringstream ss;
		ss << "Socket " << client << " timed out.";

		this->socketCleanup(client);

		closesocket(client);
		FD_CLR(client, &m_FDMaster);
		printf("%s\n", ss.str().c_str());
	}

	const inline void NetServer::save(const SOCKET& client, const std::string& param)
	{
		ChatRoom::GetInstance()->save();
	}

	const void NetServer::scheduleCallback(const std::string& identifier, const SOCKET& client, const float& delay, const unsigned int& repetitions, const void (NetServer::* func)(const SOCKET&, const std::string&), const std::string& param)
	{
		int milSec = (delay * 1000.0f);

		ServerCallback callback(client, { std::chrono::high_resolution_clock::now(),  std::chrono::milliseconds(milSec) }, repetitions, func, param);

		m_CallbackList.insert({ identifier, callback });
	}

	const void NetServer::updateTitle()
	{
		ChatRoom* chatPtr = ChatRoom::GetInstance();

		std::ostringstream ss;
		ss << "SilendaServer " << chatPtr->GetConnectionCount() << "/" << chatPtr->GetMaxConnectCount() << " online users. " << "@ " << getTicksPerSec(0.8) << "/" << SERVER_TICKRATE << " Hz";

		SetConsoleTitle(LPCSTR(ss.str().c_str()));
	}

	const void NetServer::tickSync()
	{
		float tickTime = m_ServerClock.elapsed_now<float, std::milli>();

		float timeDif = ((m_TickRateTime - tickTime) * 1000) - 900;

		std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::microseconds((int)timeDif));
	}

	const void NetServer::socketCleanup(const SOCKET& client)
	{
		std::ostringstream ss;
		ss << client;

		std::string socketName = ss.str();

		ChatRoom::GetInstance()->leave(m_ClientAuthMap.at(client).uuid);
		m_ClientAuthMap.erase(client);

		m_CallbackList.erase(socketName + "hBeat");
		m_CallbackList.erase(socketName + "hBeatTimeout");
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

	const unsigned int NetServer::getTicksPerSec(const float& smoothing)
	{
		float oldWeight = pow(smoothing, m_ServerClock.elapsed_now<float, std::ratio<1, 1>>());
		m_TicksPerSec = oldWeight * m_TicksPerSec + (1.0f - oldWeight) * ((1000.0f / m_ServerClock.elapsed_now<float, std::milli>()));

		return round(m_TicksPerSec);
	}
}