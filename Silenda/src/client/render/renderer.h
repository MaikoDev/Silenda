#pragma once

#include "framebuffer.h"
#include "renderable.h"

namespace render
{
	const inline void ClearScreen()
	{
		HANDLE                     hStdOut;
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		DWORD                      count;
		DWORD                      cellCount;
		COORD                      homeCoords = { 0, 0 };

		hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hStdOut == INVALID_HANDLE_VALUE) return;

		if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
		cellCount = csbi.dwSize.X * csbi.dwSize.Y;

		if (!FillConsoleOutputCharacter(
			hStdOut,
			(TCHAR) ' ',
			cellCount,
			homeCoords,
			&count
		)) return;

		if (!FillConsoleOutputAttribute(
			hStdOut,
			csbi.wAttributes,
			cellCount,
			homeCoords,
			&count
		)) return;

		SetConsoleCursorPosition(hStdOut, homeCoords);
	}

	class Renderer
	{
	public:
		~Renderer() {};

		static Renderer* GetInstance();

		void init(const uint& length, const uint& width);
		bool draw(Renderable* obj);
		void flush();

		void SetWindowSize(const int& width, const int& height);
	private:
		void setCursorPos(const ushort& x, const ushort& y);
		void showCursor(const bool& showFlag);
	private:
		FragBuffer m_CurrentBuffer;
		FragBuffer m_FutureBuffer;

		ushort m_FrameLength = 0;
		ushort m_FrameWidth = 0;

		const HWND _hwnd = GetConsoleWindow();
		const HANDLE _hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	private:
		Renderer();
		static Renderer* m_Instance;
	};
}