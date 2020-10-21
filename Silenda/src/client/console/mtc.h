#pragma once

#include <string>
#include <thread>
#include <Windows.h>

#include "..\utils\iobservable.h"

namespace Silenda
{
	class MTConsole : public IObservable
	{
	public:
		static MTConsole* GetInstance();

		~MTConsole();

		inline const std::string& GetInputBuffer() const& { return m_InputBuffer; };
		const void SetMaxBufferSize(const byte& size) { m_MaxBufferSize = size; };
		inline const byte& GetMaxBufferSize() const& { return m_MaxBufferSize; };

		// IObservable overrides //

		void attach(IObserver* obs) override;
		void detach(IObserver* obs) override;
		void notify() override;
	private:
		void OnThreadTick();
	private:
		std::thread m_ConsoleWorker;
		bool m_ThreadRunning = false;

		std::string m_InputBuffer;
		byte m_MaxBufferSize;
	private:
		static MTConsole* m_Instance;
		MTConsole();
	};
}