#pragma once

#include "Event.h"
#include "thread/threadpool.h"

namespace Silenda
{
	class EventHookBase
	{
	public:
		virtual ~EventHookBase() {};
	protected:
		EventHookBase() {};
	};

	template <class... ArgTypes>
	class EventHook : public EventHookBase
	{
		typedef std::function<void(bool&, ArgTypes...)> Callable;
	public:
		EventHook() {};
		EventHook(Callable f) : m_Func(f) {}
		void operator()(ArgTypes... args) { if (m_Func) m_Func(args...); };

		Callable& getCallable() { return m_Func; };
	private:
		Callable m_Func;
	};

	// Forward declaration of EventListener
	class IEventListener
	{
	public:
		virtual ~IEventListener() = default;
		virtual void onEvent(Event& e) = 0;
	protected:
		IEventListener() = default;
	};

	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<void(bool&, T&)>;

		typedef std::function<void()> EventProcessType;
		typedef std::shared_ptr<EventHookBase> EventHookBasePtr;
	public:
		static EventDispatcher* GetInstance();

		template<typename T>
		void attach(IEventListener* listener)
		{
			std::stringstream ss;
			ss << listener;

			std::string addressName = ss.str();

			auto eventTypeIter = m_ListenerList.find(T::GetStaticName());
			if (eventTypeIter != m_ListenerList.end())
			{
				eventTypeIter->second.insert({ addressName, listener });
			}
			else
			{
				m_ListenerList[T::GetStaticName()] = { { addressName, listener } };
			}
		}

		template<typename T>
		void detach(IEventListener* listener)
		{
			std::stringstream ss;
			ss << listener;

			std::string addressName = ss.str();

			auto eventTypeIter = m_ListenerList.find(T::GetStaticName());
			if (eventTypeIter != m_ListenerList.end())
			{
				auto& listenerMapRef = eventTypeIter->second;
				auto listenerIter = listenerMapRef.find(addressName);

				if (listenerIter != listenerMapRef.end())
					listenerMapRef.erase(listenerIter);
			}
		}

		template<typename T>
		void notify(Event& e)
		{
			auto eventTypeIter = m_ListenerList.find(T::GetStaticName());
			if (eventTypeIter != m_ListenerList.end())
			{
				auto& listenerMapRef = eventTypeIter->second;
				for (auto listenerIter = listenerMapRef.begin(); listenerIter != listenerMapRef.end(); listenerIter++)
				{
					listenerIter->second->onEvent(e);
				}
			}
		}

		// Used by any class which wishes to have layer-like implimentation of event handling. Events can be blocked by returning true in handling client-class code.
		template<class T>
		void dispatch(EventFn<T> func, Event& eventInfo)
		{
			if (eventInfo.GetEventType() == T::GetStaticType())
			{
				ThreadPoolExecutor::GetInstance()->scheduleProcess(func, eventInfo.m_EventBasePtr.get()->m_Handled, (*(T*)eventInfo.m_EventBasePtr.get()));
			}
		}

		template<class T>
		void trigger(T& e)
		{
			Event triggeredEvent(std::make_shared<T>(e));
			m_TriggeredEvents.push(triggeredEvent);

			// Notify all listeners
			notify<T>(triggeredEvent);


		}
	private:
		template<typename T, class... ArgTypes>
		void scheduleHookCalls(ArgTypes... args)
		{
			typedef EventHook<T&> hookType;

			auto eventIter = m_EventHookMap.find(T::GetStaticName());
			if (eventIter != m_EventHookMap.end())
			{
				auto& eventMapRef = eventIter->second;

				for (auto hookIter = eventMapRef.begin(); hookIter != eventMapRef.end(); hookIter++)
				{
					hookType* h = dynamic_cast<hookType*>(hookIter->second.get());

					if (h)
					{
						auto hookFunc = h->getCallable();
						this->scheduleProcess(hookFunc, args...);
					}
				}
			}
		}

		template<typename Func, class... ArgTypes>
		void scheduleProcess(Func&& funcPtr, ArgTypes&& ... args)
		{
			std::function<void()> processFunc = std::bind(std::forward<Func>(funcPtr), std::ref(args)...);

			m_ProcessQueue.push(processFunc);
		}
	private:
		EventDispatcher() {};
		static EventDispatcher* m_Instance;
		ThreadPoolExecutor* m_ThreadPoolPtr = ThreadPoolExecutor::GetInstance();

		std::queue<Event> m_TriggeredEvents;
		std::queue<EventProcessType> m_ProcessQueue;

		std::unordered_map<std::string, std::unordered_map<std::string, IEventListener*>> m_ListenerList;

		std::unordered_map<std::string, std::unordered_map<std::string, EventHookBasePtr>> m_EventHookMap;
	};
}