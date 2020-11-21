#pragma once

#include "base.h"
#include "chatapp.h"

#include "../utils/timing.h"

namespace Silenda
{
	class ChatPage : public Page
	{
	public:
		ChatPage();
		~ChatPage();

		// Active thread of execution of page
		void update(IObservable* src, const unsigned char controller = 0) override;

		render::MeshFrame OnRender() override;
		void initPostLoad() override;
	private:
		ChatApp* m_ChatWindow;

		bool m_milTime = false;
	private:
		short m_FormLength = 95, m_FormWidth = 26;
		render::FragPos m_FormPos = { (m_Mesh->GetLength() / 2) - (m_FormLength / 2), (m_Mesh->GetWidth() / 2) - (m_FormWidth / 2), BACKGROUND_DEPTH - 1 };

		std::wstring m_InputText = L"";
		BasicTimer m_Timer;
		std::mutex m_Mtx;
	};
}