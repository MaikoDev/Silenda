#include "chat.h"

namespace Silenda
{
	ChatPage::ChatPage() : Page(117, 29)
	{
		MTConsole::GetInstance()->attach(this);
		MsgHandler::GetInstance()->attach(this);
	}

	ChatPage::~ChatPage()
	{
		MsgHandler::GetInstance()->detach(this);
		MTConsole::GetInstance()->detach(this);
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

			}
			else if (controller == 4) // msghandler <- messages from handler are only commands on this controller channel
			{
				std::queue<MsgHandle> commands = MsgHandler::GetInstance()->CopyLastHandle();

				while (!commands.empty())
				{
					if (commands.front().Message.Key == L"exit")
						BaseRunningState = false;
					// exit program execution

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
		m_Mesh->DrawRect(m_FormLength, m_FormWidth, render::COLOR_DARK_GREEN, render::COLOR_BLACK, m_FormPos, TOP_SHADOWED | RIGHT_SHADOWED);

		m_Mesh->DrawUText(L"Silenda", render::COLOR_DARK_TEAL, render::COLOR_BLACK, { m_FormPos.x + 3, m_FormPos.y + 1, 0 });
		
		m_Mesh->DrawLine({ m_FormPos.x + 1, m_FormPos.y + 2, 0 }, { m_FormPos.x + m_FormLength - 1, m_FormPos.y + 2 }, render::COLOR_DARK_GREEN, render::COLOR_BLACK);

		m_Mesh->DrawLine({ m_FormPos.x + 1, m_FormWidth - 2, 0 }, { m_FormPos.x + m_FormLength - 1, m_FormWidth - 2 }, render::COLOR_DARK_GREEN, render::COLOR_BLACK);

		m_Mesh->DrawFrag({ L'\u2570', render::COLOR_DARK_GREEN, render::COLOR_BLACK }, { m_FormPos.x, m_FormPos.y + m_FormWidth - 1, 0 });

		m_Mesh->DrawFrag({ L'>', render::COLOR_DARK_GREEN, render::COLOR_BLACK }, { m_FormPos.x + 3, m_FormWidth - 1, 0 });

		////////////////////////////////////////////////////////////////////////

		m_Mesh->DrawUText(m_InputText, render::COLOR_DARK_GREEN, render::COLOR_BLACK, { m_FormPos.x + 4, m_FormWidth - 1, 0 });

		if (m_Timer.elapsed_now<int, std::ratio<1, 1>>() % 2 != 0)
			m_Mesh->DrawUText(L"|", render::COLOR_DARK_GREEN, render::COLOR_BLACK, { (m_FormPos.x + 4) + (short)m_InputText.size(), m_FormWidth - 1, 0 });

		m_Mtx.unlock();

		return *m_Mesh;
	}
}