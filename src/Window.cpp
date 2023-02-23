#include "Window.h"

AnyCopy::Window AnyCopy::Window::m_sWindow;

bool AnyCopy::Window::Init()
{
    WNDCLASSEXA wclsx = {};
    wclsx.cbSize = sizeof(wclsx);
    wclsx.hInstance = GetModuleHandle(nullptr);
    wclsx.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
    wclsx.lpszClassName = WIN_NAME;
    wclsx.style = CS_VREDRAW | CS_HREDRAW;
    wclsx.hCursor = LoadCursor(0, IDC_ARROW);
    wclsx.hIcon = wclsx.hIconSm = LoadIcon(0, IDI_APPLICATION);
    wclsx.lpfnWndProc = Window::WndProc;

    if (!RegisterClassExA(&wclsx))
        return false;

    m_WinHandle = CreateWindowExA(WS_EX_TOPMOST, WIN_NAME, WIN_NAME, WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
        GetSystemMetrics(SM_CXSCREEN) / 2 - WIN_WIDTH / 2, GetSystemMetrics(SM_CYSCREEN) / 2 - WIN_HEIGHT / 2, WIN_WIDTH, WIN_HEIGHT,
        0, 0, wclsx.hInstance, nullptr);

    if (!m_WinHandle)
        return false;

    RECT clientRect = {};
    GetClientRect(m_WinHandle ,&clientRect);

    m_Tbox = CreateWindowExA(0, "Edit", "", 
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
        0, 0, clientRect.right, clientRect.bottom / 2, m_WinHandle, 0, wclsx.hInstance, nullptr);

    if (!m_Tbox)
        return false;

    //clientRect.bottom / 2 / 10 / 2

    m_Status = CreateWindowExA(0, "Static", STATUSTXT_READY, WS_CHILD | WS_VISIBLE | ES_CENTER,
        0, clientRect.bottom / 2 + clientRect.bottom / 2 / 10 / 2, clientRect.right, 16, m_WinHandle, 0, wclsx.hInstance, nullptr);
    
    if (!m_Status)
        return false;

    int btnsRow = clientRect.bottom / 2 + clientRect.bottom / 10 / 2 * 3;
    int btnsWidth = clientRect.right / 10 * 2;
    int btnsHeight = clientRect.bottom / 10 * 2;

    m_BtnCancel = CreateWindowExA(0, "Button", "Cancel", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
        clientRect.right / 10 , btnsRow, btnsWidth, btnsHeight ,
        m_WinHandle, reinterpret_cast<HMENU>(BTN_ID_CANCEL), wclsx.hInstance, nullptr );

    if (!m_BtnCancel)
        return false;

    EnableWindow(m_BtnCancel, false);
    
    m_BtnStart = CreateWindowExA(0, "Button", "Start", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
        clientRect.right / 10 * 4, btnsRow, btnsWidth, btnsHeight,
        m_WinHandle, reinterpret_cast<HMENU>(BTN_ID_START), wclsx.hInstance, nullptr);

    if (!m_BtnStart)
        return false;

    m_TboxSecs = CreateWindowExA(0, "Edit", "2000", WS_CHILD | WS_VISIBLE | ES_CENTER | ES_NUMBER,
        clientRect.right / 10 * 7, btnsRow, btnsWidth, btnsHeight / 2, m_WinHandle, 0, wclsx.hInstance, nullptr);

    if (!m_TboxSecs)
        return false;

    m_TboxSecsCtrl = CreateWindowExA(0, UPDOWN_CLASSA, "", WS_CHILD | WS_VISIBLE | UDS_ALIGNRIGHT | UDS_SETBUDDYINT | UDS_ARROWKEYS,
        clientRect.right / 10 * 7 + btnsWidth, btnsRow, btnsWidth / 2, btnsHeight, m_WinHandle, 0, wclsx.hInstance, nullptr);
    
    if (!m_TboxSecsCtrl)
        return false;

    SendMessage(m_TboxSecsCtrl, UDM_SETBUDDY, reinterpret_cast<WPARAM>(m_TboxSecs), 0);

    MoveWindow(m_TboxSecs, clientRect.right / 10 * 7, btnsRow, btnsWidth, btnsHeight / 2, TRUE);
    MoveWindow(m_TboxSecsCtrl, clientRect.right / 10 * 7 + btnsWidth, btnsRow, btnsWidth / 3, btnsHeight, TRUE);

    m_TboxSecsText = CreateWindowExA(0, "Static", "ms", WS_CHILD | WS_VISIBLE | SS_CENTER,
        clientRect.right / 10 * 7, btnsRow + clientRect.bottom / 10, btnsWidth, btnsHeight / 2, m_WinHandle, 0, wclsx.hInstance, 0);

    if (!m_TboxSecsText)
        return false;

    m_TboxSecsDesc = CreateWindowExA(0, "Static", "Start delay", WS_CHILD | WS_VISIBLE | SS_CENTER,
        clientRect.right / 10 * 7, btnsRow + clientRect.bottom / 10 * 2, btnsWidth, btnsHeight / 2, m_WinHandle, 0, wclsx.hInstance, 0);

    if (!m_TboxSecsDesc)
        return false;

    m_Font = CreateFontA(16,0,0,0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, "Arial");

    HWND fontControls[] = 
    { m_WinHandle,m_Tbox, m_BtnStart, m_BtnCancel, m_TboxSecs, m_TboxSecsCtrl, m_TboxSecsText,m_TboxSecsDesc };
    
    for(HWND control : fontControls)
        SendMessage(control, WM_SETFONT, reinterpret_cast<WPARAM>(m_Font), true);

    return true;
}

