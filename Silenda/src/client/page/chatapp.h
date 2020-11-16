#pragma once

#include "../render/mesh/mesh.h"
#include "../network/chat/cl_message.h"

namespace Silenda
{
	class ChatApp
	{
	public:
		// ChatApp
		// @param1 position of bottom left quadrant of chat region.
		// @INFO ChatApp is drawn from bottom left up instead of top left down.
		ChatApp(const render::FragPos& pos);
		~ChatApp() {};

		void OnReceive(const ChatMessage message);

		void draw(render::MeshFrame* mesh);
		const std::wstring TimeStr(const __int64 time);
	private:
	private:
		std::vector<ChatMessage> m_MessageLog;
		render::FragPos m_ChatPos;
	};
}