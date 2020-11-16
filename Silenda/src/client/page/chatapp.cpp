#include "chatapp.h"

#include <ctime>

namespace Silenda
{
	ChatApp::ChatApp(const render::FragPos& pos) : m_ChatPos(pos)
	{
		m_MessageLog.reserve(256);
	}

	void ChatApp::OnReceive(const ChatMessage message)
	{
		m_MessageLog.push_back(message);
	}

	void ChatApp::draw(render::MeshFrame* mesh)
	{
		/*(for (unsigned char i = 0; i < m_MessageLog.size() && i < 7; i++)
		{
			mesh->DrawUText()
		}*/
	}

	const std::wstring ChatApp::TimeStr(const __int64 time)
	{
		struct tm time_struct;
		localtime_s(&time_struct, &time);

		return L"TEST";
	}
}