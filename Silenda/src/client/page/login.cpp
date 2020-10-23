#include "login.h"

namespace Silenda
{
	LoginPage::LoginPage() : Page(35, 7) 
	{
		MTConsole::GetInstance()->attach(this);
		MsgHandler::GetInstance()->attach(this);
	}

	LoginPage::~LoginPage()
	{
		MsgHandler::GetInstance()->detach(this);
		MTConsole::GetInstance()->detach(this);
	}

	void LoginPage::update(IObservable* src, const unsigned char controller) // executed when notified by console or by msghandler
	{
		mtx.lock();
		if (LoadedPage == this)
		{
			if (controller == 0) // console
			{
				inputText = MTConsole::GetInstance()->GetMsgBuffer();
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
		}
		mtx.unlock();
	}

	render::MeshFrame LoginPage::OnRender() // executed on the renderer call of page object draw
	{
		mtx.lock();
		m_Mesh->reset();

		m_Mesh->DrawRect(33, 5, render::COLOR_DARK_GREEN, render::COLOR_BLACK, { 0, 0, 0 });

		m_Mesh->DrawUText(L"Silenda", render::COLOR_DARK_GREY, render::COLOR_BLACK, { 3, 1, 0 });
		m_Mesh->DrawUText(L"Display Name:", render::COLOR_DARK_GREEN, render::COLOR_BLACK, { 3, 3, 0 });
		m_Mesh->DrawUText(inputText, render::COLOR_DARK_GREEN, render::COLOR_BLACK, { 17, 3, 0 });
		mtx.unlock();

		return *m_Mesh;
	}
}