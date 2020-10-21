#pragma once

#include "base.h"

namespace Silenda
{
	class LoginPage : public Page
	{
	public:
		LoginPage() : Page(35, 7) {};
		~LoginPage() {};

		void active() override;
		render::MeshFrame OnRender() override;
	};
}