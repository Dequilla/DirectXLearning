#pragma once
#include <Windows.h>
#include <string>

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long uint64;

inline std::wstring stringToWstring(std::string string)
{
	std::wstring ws;
	ws.assign(string.begin(), string.end());
	return ws;
}

class ApplicationContext
{
private:
	ApplicationContext() {}

protected:
	HINSTANCE m_hInstance; // Application handle

public:
	ApplicationContext(ApplicationContext const&) = delete;
	void operator=(ApplicationContext const&) = delete;

	static ApplicationContext& getInstance()
	{
		static ApplicationContext instance;
		return instance;
	}

#ifdef _WIN32
	void setAppHandle(HINSTANCE hInstance);
	HINSTANCE getAppHandle();
#endif
};

#ifdef _WIN32
inline void ApplicationContext::setAppHandle(HINSTANCE hInstance)
{
	m_hInstance = hInstance;
}

inline HINSTANCE ApplicationContext::getAppHandle()
{
	return m_hInstance;
}
#endif