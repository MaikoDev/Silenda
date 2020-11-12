#pragma once

#include <string>
#include <unordered_map>

#include "..\render\renderable.h"
#include "..\render\mesh\mesh.h"
#include "..\console\msghandler.h"

namespace Silenda
{
	extern std::wstring DisplayName;
	extern bool BaseRunningState;

	class Page : public render::Renderable, public IObserver
	{
	public:
		// Base Page Class
		// @param1 length of the page
		// @param2 width of the page
		Page(const unsigned short length, const unsigned short width);
		~Page();

		void load();
		void unload();
		virtual void initPostLoad() {};

		// LinkPage
		// @param1 unique string identifier
		// @param2 ptr to associated page
		// @param3 whether the function should overwrite map entry if similar uid is found
		// @return 0 = success, 1 = overwrite, -1 = identical uid, action skipped
		int LinkPage(const std::string& uid, Page* connectedPage, bool overwrite = false);

		// GetConnected
		// @param1 unique string identifier
		Page* GetConnected(const std::string& uid);
	protected:
		render::MeshFrame* m_Mesh;
		std::unordered_map<std::string, Page*> m_ConnectedPages;
		bool m_IsActivePage = false;
	};

	extern Page* LoadedPage;
}