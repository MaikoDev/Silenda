#pragma once

class BasicTimer
{
public:
	BasicTimer() {};
	~BasicTimer() {};

	inline const void start() { m_StartPoint = std::chrono::high_resolution_clock::now(); };
	inline const void stop() { m_EndPoint = std::chrono::high_resolution_clock::now(); };
	inline const void reset() { m_StartPoint = m_EndPoint; };

	template<class T, class D>
	inline const T elapsed() {
		if (m_StartPoint > m_EndPoint)
			return 0;
		return std::chrono::duration_cast<std::chrono::duration<T, D>>(m_EndPoint - m_StartPoint).count();
	}

	template<class T, class D>
	inline const T elapsed_now() {
		return std::chrono::duration_cast<std::chrono::duration<T, D>>(std::chrono::high_resolution_clock::now() - m_StartPoint).count();
	}

	inline const std::chrono::high_resolution_clock::time_point& GetStartPoint() const& { return m_StartPoint; };
	inline const std::chrono::high_resolution_clock::time_point& GetEndPoint() const & { return m_EndPoint; };
private:
	std::chrono::high_resolution_clock::time_point m_StartPoint;
	std::chrono::high_resolution_clock::time_point m_EndPoint;
};