#include "base.h"

namespace Silenda
{
	Page::Page(const unsigned short length, const unsigned short width)
	{
		m_Mesh = new render::MeshFrame(length, width);
	}

	Page::~Page()
	{
		delete m_Mesh;
	}

	int Page::LinkPage(const std::string& uid, Page* connectedPage, bool overwrite)
	{
		if (m_ConnectedPages.find(uid) != m_ConnectedPages.end())
		{
			// Duplicated uid was found
			if (overwrite)
			{
				m_ConnectedPages[uid] = connectedPage;
				return 1;
			}
			
			return -1;
		}
		else
		{
			// Duplicated uid not found
			m_ConnectedPages[uid] = connectedPage;
			return 0;
		}
	}

	Page* Page::GetConnected(const std::string& uid)
	{
		if (!m_ConnectedPages.empty())
		{
			if (m_ConnectedPages.find(uid) != m_ConnectedPages.end())
				return m_ConnectedPages[uid];

			return nullptr;
		}

		return nullptr;
	}
}