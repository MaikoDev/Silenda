#include "pch.h"

#include "client/SilendaClient.h"

using namespace render;

int main(int argc, char** argv)
{
	Silenda::NetPacker* packer = Silenda::NetPacker::GetInstance();
	packer->genKeys(4096);

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
		 
	return 0;
}