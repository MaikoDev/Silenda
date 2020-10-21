#include "mtc.h"

#include "..\utils\iobserver.h"

namespace Silenda
{
	MTConsole* MTConsole::m_Instance = nullptr;

	MTConsole::MTConsole()
	{
		m_ThreadRunning = true;
		m_ConsoleWorker = std::thread(&MTConsole::OnThreadTick, this);
	}

	MTConsole::~MTConsole()
	{
		m_ThreadRunning = false;
		m_ConsoleWorker.join();
	}

	MTConsole* MTConsole::GetInstance()
	{
		if (m_Instance == nullptr)
			m_Instance = new MTConsole();
		return m_Instance;
	}

	void MTConsole::attach(IObserver* obs)
	{

	}

	void MTConsole::detach(IObserver* obs)
	{

	}

	void MTConsole::notify()
	{

	}

	void MTConsole::OnThreadTick()
	{
		HANDLE input_handle = GetStdHandle(STD_INPUT_HANDLE);
		INPUT_RECORD input_record;
		DWORD length = 0;

		while (m_ThreadRunning)
		{
			// Determine if there are any inputs to handle
			if (PeekConsoleInput(input_handle, &input_record, 1, &length))
			{
				// Filter any events that aren't key events
				if (length == 1 && input_record.EventType == KEY_EVENT)
				{
					// Filter out key releases, we only want key presses.
					if (input_record.Event.KeyEvent.bKeyDown)
					{

					}
				}
			}

			// Remove current input and preceed to next
			ReadConsoleInput(input_handle, &input_record, 1, &length);
		}
	}
}