void AnyCopy::Window::Show()
{
    if (!m_WinHandle)
        return;

    ShowWindow(m_WinHandle, SW_SHOW);
    UpdateWindow(m_WinHandle);
}

void AnyCopy::Window::Run()
{
    MSG msg = {};

    while (GetMessageA(&msg, 0, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

int AnyCopy::Window::GetDelayMS()
{
    if (!m_WinHandle || !m_TboxSecs)
        return 0;

    int strMSlen = GetWindowTextLengthA(m_TboxSecs);

    if (!strMSlen)
        return 0;

    auto strMS = std::make_unique<char[]>(strMSlen+1); // No need to reset mem since GetWindowTextA null-terminates.

    GetWindowTextA(m_TboxSecs, strMS.get(), strMSlen + 1);

    int ms;

    try
    {
        ms = std::stoi(strMS.get());
    }
    catch (int )
    {
        ms = 0;
    }

    return ms;
}

void AnyCopy::Window::DisableStart(bool b)
{
    EnableWindow(m_BtnStart, !b);
    EnableWindow(m_BtnCancel, b);
}

LRESULT AnyCopy::Window::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_NOTIFY:
    {
        NM_UPDOWN* nmUpDown = reinterpret_cast<NM_UPDOWN*>(lparam);
        if (nmUpDown->hdr.code == UDN_DELTAPOS)
        {
            char buffer[16] = {};
            GetWindowTextA(Window::GetWindow().m_TboxSecs, buffer, 16);
            int newValue;
            try 
            {
                newValue = std::stoi(buffer) + nmUpDown->iDelta * -50;
            }
            catch (int) { return -1; }

            if (newValue < 0)
                return 1;

            SetWindowTextA(Window::GetWindow().m_TboxSecs, std::to_string(newValue).c_str());
            return 1;
        }
        break;
    }
    case WM_COMMAND:
    {
        // If a third option is added, change to a switch I guess
        // Even though MSVC will probably optimize it to the same code
        if (LOWORD(wparam) == BTN_ID_START)
        {
            
            auto& window = Window::GetWindow();

            if (!window.m_bStarted) // I think we can skip this, since it shouldn't be possible
            {
                int tboxTextLen = GetWindowTextLengthA(window.m_Tbox);
                if (!tboxTextLen)
                {
                    Window::ShowMsg("Text box is empty!");
                    break;
                }

                LANGID langID = LANGIDFROMLCID(GetKeyboardLayout(0));
                if (PRIMARYLANGID(langID) != LANG_ENGLISH)
                {
                    HKL enUSLayout = LoadKeyboardLayoutA("00000409", KLF_ACTIVATE);
                    if (!enUSLayout)
                    {
                        ShowMsg("Failed to load en-us keyboard layout");
                        break;
                    }
                }

                window.DisableStart(true);
                window.m_bStarted = true;
                window.SetStatusText(STATUSTXT_WAITING);
                window.m_StartDelayLeft = window.GetDelayMS();
                if (!window.m_PasteString.empty())
                    window.m_PasteString.clear();
                auto tboxText = std::make_unique<char[]>(tboxTextLen + 1);
                GetWindowTextA(window.m_Tbox, tboxText.get(), tboxTextLen + 1);
                window.m_PasteString = tboxText.get();
                window.m_PasteIndex = 0;
                SetTimer(window.m_WinHandle, TIMER_ID_DELAY, 50 /*IMO Win32 Timer has 55ms~ accuracy(fun fact :P)*/, Window::TimerDelayCB);
            }
        }
        else if (LOWORD(wparam) == BTN_ID_CANCEL)
        {
            auto& window = Window::GetWindow();
            window.SetStatusText(STATUSTXT_READY);
            KillTimer(hwnd, TIMER_ID_DELAY);
            KillTimer(hwnd,TIMER_ID_PASTE);
            window.DisableStart(false);
            window.m_bStarted = false;
        }
        break;
    }
    case WM_DESTROY:
        if (Window::GetWindow().m_Font)
            DeleteObject(Window::GetWindow().m_Font);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcA(hwnd,msg,wparam,lparam);
    }
    return 0;
}

void CALLBACK AnyCopy::Window::TimerDelayCB(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    auto& window = Window::GetWindow();
    window.m_StartDelayLeft -= 50;
    std::ostringstream oss;
    oss << STATUSTXT_WAITING " (" << window.m_StartDelayLeft << ")";
    window.SetStatusText(oss.str().c_str());
    if (window.m_StartDelayLeft <= 0)
    {
        window.SetStatusText(STATUSTXT_PASTING);
        KillTimer(hwnd, TIMER_ID_DELAY);
        SetTimer(hwnd, TIMER_ID_PASTE, PASTE_DELAY, TimerPasteCB);
    }
}

void AnyCopy::Window::TimerPasteCB(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    auto& window = Window::GetWindow();

    if (!window.m_bStarted || window.m_PasteIndex >= window.m_PasteString.size())
    {
        KillTimer(hwnd, TIMER_ID_PASTE);
        window.m_bStarted = false;
        window.m_PasteString.clear();
        window.m_PasteIndex = 0;
        window.DisableStart(false);
        window.SetStatusText(STATUSTXT_READY);
        return;
    }

    std::ostringstream oss;
    oss << STATUSTXT_PASTING " (" << window.m_PasteIndex << "/" << window.m_PasteString.size() << ")";
    window.SetStatusText(oss.str().c_str());

    BYTE vk = 0;
    BYTE shift = 0;
    char c = window.m_PasteString.at(window.m_PasteIndex++);

    if (c == '\r')
    {
        vk = VK_RETURN;
        window.m_PasteIndex++;
    }
    else
    {
        short keyscana = VkKeyScanA(c);
        vk = LOBYTE(keyscana);
        shift = HIBYTE(keyscana);
    }


    INPUT inputShift = {};
    if (shift)
    {
        inputShift.type = INPUT_KEYBOARD;
        inputShift.ki.wVk = VK_LSHIFT;
        inputShift.ki.wScan = 42;
        inputShift.ki.dwFlags = KEYEVENTF_SCANCODE;
        SendInput(1, &inputShift, sizeof(INPUT));
    }

    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vk;
    input.ki.wScan = MapVirtualKeyA(vk, MAPVK_VK_TO_VSC);
    input.ki.dwFlags = KEYEVENTF_SCANCODE;

    SendInput(1, &input, sizeof(INPUT));

    input.ki.dwFlags |= KEYEVENTF_KEYUP;

    SendInput(1, &input, sizeof(INPUT));

    if (shift)
    {
        inputShift.ki.dwFlags |= KEYEVENTF_KEYUP;
        SendInput(1, &inputShift, sizeof(INPUT));
    }

}
