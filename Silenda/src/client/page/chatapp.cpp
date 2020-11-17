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

	void ChatApp::draw(render::MeshFrame* mesh, const bool& milTime)
	{
		// Possible overflow with ChatPos short to uchar j
		// i < 7 <-- magic number is relative to amount of messages can be fit in the Silenda 'window' (dependent on width)
		unsigned int logSize = m_MessageLog.size();
		for (unsigned char i = logSize, j = m_ChatPos.y; i > 0 && logSize - i < 7; i--, j -= 3)
		{
			ChatMessage msg = m_MessageLog[i - 1];

			std::wstring senderName = msg.get<0>();
			render::FragColor userColor;
			std::wstring msgtime = TimeStr(msg.get<2>(), milTime);

			switch (msg.get<1>()) // retrieve userlevel from message
			{
			case UserLevel::user:
				userColor = render::COLOR_DARK_GREEN;
				break;
			case UserLevel::admin:
				userColor = render::COLOR_DARK_BLUE;
				break;
			case UserLevel::superuser:
				userColor = render::COLOR_DARK_RED;
				break;
			}

			// draw sender name
			mesh->DrawUText(senderName, userColor, render::COLOR_BLACK, { m_ChatPos.x, j - 1, m_ChatPos.z });

			// draw time
			mesh->DrawUText(msgtime, render::COLOR_DARK_GREEN, render::COLOR_BLACK, { short(m_ChatPos.x + senderName.size() + 1), j - 1, m_ChatPos.z });

			// draw message
			mesh->DrawUText(msg.get<3>(), render::COLOR_WHITE, render::COLOR_BLACK, { m_ChatPos.x, j, m_ChatPos.z });
		}
	}

	const std::wstring ChatApp::TimeStr(const __int64 input_time, bool milTime)
	{
		struct tm time_struct;

		time_t now = time(0);
		struct tm now_struct;

		localtime_s(&time_struct, &input_time);
		localtime_s(&now_struct, &now);

		wchar_t buff[256];

		if (now_struct.tm_year == time_struct.tm_year) // determine if the year is the same
		{
			if (now_struct.tm_mon == time_struct.tm_mon) // determine if the month is the same
			{
				// Structure - (Today) | (Yesterday) at (H:MM) AM/PM | (HH:MM)
				if (now_struct.tm_mday == time_struct.tm_mday) // determine if date is today
				{
					if (!milTime)
						wcsftime(buff, 256, L"Today at %I:%M %p", &time_struct);
					else
						wcsftime(buff, 256, L"Today at %H:%M", &time_struct);

					return buff;
				}
				else if (now_struct.tm_mday - time_struct.tm_mday == 1) // determine if date was yesterday
				{
					if (!milTime)
						wcsftime(buff, 256, L"Yesterday at %I:%M %p", &time_struct);
					else
						wcsftime(buff, 256, L"Yesterday at %H:%M", &time_struct);

					return buff;
				}
			}
		}

		// Fall case if not today or yesterday - MM/DD/YYYY
		wcsftime(buff, 256, L"%m/%d/%Y", &time_struct);
		return buff;
	}
}