#include "pch.h"

#include "SilendaClient.h"

#include "event/KeyEvent.h"
#include "event/EventDispatcher.h"

using namespace render;

void DoStuff(int num, bool state)
{
	
}

void DoOtherStuff(char h, std::string name)
{

}

bool doEventStuff(Silenda::KeyPressedEvent& e)
{
	return 0;
}

class TestClass : public Silenda::IEventListener
{
public:
	void onEvent(Silenda::Event& e)
	{
		Silenda::EventDispatcher::GetInstance()->dispatch<Silenda::KeyPressedEvent>(std::bind(&TestClass::onKeyPressEvent, this, std::placeholders::_1, std::placeholders::_2), e);
	}
private:
	void onKeyPressEvent(bool& handled, Silenda::KeyPressedEvent& e)
	{
		printf("Hello EventDispatcher!");

		handled = true;
	}
};

int main(int argc, char** argv)
{
	auto dispatcherPtr = Silenda::EventDispatcher::GetInstance();

	Silenda::KeyPressedEvent myKeyEvent(73, 9999);

	TestClass tul;

	dispatcherPtr->attach<Silenda::KeyPressedEvent>(&tul);
	
	//dispatcherPtr->hookAdd<Silenda::KeyPressedEvent>("myIdentifier", std::bind(&doEventStuff, std::placeholders::_1));

	dispatcherPtr->trigger(myKeyEvent);

	while (true)
	{

	}

	std::function<void(int, bool)> tFunc1 = std::bind(&DoStuff, std::placeholders::_1, std::placeholders::_2);
	std::function<void(char, std::string)> tFuncZ = std::bind(&DoOtherStuff, std::placeholders::_1, std::placeholders::_2);

	std::function<void()> tFunc2 = std::bind(tFuncZ, 'Z', "Hello World!");

	Silenda::ThreadPoolExecutor::GetInstance()->scheduleProcess(tFunc2);

	//std::function<void()> tFunc2 = std::bind(reinterpret_cast<(*void)(int, bool)>, 1, false);

	dispatcherPtr->trigger(myKeyEvent);

	//dispatcherPtr->dispatch();

	//std::function<void()>func = std::bind(&DoStuff, 2, false);

	/*auto threadp = Silenda::ThreadPoolExecutor::GetInstance();
	threadp->scheduleProcess(DoStuff, 3, true);
	threadp->scheduleProcess(DoOtherStuff, 'p', "Hello World!");

	while (true)
	{

	}

	std::function<void()>testFunc = std::bind(&DoStuff, 2, false);
	std::function<void()>otherFunc = std::bind(&DoOtherStuff, 'd', "Hello World!");*/

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