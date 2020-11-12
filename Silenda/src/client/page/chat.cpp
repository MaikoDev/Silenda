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
		m_Mtx.lock();

		m_Mtx.unlock();
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



		m_Mtx.unlock();

		return *m_Mesh;
	}
}