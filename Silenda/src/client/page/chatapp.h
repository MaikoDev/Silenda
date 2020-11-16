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
	private:
		// TimeStr
		// @param1 time to be compared with current time to determine time format
		// @param2 whether to use 12 or 24 hour clock format
		// @return final formatted time string
		const std::wstring TimeStr(const __int64 input_time, bool milTime = false);
	private:
		std::vector<ChatMessage> m_MessageLog;
		render::FragPos m_ChatPos;
	};
}