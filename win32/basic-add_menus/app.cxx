#define UNICODE
#define _UNICODE

#include <windows.h>
#include <windowsx.h>


void add_menus(HWND hwnd) {
  HMENU parrentMenus = CreateMenu();
  SetMenu(hwnd, parrentMenus);
}

LRESULT CALLBACK w_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_CREATE: add_menus(hwnd); return 0;
    case WM_DESTROY: PostQuitMessage(0); return 0;
    default: return DefWindowProcW(hwnd, uMsg, wParam, lParam);
  }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
  const wchar_t CLASS_NAME[] = L"MyWindowClass";

  WNDCLASS wc = {};
  wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.lpfnWndProc = w_proc;
  wc.hInstance = hInstance;
  wc.lpszClassName = CLASS_NAME;

  RegisterClass(&wc);

  HWND hwnd = CreateWindowExW(0, CLASS_NAME, L"WIN32 menu section!", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                              CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

  if (!hwnd) return 0;

  ShowWindow(hwnd, nCmdShow);

  MSG msg = {};
  while (GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}