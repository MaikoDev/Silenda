#include "pch.h"

#include "SilendaClient.h"

#include "thread/threadpool.h"
#include "thread/pipeline.h"
#include "thread/pipedworker.h"

using namespace render;

void DoStuff(int num, bool state)
{
	
}

void DoOtherStuff(char h, std::string name)
{

}

int main(int argc, char** argv)
{
	std::function<void()>func = std::bind(&DoStuff, 2, false);

	auto tp = Silenda::ThreadPoolExecutor::GetInstance();
	tp->scheduleProcess(DoStuff, 345, false);
	tp->scheduleProcess(DoOtherStuff, 'j', "Hey ThreadPool nice to see you working!");

	while (true)
	{

	}

	//std::function<void()>func = std::bind(&DoStuff, 2, false);

	std::function<void()>testFunc = std::bind(&DoStuff, 2, false);
	std::function<void()>otherFunc = std::bind(&DoOtherStuff, 'd', "Hello World!");

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

		Silenda::NetClient::GetInstance()->handleCallbacks();

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	render::ClearScreen();
		 
	return 0;
}