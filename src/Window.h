#pragma once
#pragma comment(linker,"\"/manifestdependency:type='win32' \ name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \ processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <CommCtrl.h>
#include <string>
#include <sstream>
#include <memory>
#define WIN_NAME "AnyCopy"
#define WIN_WIDTH 400
#define WIN_HEIGHT 200
#define BTN_ID_START 0x690
#define BTN_ID_CANCEL 0x691
#define TIMER_ID_DELAY 0x420
#define TIMER_ID_PASTE 0x421
#define PASTE_DELAY 50
#define STATUSTXT_READY "Ready"
#define STATUSTXT_WAITING "Waiting delay"
#define STATUSTXT_PASTING "Pasting"


namespace AnyCopy
{
	class Window
	{
	public:
		bool Init();
		void Show();
		void Run();
	private:
		int GetDelayMS();
		void DisableStart(bool b);
		inline void SetStatusText(const char* text) { SetWindowTextA(m_Status, text); }
	public:
		static inline void ShowMsg(const char* msg) { MessageBoxA(0, msg, WIN_NAME, 0); };
	private:
		static LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
		static void CALLBACK TimerDelayCB(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
		static void CALLBACK TimerPasteCB(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	private:
		HWND m_WinHandle = 0;
		HWND m_Tbox = 0;
		HWND m_Status = 0;
		HWND m_BtnStart = 0;
		HWND m_BtnCancel = 0;
		HWND m_TboxSecs = 0;
		HWND m_TboxSecsCtrl = 0;
		HWND m_TboxSecsText = 0;
		HWND m_TboxSecsDesc = 0;
		HFONT m_Font = 0;

		bool m_bStarted = false;
		int m_StartDelayLeft = 0;
		int m_PasteIndex = 0;
		std::string m_PasteString;
	/////////////////////
	public: // SINGLETON
		static inline Window& GetWindow() { return m_sWindow; };
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
	private:
		Window() {};
		static Window m_sWindow;
	};
}
