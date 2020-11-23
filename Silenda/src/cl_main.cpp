#include "pch.h"

#include "SilendaClient.h"

using namespace render;

int main(int argc, char** argv)
{
	if (!Silenda::SilendaInit())
	{
		std::cin.get();
		return 0;
	}

	Silenda::MTConsole* consolePtr = Silenda::MTConsole::GetInstance();
	Silenda::MsgHandler* handlerPtr = Silenda::MsgHandler::GetInstance();

	Renderer* rPtr = Renderer::GetInstance();

	Silenda::Page* chat = new Silenda::ChatPage();
	Silenda::Page* login = new Silenda::LoginPage();

	login->LinkPage("SilendaChat", chat);
	chat->LinkPage("SilendaLogin", login);

	login->load();

	rPtr->init(117, 29);
	Silenda::BaseRunningState = true;

	while (Silenda::BaseRunningState)
	{
		rPtr->draw(Silenda::LoadedPage);
		rPtr->flush();

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	render::ClearScreen();
		 
	return 0;
}