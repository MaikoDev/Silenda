#include "login.h"

namespace Silenda
{
	LoginPage::LoginPage() : Page(117, 29) 
	{
		MTConsole::GetInstance()->attach(this);
		MsgHandler::GetInstance()->attach(this);

		// Start internal timer
		m_Timer.start();
	}

	LoginPage::~LoginPage()
	{
		MsgHandler::GetInstance()->detach(this);
		MTConsole::GetInstance()->detach(this);
	}

	void LoginPage::update(IObservable* src, const unsigned char controller) // executed when notified by console or by msghandler
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
				DisplayName = MsgHandler::GetInstance()->GetLastHandle().front().Message.Value.front();

				Page* chat = GetConnected("SilendaChat");

				if (chat != nullptr)
				{
					this->unload();
					chat->load();
				}
			}
			else if (controller == 4) // msghandler <- message from handler are only commands on this controller channel
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

	void LoginPage::initPostLoad()
	{
		MTConsole::GetInstance()->SetMaxBufferSize(14);
	}

	render::MeshFrame LoginPage::OnRender() // executed on the renderer call of page object draw
	{
		m_Mtx.lock();

		m_Mesh->reset();

		m_Mesh->DrawRect(m_FormLength, m_FormWidth, render::COLOR_DARK_GREEN, render::COLOR_BLACK, m_FormPos, TOP_SHADOWED | RIGHT_SHADOWED, true);

		m_Mesh->DrawLine({ m_FormPos.x + 1, m_FormPos.y + 2, 0 }, { (m_FormPos.x + 1) + (m_FormLength - 2), m_FormPos.y + 2, m_FormPos.z }, render::COLOR_DARK_GREEN, render::COLOR_BLACK);
		m_Mesh->DrawFrag({ L'\u2570', render::COLOR_DARK_GREEN, render::COLOR_BLACK }, { m_FormPos.x, m_FormPos.y + m_FormWidth - 1, m_FormPos.z });

		m_Mesh->DrawUText(L"Silenda", render::COLOR_DARK_TEAL, render::COLOR_BLACK, { m_FormPos.x + 3, m_FormPos.y + 1, BACKGROUND_DEPTH });
		m_Mesh->DrawUText(L"Display Name:", render::COLOR_DARK_GREEN, render::COLOR_BLACK, { m_FormPos.x + 3, m_FormPos.y + 3, BACKGROUND_DEPTH });

		m_Mesh->DrawUText(m_InputText, render::COLOR_DARK_GREEN, render::COLOR_BLACK, { m_FormPos.x + 17, m_FormPos.y + 3, BACKGROUND_DEPTH });

		if (m_Timer.elapsed_now<int, std::ratio<1, 1>>() % 2 != 0)
			m_Mesh->DrawUText(L"|", render::COLOR_DARK_GREEN, render::COLOR_BLACK, { (m_FormPos.x + 17) + (short)m_InputText.size(), m_FormPos.y + 3, BACKGROUND_DEPTH });

		m_Mtx.unlock();

		return *m_Mesh;
	}
}