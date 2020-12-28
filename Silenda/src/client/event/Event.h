#pragma once

namespace Silenda
{
	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		ChatJoin, ChatLeave, ChatLog, ChatMsg,
		NetMessageSend, NetMessageReceived, NetDeny, NetPKValidate, NetPKSend, NetPKReceived, NetPKRequest, NetAuthPassed, NetAuthFailed, NetUUIDRequest, NetUUIDSend, NetUUIDReceived,
		KeyPressed, KeyReleased,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		None                      = 0,
		EventCategoryApplication  = 1 << 0,
		EventCategoryNetwork      = 1 << 1,
		EventCategoryInput        = 1 << 2,
		EventCategoryKeyboard     = 1 << 3,
		EventCategoryMouse        = 1 << 4,
		EventCategoryMouseButton  = 1 << 5
	};

	#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
                                   static const char* GetStaticName() { return #type; }\
                                   virtual EventType GetEventType() const override { return GetStaticType(); }\
                                   virtual const char* GetName() const override { return #type; }

	#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	class EventDispatcher;

	class EventBase
	{
		friend class EventDispatcher;
	public:
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;

		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}
	protected:
		bool m_Handled = false;
	};

	// class wrapper for EventBase
	class Event
	{
		friend class EventDispatcher;
	public:
		Event() {};
		Event(std::shared_ptr<EventBase> basePtr) : m_EventBasePtr(basePtr) {};
		Event(const Event& e) : m_EventBasePtr(e.m_EventBasePtr) {};
		Event(Event&& e) noexcept { this->m_EventBasePtr.swap(e.m_EventBasePtr); };

		~Event() {};

		Event& operator=(std::shared_ptr<EventBase> basePtr) { this->m_EventBasePtr = basePtr; };
		Event& operator=(const Event& e) { this->m_EventBasePtr = e.m_EventBasePtr; };
		Event& operator=(Event&& e) noexcept { this->m_EventBasePtr.reset(); this->m_EventBasePtr.swap(e.m_EventBasePtr); };

		const inline EventType GetEventType() const { return m_EventBasePtr->GetEventType(); };
		inline const char* GetName() const { return m_EventBasePtr->GetName(); };
		inline int GetCategoryFlags() const { return m_EventBasePtr->GetCategoryFlags(); };
	private:
		std::shared_ptr<EventBase> m_EventBasePtr = nullptr;
	};
}