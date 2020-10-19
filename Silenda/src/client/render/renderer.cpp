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
		_setmode(_fileno(stdout), _O_U16TEXT);
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
					if (m_FutureBuffer[row_major(x, y, m_FrameLength)] == m_CurrentBuffer[row_major(x, y, m_FrameLength)])
						continue;

					setCursorPos(x, y);
					std::wcout << *m_FutureBuffer[row_major(x, y, m_FrameLength)];
				}
			}

			std::wcout.flush();
		}

		m_CurrentBuffer = m_FutureBuffer;
		return;
	}

	void Renderer::setCursorPos(const ushort& x, const ushort& y)
	{
		static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		std::wcout.flush();
		COORD coord = { (SHORT)x, (SHORT)y };
		SetConsoleCursorPosition(hOut, coord);
	}
}