#include "Window.h"

#include <windowsx.h>

#include <stdexcept>

#include <iostream>

uint32_t Window::numWindows = 0;
void* Window::mainFiber = nullptr;

void Window::staticFiberProc(void* instance) {
	((Window*)instance)->fiberProc();
}

void Window::fiberProc() {
	bool f11Prev = false;
	while(true) {
		if(GetKeyState(VK_F11) & 0x80) {
			if(!f11Prev)
				SendMessageW(wnd, WM_HOTKEY, 0xF11, 0);
			f11Prev = true;
		} else
			f11Prev = false;

		MSG msg;
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		SwitchToFiber(mainFiber);
	}
}

Window::Window(VkInstance& instance, uint32_t x, uint32_t y, uint32_t width, uint32_t height):
		instance(instance), shouldClose(false), isMinimized(false), isMaximized(false), isResized(false) {
	if(numWindows++ == 0) {
		mainFiber = ConvertThreadToFiber(0);
		std::cout << "Creating First Window." << "\n";
	}

	messageFiber = CreateFiber(0, staticFiberProc, this);

	WCHAR className[] = L"win32app_";
	className[8] = L'A' + numWindows;
	std::wcout << className << "\n";

	WNDCLASSW win_class{};
    win_class.hInstance = GetModuleHandle(NULL);
    win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    win_class.lpszClassName = className;
    win_class.lpfnWndProc = staticWndProc;

    if (!RegisterClassW(&win_class)) {
        std::cout << "Error registering Window class:" << GetLastError() << "\n";
        exit(-1);
    }

	// DWORD extendedWindowStyle = WS_EX_APPWINDOW | WS_EX_ACCEPTFILES | WS_EX_TOPMOST; // WS_EX_TOOLWINDOW, // WS_EX_APPWINDOW
	DWORD extendedWindowStyle = WS_EX_APPWINDOW; // | (WS_EX_TOPMOST*0) | WS_EX_NOREDIRECTIONBITMAP*0;

	DWORD windowStyle = WS_VISIBLE | WS_OVERLAPPEDWINDOW; /*WS_POPUP*0 | WS_SYSMENU | WS_MAXIMIZEBOX*/

	RECT clientSize{};
	clientSize.right = width;
	clientSize.bottom = height;
	AdjustWindowRect(&clientSize, windowStyle, true);

	CreateWindowExW(
		extendedWindowStyle,
		win_class.lpszClassName, // lpClassName
		L"Title", // lpWindowName
		windowStyle,
		x, // x
		y, // y
		clientSize.right-clientSize.left, // width
		clientSize.bottom-clientSize.top, // height
		NULL, // hWndParent
		NULL, // hMenu
		win_class.hInstance, // hInstance
		this  // lpParam
		);

    ShowWindow(wnd, SW_SHOW);
    UpdateWindow(wnd);

	// RegisterHotKey(wnd, 0xF11, MOD_NOREPEAT, VK_F11);

	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.pNext = NULL;
	surfaceCreateInfo.flags = 0;
	surfaceCreateInfo.hinstance = win_class.hInstance;
	surfaceCreateInfo.hwnd = wnd;

	if(vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, NULL, &surface) != VK_SUCCESS)
		throw std::runtime_error("failed to create window surface!");
}

Window::~Window() {
	vkDestroySurfaceKHR(instance, surface, nullptr);
	DestroyWindow(wnd);
	if(--numWindows == 0) {
		ConvertFiberToThread();
		std::cout << "Destroyed last Window." << "\n";
	}
}

void Window::pollMsg() {
	SwitchToFiber(messageFiber);
};

bool Window::noArea() const {
	VkExtent2D winSize{ getSize() };
	return winSize.width == 0 || winSize.height == 0;
}

