#pragma once

#include "Event.h"

namespace Silenda
{
	class MouseMovedEvent : public EventBase
	{
	public:
		MouseMovedEvent(float x, float y) : m_MouseX(x), m_MouseY(y) {};

		inline const float GetX() const { return m_MouseX; };
		inline const float GetY() const { return m_MouseY; };

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
			EVENT_CLASS_TYPE(MouseMoved)
	private:
		float m_MouseX, m_MouseY;
	};

	class MouseScrolledEvent : public EventBase
	{
	public:
		MouseScrolledEvent(float xOffset, float yOffset) : m_XOffset(xOffset), m_YOffset(yOffset) {};

		inline const float GetXOffset() const { return m_XOffset; };
		inline const float GetYOffset() const { return m_YOffset; };

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
			EVENT_CLASS_TYPE(MouseScrolled)
	private:
		float m_XOffset, m_YOffset;
	};

	class MouseButtonEvent : public EventBase
	{
	public:
		inline int GetMouseButton() const { return m_Button; };

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	protected:
		MouseButtonEvent(int button) : m_Button(button) {};
		int m_Button;
	};

	class MousePressedEvent : public MouseButtonEvent
	{
	public:
		MousePressedEvent(int button) : MouseButtonEvent(button) {};

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class MouseReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseReleasedEvent(int button) : MouseButtonEvent(button) {};

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}