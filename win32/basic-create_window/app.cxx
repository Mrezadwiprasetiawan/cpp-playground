#define UNICODE
#define _UNICODE
#include <windows.h>
#include <windowsx.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    default: return DefWindowProc(hwnd, uMsg, wParam, lParam);
    case WM_DESTROY: PostQuitMessage(0); return 0;
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps);
      RECT rect;
      GetClientRect(hwnd, &rect);  // ambil ukuran area dalam window (tanpa border/titlebar)
      HBRUSH blackBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
      FillRect(hdc, &rect, blackBrush);
      const wchar_t msg[] = L"Hello World!";
      SetBkColor(hdc, RGB(0, 0, 0));
      SetTextColor(hdc, RGB(255, 255, 255));
      TextOut(hdc, 50, 50, msg, lstrlen(msg));

      EndPaint(hwnd, &ps);
      return 0;
    }
  }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
  const wchar_t CLASS_NAME[] = L"MyWindowClass";

  WNDCLASS wc = {};
  wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.lpfnWndProc = WindowProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = CLASS_NAME;

  RegisterClass(&wc);

  HWND hwnd = CreateWindowExW(0, CLASS_NAME, L"Hello, Win32!", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                              NULL, NULL, hInstance, NULL);

  if (!hwnd) return 0;

  ShowWindow(hwnd, nCmdShow);

  MSG msg = {};
  while (GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}
