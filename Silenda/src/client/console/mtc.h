#pragma once

#include <string>
#include <deque>
#include <thread>

#define NOMINMAX
#include <Windows.h>

#include "..\utils\iobservable.h"

namespace Silenda
{
	class MTConsole : public IObservable
	{
	public:
		static MTConsole* GetInstance();

		~MTConsole();

		inline const std::wstring& GetMsgBuffer() const& { return m_MsgBuffer; };
		inline const std::deque<std::wstring>& GetMsgRecord() const& { return m_MsgRecord; };
		const void SetMaxBufferSize(const unsigned char& size);
		inline const unsigned char& GetMaxBufferSize() const& { return m_MaxBufferSize; };

		// IObservable overrides //

		void attach(IObserver* obs) override;
		void detach(IObserver* obs) override;
		void notify(const unsigned char controller = 0) override;
	private:
		// postChar: post a single char to current msgbuffer
		// @param1 wchar to be pushed to msgbuffer
		inline void postChar(const wchar_t& input) { m_MsgBuffer += input; notify(); };

		// postLine: removes current msg on buffer and post it to the msgrecord
		inline void postLine() { 
			if (!(m_MsgRecord.size() < 0xFF))
				m_MsgRecord.pop_front();
				
			m_MsgRecord.push_back(m_MsgBuffer); m_MsgBuffer = L""; notify(1);
			notify();
		};

		void OnThreadTick();
	private:
		std::thread m_ConsoleWorker;
		bool m_ThreadRunning = false;

		std::wstring m_MsgBuffer;
		unsigned char m_MaxBufferSize = 10;

		std::deque<std::wstring> m_MsgRecord;
		unsigned char m_MsgRecordIndex = 0;
	private:
		static MTConsole* m_Instance;
		MTConsole();
	};
}