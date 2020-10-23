#include <iostream>
#include <Windows.h>
#include <io.h>
#include <fcntl.h>

#include "renderer.h"

#include "..\utils\row_major.h"

namespace render
{
	Renderer* Renderer::m_Instance = nullptr;

	Renderer::Renderer()
	{
		// Setup unicode
		_setmode(_fileno(stdout), _O_U16TEXT);

		SetConsoleTitle(LPCSTR("Silenda"));

		HWND hwnd = GetConsoleWindow();
		DWORD style = GetWindowLong(hwnd, GWL_STYLE);

		style &= ~WS_MAXIMIZEBOX;
		style &= ~WS_MINIMIZEBOX;
		style &= ~WS_HSCROLL;
		style &= ~WS_VSCROLL;
		style &= ~WS_SYSMENU;

		ShowScrollBar(hwnd, SB_BOTH, FALSE);
		SetWindowLong(hwnd, GWL_STYLE, style);
		SetWindowPos(hwnd, NULL, 0, 0, 20, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
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
		m_DepthBuffer = std::move(rBuffer<short>(32767, area));
	}

	bool Renderer::draw(Renderable* obj)
	{
		// Check if any of the buffers are empty/uninitialize before starting
		if (m_CurrentBuffer.empty() || m_FutureBuffer.empty() || m_DepthBuffer.empty())
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

				if (bufferIndex >= m_FutureBuffer.size() || bufferIndex >= m_DepthBuffer.size())
					continue;

				// Depth buffer check
				if (mesh[meshIndex]->pos.z < *m_DepthBuffer[bufferIndex])
				{
					*m_FutureBuffer[bufferIndex] = mesh[meshIndex]->frag;

					// Set new depth buffer num at pos
					*m_DepthBuffer[bufferIndex] = mesh[meshIndex]->pos.z;
				}
			}
		}

		return true;
	}

	void Renderer::flush()
	{
		// Check if any of the buffers are empty/uninitialize before starting
		if (m_CurrentBuffer.empty() || m_FutureBuffer.empty() || m_DepthBuffer.empty())
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
				}
			}

			std::wcout.flush();
		}

		m_CurrentBuffer = m_FutureBuffer;
		resetDepth();
		return;
	}

	void Renderer::SetWindowSize(const int& width, const int& height)
	{
		HWND hwnd = GetConsoleWindow();
		RECT rct = { NULL };

		GetWindowRect(hwnd, &rct);
		MoveWindow(hwnd, rct.left, rct.top, width * 10, height * 21, TRUE);
	}

	void Renderer::setCursorPos(const ushort& x, const ushort& y)
	{
		static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		std::wcout.flush();
		COORD coord = { (SHORT)x, (SHORT)y };
		SetConsoleCursorPosition(hOut, coord);
	}

	void Renderer::resetDepth()
	{
		m_DepthBuffer = std::move(rBuffer<short>(32767, m_FrameLength * m_FrameWidth));
	}
}