LRESULT Window::wndProc(UINT msg, WPARAM wParam, LPARAM lParam) {
	const auto maximize = [&]()->void {
		isResized = true;
		isMaximized = true;
		isMinimized = false;
		// SetWindowLong(wnd, GWL_STYLE, GetWindowLong(wnd, GWL_STYLE) | (WS_MAXIMIZE));
		// SetWindowPos(wnd, NULL, 0, 0, 1920, 1080, SWP_NOZORDER);
		SetWindowLong(wnd, GWL_STYLE, GetWindowLong(wnd, GWL_STYLE) & ~(WS_SYSMENU | WS_THICKFRAME | WS_CAPTION));
		ShowWindow(wnd, SW_MAXIMIZE);
		// UpdateWindow(wnd);
	};
	const auto minimize = [&]()->void {
		isResized = true;
		isMaximized = false;
		isMinimized = true;
		ShowWindow(wnd, SW_MINIMIZE);
	};
	const auto restore = [&]()->void {
		isResized = true;
		isMaximized = false;
		isMinimized = false;
		SetWindowLong(wnd, GWL_STYLE, GetWindowLong(wnd, GWL_STYLE) | WS_OVERLAPPEDWINDOW);
		ShowWindow(wnd, SW_RESTORE);
	};
	switch (msg) {
	// case WM_SIZE:
	// 	// std::cout << "WM Size\n";
	// 	switch(wParam) {
	// 		case SIZE_MAXIMIZED:
	// 			std::cout << "Size Maximized\n";
	// 			maximize();
	// 			return 0;

	// 		case SIZE_MINIMIZED:
	// 			std::cout << "Size Minimized\n";
	// 			minimize();
	// 			return 0;

	// 		case SIZE_RESTORED:
	// 			std::cout << "Size Restored\n";
	// 			restore();
	// 			return 0;
	// 	}
	// 	break;

	case WM_ENTERSIZEMOVE:
		std::cout << "WM EnterSizeMove\n";
		SetTimer(wnd, 0x187420, 0, NULL);
		break;

	// case WM_MOUSEMOVE:
	// 	break;

	case WM_SIZING:
		// std::cout << "WM Sizing\n";
		isResized = true;
		// SwitchToFiber(mainFiber);
		break;

	case WM_EXITSIZEMOVE:
		std::cout << "WM ExitSizeMove\n";
		KillTimer(wnd, 0x187420);
		break;

	case WM_TIMER:
		// std::cout << "Timer\n";
		SwitchToFiber(mainFiber);
		break;

	case WM_HOTKEY:
		switch(wParam) {
			case 0xF11:
				isResized = true;
				SendMessage(wnd, WM_SYSCOMMAND, isMaximized ? SC_RESTORE : SC_MAXIMIZE, 0);
				break;
		}
		break;
	
	case WM_SYSCOMMAND:
		switch(wParam & 0xFFF0) {
			case SC_MAXIMIZE:
				std::cout << "SC Maximize\n";
				maximize();
				return 0; // Signal that messages are processed manually

			case SC_MINIMIZE:
				std::cout << "SC Minimize\n";
				minimize();
				return 0; // Signal that messages are processed manually

			case SC_RESTORE:
				std::cout << "SC Restore\n";
				restore();
				return 0; // Signal that messages are processed manually

			case SC_CLOSE:
				std::cout << "SC Close\n";
				shouldClose = true;
				return 0; // Signal that messages are processed manually
		}
	}
	return DefWindowProcW(wnd, msg, wParam, lParam);
}

LRESULT WINAPI Window::staticWndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	Window* targetWindow;
	if(msg == WM_NCCREATE) {
		LPCREATESTRUCT lpcs = (LPCREATESTRUCT)(lParam);
		targetWindow = (Window*)(lpcs->lpCreateParams);
		targetWindow->wnd = wnd;
		// targetWindow->device_context = GetDC(wnd);
		SetWindowLongPtr(wnd, GWLP_USERDATA, (LONG_PTR)(targetWindow));
	} else {
 		targetWindow = (Window*)GetWindowLongPtr(wnd, GWLP_USERDATA);
	}
	if (targetWindow)
		return targetWindow->wndProc(msg, wParam, lParam);
	return DefWindowProc(wnd, msg, wParam, lParam);
}