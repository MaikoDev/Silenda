#include "pch.h"

#include "EventDispatcher.h"

namespace Silenda
{
	EventDispatcher* EventDispatcher::m_Instance = nullptr;

	EventDispatcher* EventDispatcher::GetInstance()
	{
		if (m_Instance == nullptr)
			m_Instance = new EventDispatcher();
		return m_Instance;
	}

	/*void EventDispatcher::attach(IEventListener* listener)
	{

	}

	void EventDispatcher::detach(IEventListener* listener)
	{

	}

	void EventDispatcher::notify()
	{

	}*/
}