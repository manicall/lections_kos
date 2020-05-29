#include <windows.h>
#include <iostream>
using namespace std;
HHOOK hKbdHook;
HHOOK hMouseHook;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
    {
        PKBDLLHOOKSTRUCT pKey = (PKBDLLHOOKSTRUCT)lParam;
        cout << "Код клавиши: " <<  pKey->vkCode << " Символ клавиши: " << (char)pKey->vkCode << endl;
    }
    return CallNextHookEx(hKbdHook, nCode, wParam, lParam);
}
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    PMOUSEHOOKSTRUCT pMouseStruct = (PMOUSEHOOKSTRUCT)lParam;
    if (pMouseStruct != NULL)
    {
        if (wParam == WM_RBUTTONDOWN)
        {
            cout << "Right botton clicked" << endl;
        }
        if (wParam == WM_LBUTTONDOWN)
        {
            cout << "Left botton clicked" << endl;
        }
        cout << "Mouse position X = " << pMouseStruct->pt.x << " Mouse Position Y = " << pMouseStruct->pt.y << endl;
    }
    return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}
int main()
{
    setlocale(LC_ALL, "ru");
    hKbdHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, NULL);
    hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, NULL);
    MSG msg;
    GetMessage(&msg, NULL, NULL, NULL);
    UnhookWindowsHookEx(hKbdHook);
    UnhookWindowsHookEx(hMouseHook);
    return 0;
}