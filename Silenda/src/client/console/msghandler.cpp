#include "msghandler.h"

#include <iostream>
#include <sstream>

namespace Silenda
{
	MsgHandler* MsgHandler::m_Instance = nullptr;

	MsgHandler::MsgHandler()
	{
		MTConsole::GetInstance()->attach(this);
	}

	MsgHandler* MsgHandler::GetInstance()
	{
		if (m_Instance == nullptr)
			m_Instance = new MsgHandler();
		return m_Instance;
	}

	MsgHandler::~MsgHandler()
	{
		MTConsole::GetInstance()->detach(this);
	}

	void MsgHandler::attach(IObserver* obs)
	{
		// Convert address to string
		std::stringstream ss;
		ss << obs;

		std::string addressName = ss.str();

		// Lookup to see if name is already registered
		if (_Olist.find(addressName) == _Olist.end())
			_Olist[addressName] = obs;
	}

	void MsgHandler::detach(IObserver* obs)
	{
		// Convert address to string
		std::stringstream ss;
		ss << obs;

		std::string addressName = ss.str();

		auto querry = _Olist.find(addressName);
		if (querry != _Olist.end())
			_Olist.erase(querry);
	}

	void MsgHandler::notify(const unsigned char controller)
	{
		for (auto iter = _Olist.begin(); iter != _Olist.end(); iter++)
			iter->second->update(this, controller);
	}

	void MsgHandler::update(IObservable* src, const unsigned char controller)
	{
		if (controller == 1)
		{
			MTConsole* console = (MTConsole*)src;
			
			m_LastHandle = parse(console->GetMsgRecord()[0]);
			if (m_LastHandle.size() == 1)
			{
				if (m_LastHandle.front().MessageType == MSG_TXT) // notify observers who only care for txt messages
					notify(3);
			}

			notify(4); // notify observers who only care for command messages
		}
	}

	std::queue<std::wstring> MsgHandler::separate(const std::wstring& input)
	{
		std::queue<std::wstring> retQueue;

		unsigned int commandPtr = 0, readPtr = 0;
		size_t inputLimit = input.size() - 1;

		for (; commandPtr != inputLimit; readPtr++)
		{
			if (readPtr == inputLimit)
			{
				retQueue.push(input.substr(commandPtr, (readPtr - commandPtr) + 1));
				commandPtr = readPtr;
			}
			else if (input[readPtr] == L';')
			{
				retQueue.push(input.substr(commandPtr, readPtr - commandPtr));
				commandPtr = readPtr;
			}
			else if (input[readPtr] == L'/')
				commandPtr = readPtr;
		}

		return retQueue;
	}

	std::queue<MsgHandle> MsgHandler::parse(const std::wstring& input)
	{
		std::queue<MsgHandle> retHandle;

		MsgHandle bufferHandle;

		unsigned int separatorPtr = 0;
		// Determines if input is msg or command
		for (; separatorPtr < input.size(); separatorPtr++)
		{
			if (input[separatorPtr] == L' ')
				continue;
			else if (input[separatorPtr] == L'/')
				break;
			else
				break;
		}

		if (input[separatorPtr] == L'/') // Command message
		{
			bufferHandle.MessageType = MSG_COMMAND;

			// Create a queue of commands that need to processed
			std::queue<std::wstring> commandQueue = separate(input);

			for (unsigned int commandIndex = 0; !(commandQueue.empty()); commandIndex++)
			{
				std::wstring currentCommand = commandQueue.front();
				size_t inputLimit = currentCommand.size();
				for (unsigned int readPtr = 0, valuePtr = 0, keyPtr = 0; keyPtr != inputLimit; readPtr++)
				{
					if (readPtr == inputLimit) 
					{
						if (valuePtr > keyPtr)
						{
							bufferHandle.Message.Value.push_back(commandQueue.front().substr(valuePtr + 1, readPtr - (valuePtr + 1)));
							valuePtr = readPtr;
						}

						if (bufferHandle.Message.Key == L"")
							bufferHandle.Message.Key = commandQueue.front().substr(keyPtr + 1, readPtr - (keyPtr + 1));
						keyPtr = readPtr;
					}
					else if (currentCommand[readPtr] == L' ') // end of key, beginning of first value
					{
						bufferHandle.Message.Key = commandQueue.front().substr(keyPtr + 1, readPtr - (keyPtr + 1));
						valuePtr = readPtr;
					}
					else if (currentCommand[readPtr] == L',') // end of value, start of next value
					{
						bufferHandle.Message.Value.push_back(commandQueue.front().substr(valuePtr + 1, readPtr - (valuePtr + 1)));
						valuePtr = readPtr;
					}
				}

				retHandle.push(bufferHandle);

				commandQueue.pop();
				bufferHandle.Message.Value.clear();
			}
		}
		else // Txt message
		{
			bufferHandle.MessageType = MSG_TXT;
			bufferHandle.Message.Value.push_back(input);
			
			retHandle.push(bufferHandle);
		}

		return retHandle;
	}
}