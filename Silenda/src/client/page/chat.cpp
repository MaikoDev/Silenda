#include "pch.h"
#include "chat.h"

#include "../network/netclient.h"

namespace Silenda
{
	ChatPage::ChatPage() : Page(117, 29)
	{
		MTConsole::GetInstance()->attach(this);
		MsgHandler::GetInstance()->attach(this);

		m_ChatWindow = new ChatApp({ m_FormPos.x + 3, (m_FormPos.y + m_FormWidth) - 4, BACKGROUND_DEPTH });
	}

	ChatPage::~ChatPage()
	{
		MsgHandler::GetInstance()->detach(this);
		MTConsole::GetInstance()->detach(this);

		delete m_ChatWindow;
	}

	void ChatPage::update(IObservable* src, const unsigned char controller)
	{
		m_Mtx.lock();
		if (LoadedPage == this)
		{
			if (controller == 0) // console
			{
				m_InputText = MTConsole::GetInstance()->GetMsgBuffer();
			}
			else if (controller == 3) // msghandler <- messages from handler are only TXT on this controller channel
			{
				ClientChatMessage message(DisplayName, MsgHandler::GetInstance()->GetLastHandle().front().Message.Value.front());
				
				std::string payload = serialize(message);
				//m_ChatWindow->OnReceive(message);
				NetClient::GetInstance()->Send({ NetMessageType::chatmsg, payload });
			}
			else if (controller == 4) // msghandler <- messages from handler are only commands on this controller channel
			{
				std::queue<MsgHandle> commands = MsgHandler::GetInstance()->CopyLastHandle();

				while (!commands.empty())
				{
					if (commands.front().Message.Key == L"exit") // exit program execution
						BaseRunningState = false;
					else if (commands.front().Message.Key == L"cl_24hrtime")
					{
						if (commands.front().Message.Value[0] == L"true")
							m_milTime = true;
						else if (commands.front().Message.Value[0] == L"false")
							m_milTime = false;
					}

					commands.pop();
				}
			}
		}
		m_Mtx.unlock();
	}

	void ChatPage::initPostLoad()
	{
		MTConsole::GetInstance()->SetMaxBufferSize(88);
	}

	render::MeshFrame ChatPage::OnRender()
	{
		m_Mtx.lock();
		m_Mesh->reset();

		//////////////////////Form Drawing//////////////////////
		m_Mesh->DrawRect(m_FormLength, m_FormWidth, render::COLOR_DARK_GREEN, render::COLOR_BLACK, m_FormPos, TOP_SHADOWED | RIGHT_SHADOWED, true);

		m_Mesh->DrawUText(L"Silenda", render::COLOR_DARK_TEAL, render::COLOR_BLACK, { m_FormPos.x + 3, m_FormPos.y + 1, BACKGROUND_DEPTH });
		
		m_Mesh->DrawLine({ m_FormPos.x + 1, m_FormPos.y + 2, m_FormPos.z }, { m_FormPos.x + m_FormLength - 1, m_FormPos.y + 2 }, render::COLOR_DARK_GREEN, render::COLOR_BLACK);

		m_Mesh->DrawLine({ m_FormPos.x + 1, m_FormWidth - 2, m_FormPos.z }, { m_FormPos.x + m_FormLength - 1, m_FormWidth - 2 }, render::COLOR_DARK_GREEN, render::COLOR_BLACK);

		m_Mesh->DrawFrag({ L'\u2570', render::COLOR_DARK_GREEN, render::COLOR_BLACK }, { m_FormPos.x, m_FormPos.y + m_FormWidth - 1, m_FormPos.z - 1 });

		m_Mesh->DrawFrag({ L'>', render::COLOR_DARK_GREEN, render::COLOR_BLACK }, { m_FormPos.x + 3, m_FormWidth - 1, BACKGROUND_DEPTH });

		////////////////////////////////////////////////////////////////////////

		// Chat Region Drawing
		m_ChatWindow->draw(m_Mesh, m_milTime);

		m_Mesh->DrawUText(m_InputText, render::COLOR_DARK_GREEN, render::COLOR_BLACK, { m_FormPos.x + 4, m_FormWidth - 1, BACKGROUND_DEPTH });

		if (m_Timer.elapsed_now<int, std::ratio<1, 1>>() % 2 != 0)
			m_Mesh->DrawUText(L"|", render::COLOR_DARK_GREEN, render::COLOR_BLACK, { (m_FormPos.x + 4) + (short)m_InputText.size(), m_FormWidth - 1, BACKGROUND_DEPTH });

		m_Mtx.unlock();

		return *m_Mesh;
	}
}