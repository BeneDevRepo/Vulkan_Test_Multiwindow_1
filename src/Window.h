#pragma once

// #define WINVER 0x0602 // Windows 8 = minimum for noRedirectionbitmap
#include <windows.h>

#define VK_USE_PLATFORM_WIN32_KHR
// #define VK_PROTOTYPES
#include <vulkan/vulkan.h>

class Window {
	static uint32_t numWindows;
	static void* mainFiber;
	void* messageFiber;
	static void staticFiberProc(void* instance);
	void fiberProc();

private:
	static LRESULT WINAPI staticWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);
	LRESULT wndProc(UINT msg, WPARAM wparam, LPARAM lparam);

private:
	VkInstance& instance;

public:
	HWND wnd;
	VkSurfaceKHR surface;
	bool shouldClose, isMinimized, isMaximized, isResized;

public:
	Window(VkInstance& instance, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
	~Window();
	void pollMsg();
	inline VkExtent2D getSize() const;
	inline POINT getPos() const;
	inline POINT getMouseAbsolute() const;
	inline POINT getMouseRel() const;
	bool noArea() const;
};

inline VkExtent2D Window::getSize() const {
	RECT clientRect{};
	GetClientRect(wnd, &clientRect);
	return {(uint32_t)(clientRect.right-clientRect.left), (uint32_t)(clientRect.bottom-clientRect.top)};
}

inline POINT Window::getPos() const {
	RECT winRect{};
	GetWindowRect(wnd, &winRect);
	return {winRect.left, winRect.top};
}

inline POINT Window::getMouseAbsolute() const {
	POINT mouse{};
	GetCursorPos(&mouse);
	return mouse;
}

inline POINT Window::getMouseRel() const {
	POINT mouse{ getMouseAbsolute() };
	POINT clientOffsetAbsolute;
	ClientToScreen(wnd, &clientOffsetAbsolute);
	return {mouse.x - clientOffsetAbsolute.x, mouse.y - clientOffsetAbsolute.y};
}