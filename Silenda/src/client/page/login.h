#pragma once

#include <mutex>

#include "base.h"
#include "..\utils\timing.h"

namespace Silenda
{
	class LoginPage : public Page
	{
	public:
		LoginPage();
		~LoginPage();

		// Active thread of execution of page
		void update(IObservable* src, const unsigned char controller = 0) override;

		render::MeshFrame OnRender() override;
		void initPostLoad() override;
	private:
		short m_FormLength = 33, m_FormWidth = 5;
		render::FragPos m_FormPos = { (m_Mesh->GetLength() / 2) - (m_FormLength / 2), (m_Mesh->GetWidth() / 2) - (m_FormWidth / 2), BACKGROUND_DEPTH - 1 };

		std::wstring m_InputText = L"";
		BasicTimer m_Timer;
		std::mutex m_Mtx;
	};
}