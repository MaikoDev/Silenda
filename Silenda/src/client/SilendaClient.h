#pragma once

#include "network/netclient.h"
#include "render/renderer.h"
#include "fileio/fio.h"
#include "page/pages.h"

namespace Silenda
{
	bool G_SilendaInitPassed = false;

	const bool SilendaInit()
	{
		if (!G_SilendaInitPassed)
		{
			printf("Initializing...");
			if (!Silenda::NetPacker::GetInstance()->genKeys(4096))
			{
				printf("\nUnable to generate RSA keys!");
				std::cin.get();
				return 0;
			}

			printf("\nConnecting to network...");

			int connection = NetClient::GetInstance()->Connect("127.0.0.1", 54000);

			if (connection == WSAECONNREFUSED)
			{
				printf("\nFailed to contact network!");
				return 0;
			}

			G_SilendaInitPassed = true;
			render::ClearScreen();
		}

		return 1;
	}
}