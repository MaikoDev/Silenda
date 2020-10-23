#pragma once

#include <mutex>

#include "base.h"

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
	private:
		std::wstring inputText = L"";
		std::mutex mtx;
	};
}