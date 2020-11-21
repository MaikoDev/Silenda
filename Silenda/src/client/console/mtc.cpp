#include "pch.h"
#include "mtc.h"

#include "../utils/iobserver.h"

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

	const void MTConsole::SetMaxBufferSize(const unsigned char& size)
	{
		m_MaxBufferSize = size;
		m_MsgBuffer = m_MsgBuffer.substr(0, m_MaxBufferSize);
		m_MsgRecord.clear();
	};

	void MTConsole::attach(IObserver* obs)
	{
		// Convert address to string
		std::stringstream ss;
		ss << obs;

		std::string addressName = ss.str();

		// Lookup to see if name is already registered
		if (_Olist.find(addressName) == _Olist.end())
			_Olist[addressName] = obs;
	}

	void MTConsole::detach(IObserver* obs)
	{
		// Convert address to string
		std::stringstream ss;
		ss << obs;

		std::string addressName = ss.str();

		auto querry = _Olist.find(addressName);
		if (querry != _Olist.end())
			_Olist.erase(querry);
	}

	void MTConsole::notify(const unsigned char controller)
	{
		// notify and update all observer on list
		for (auto iter = _Olist.begin(); iter != _Olist.end(); iter++)
			iter->second->update(this, controller);
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
				if (length == 1 && input_record.EventType != KEY_EVENT)
				{
					ReadConsoleInput(input_handle, &input_record, 1, &length);
					continue;
				}
				// Filter any events that aren't key events
				else if (length == 1 && input_record.EventType == KEY_EVENT)
				{
					// Filter out key releases, we only want key presses.
					if (input_record.Event.KeyEvent.bKeyDown)
					{
						switch (input_record.Event.KeyEvent.wVirtualKeyCode)
						{
						case VK_RETURN: // return key handle
							postLine();
							break;
						case VK_BACK: // backspace key handle
							if (m_MsgBuffer.size() > 0)
								m_MsgBuffer.pop_back();

							notify();
							break;
						case VK_UP:
							if (m_MsgRecord.size() > 0)
							{
								if (!(m_MsgRecordIndex < m_MsgRecord.size()))
									m_MsgRecordIndex = 0;

								m_MsgBuffer = m_MsgRecord[m_MsgRecordIndex];
								m_MsgRecordIndex++;
								notify();
							}

							break;
						case VK_DOWN:
							if (m_MsgRecord.size() > 0)
							{
								if (m_MsgRecordIndex == 0)
									m_MsgRecordIndex = m_MsgRecord.size() - 1;
								else
									m_MsgRecordIndex--;

								m_MsgBuffer = m_MsgRecord[m_MsgRecordIndex];
								notify();
							}

							break;
						case VK_SHIFT:
							break;
						default: 
							// Only accept unicode letters
							wchar_t letter = input_record.Event.KeyEvent.uChar.UnicodeChar;
							if (letter > 31 && letter < 127)
							{
								// Check if InputBuffer can still accept input according to buffer size
								if (m_MsgBuffer.size() < m_MaxBufferSize)
									postChar(letter);
							}

							break;
						}
					}

					// Remove current input and preceed to next
					ReadConsoleInput(input_handle, &input_record, 1, &length);
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}