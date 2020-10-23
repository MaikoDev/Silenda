#pragma once

#include <mutex>

#include "base.h"

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
	private:
		std::wstring inputText = L"";
		std::mutex mtx;
	};
}