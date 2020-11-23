#include "pch.h"
#include "renderer.h"

#include "../utils/row_major.h"

namespace render
{
	Renderer* Renderer::m_Instance = nullptr;

	Renderer::Renderer()
	{
		// Setup unicode
		_setmode(_fileno(stdout), _O_U16TEXT);

		SetConsoleTitle(LPCSTR("Silenda"));
		DWORD style = GetWindowLong(_hwnd, GWL_STYLE);

		CONSOLE_FONT_INFOEX cfi;
		cfi.cbSize = sizeof(cfi);
		cfi.nFont = 0;
		cfi.dwFontSize.X = 8;
		cfi.dwFontSize.Y = 18;
		cfi.FontFamily = FF_DONTCARE;
		cfi.FontWeight = FW_NORMAL;
		wcscpy_s(cfi.FaceName, _countof(cfi.FaceName), L"Consolas");
		SetCurrentConsoleFontEx(_hOut, FALSE, &cfi);

		style &= ~WS_MAXIMIZEBOX;
		style &= ~WS_MINIMIZEBOX;
		style &= ~WS_HSCROLL;
		style &= ~WS_VSCROLL;
		style &= ~WS_SYSMENU;
		style &= ~WS_SIZEBOX;

		ShowScrollBar(_hwnd, SB_BOTH, FALSE);
		SetWindowLong(_hwnd, GWL_STYLE, style);
		SetWindowPos(_hwnd, NULL, 0, 0, 20, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}

	Renderer* Renderer::GetInstance()
	{
		if (m_Instance == nullptr)
			m_Instance = new Renderer();
		return m_Instance;
	}

	void Renderer::init(const uint& length, const uint& width)
	{
		m_FrameLength = length;
		m_FrameWidth = width;
		uint area = m_FrameLength * m_FrameWidth;

		// Setup of future and current frame buffers
		m_FutureBuffer = std::move(FragBuffer(area));
		m_CurrentBuffer = std::move(FragBuffer(area));

		// Setup depth buffer
		//m_DepthBuffer = std::move(rBuffer<short>(32767, area));

		// Reset console window to size
		SetWindowSize(length, width);
	}

	bool Renderer::draw(Renderable* obj)
	{
		// Check if any of the buffers are empty/uninitialize before starting
		if (m_CurrentBuffer.empty() || m_FutureBuffer.empty())
			return false;

		// Retrieve the obj's mesh
		MeshFrame mesh = obj->OnRender();
		FragPos meshPos = mesh.GetGlobalPos();

		ushort meshLength = mesh.GetLength();
		ushort meshWidth = mesh.GetWidth();

		// Place mesh's fragments on the proper location relative to futurebuffer
		for (ushort y = 0; y < meshWidth; y++)
		{
			for (ushort x = 0; x < meshLength; x++)
			{
				uint meshIndex = row_major(x, y, meshLength);
				uint bufferIndex = row_major(meshPos.x + x, meshPos.y + y, m_FrameLength);

				if (bufferIndex >= m_FutureBuffer.size())
					continue;

				*m_FutureBuffer[bufferIndex] = mesh[meshIndex]->frag;
			}
		}

		return true;
	}

	void Renderer::flush()
	{
		// Check if any of the buffers are empty/uninitialize before starting
		if (m_CurrentBuffer.empty() || m_FutureBuffer.empty())
			return;

		if (m_CurrentBuffer != m_FutureBuffer)
		{
			for (ushort y = 0; y < m_FrameWidth; y++)
			{
				for (ushort x = 0; x < m_FrameLength; x++)
				{
					if (*m_FutureBuffer[row_major(x, y, m_FrameLength)] == *m_CurrentBuffer[row_major(x, y, m_FrameLength)])
						continue;

					setCursorPos(x, y);
					std::wcout << *m_FutureBuffer[row_major(x, y, m_FrameLength)];
					//std::wcout << L'*';
				}
			}

			std::wcout.flush();
		}

		m_CurrentBuffer = m_FutureBuffer;
		return;
	}

	void Renderer::SetWindowSize(const int& width, const int& height)
	{
		RECT rct = { NULL };

		HANDLE _hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO SBInfo;
		COORD SBSize;

		GetWindowRect(_hwnd, &rct);
		MoveWindow(_hwnd, rct.left, rct.top, (width * 8) + 35, (height * 19) + 28, TRUE);

		GetConsoleScreenBufferInfo(_hOut, &SBInfo);
		
		SBSize.X = SBInfo.dwSize.X;
		SBSize.Y = height + 1;

		SetConsoleScreenBufferSize(_hOut, SBSize);
		showCursor(false);
	}

	void Renderer::setCursorPos(const ushort& x, const ushort& y)
	{
		std::wcout.flush();
		COORD coord = { (SHORT)x, (SHORT)y };
		SetConsoleCursorPosition(_hOut, coord);
	}

	void Renderer::showCursor(const bool& showFlag)
	{
		CONSOLE_CURSOR_INFO lpCursor;
		GetConsoleCursorInfo(_hOut, &lpCursor);
		lpCursor.bVisible = false;
		SetConsoleCursorInfo(_hOut, &lpCursor);
	}
}