#pragma once

#include "../utils/iobserver.h"
#include "mtc.h"

#define MSG_TXT 0x0
#define MSG_COMMAND 0x1

namespace Silenda
{
	struct Msg
	{
		std::wstring Key = L"";
		std::deque<std::wstring> Value;
	};

	struct MsgHandle
	{
		Msg Message;
		unsigned char MessageType;
	};

	class MsgHandler : public IObserver, public IObservable
	{
	public:
		static MsgHandler* GetInstance();
		~MsgHandler();

		void attach(IObserver* obs) override;
		void detach(IObserver* obs) override;
		void notify(const unsigned char controller = 0) override;

		void update(IObservable* src, const unsigned char controller = 0) override;

		inline const std::queue<MsgHandle>& GetLastHandle() const & { return m_LastHandle; };
		inline std::queue<MsgHandle> CopyLastHandle() { return m_LastHandle; };
	private:
		// separate - separates string into individual commands via ';' character
		// @param1 input string to separate
		// @return queue of string commands in the order they were typed.
		std::queue<std::wstring> separate(const std::wstring& input);

		std::queue<MsgHandle> parse(const std::wstring& input);
	private:
		std::queue<MsgHandle> m_LastHandle;
	private:
		static MsgHandler* m_Instance;
		MsgHandler();
	};
}