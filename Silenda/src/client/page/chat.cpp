#include "chat.h"

namespace Silenda
{
	ChatPage::ChatPage() : Page(97, 28)
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
		mtx.lock();

		mtx.unlock();
	}

	render::MeshFrame ChatPage::OnRender()
	{
		mtx.lock();
		m_Mesh->reset();

		m_Mesh->DrawRect(95, 26, render::COLOR_DARK_GREEN, render::COLOR_BLACK, { 0, 0, 0 });
		mtx.unlock();

		return *m_Mesh;
	}
}