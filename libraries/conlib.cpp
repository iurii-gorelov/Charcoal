#include <conlib.h>
#include <windows.h>
#include <shlobj.h>
#include <string>
#include <vector>

static HANDLE ohandle = 0;
static HANDLE ihandle = 0;
static HWND   window  = 0;
static bool   noScrollBar = false;
static bool   isFullScreen = false;

COORD      bufsize = {0, 0};
CHAR_INFO* buffer  = nullptr;
CHAR_INFO trashbox;

std::vector<std::string> inputbuf;

static void ActualizeBufferSize(void);

bool ConLib::Initialize(void)
{
    ohandle = GetStdHandle(STD_OUTPUT_HANDLE);
  ihandle = GetStdHandle(STD_INPUT_HANDLE);
  window = GetConsoleWindow();
  ActualizeBufferSize();

    return true;
}

void ConLib::Destroy()
{
    delete[] buffer;
  
    DWORD mode;
  GetConsoleMode(ihandle, &mode);
  mode |= ENABLE_QUICK_EDIT_MODE;
  SetConsoleMode(ihandle, mode);
}

void ConLib::Title(const char* title) {
  SetConsoleTitleA(title);
}

bool ConLib::FullScreen(void) {
  return isFullScreen;
}

void ConLib::FullScreen(bool enable)
{
    static WINDOWPLACEMENT wp { sizeof(WINDOWPLACEMENT) };
  static LONG_PTR style = 0;
  static RECT windowRect { 0 };

    if (enable && !isFullScreen)
  {
        style = GetWindowLongPtr(window, GWL_STYLE);
    GetWindowRect(window, &windowRect);
    GetWindowPlacement(window, &wp);

        int screenWidth  = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

        SetWindowLongPtr(window, GWL_STYLE, WS_VISIBLE | WS_POPUP);

        SetWindowPos(
      window,
      HWND_TOPMOST,
      0, 0, 
      screenWidth, screenHeight,
      SWP_FRAMECHANGED | SWP_SHOWWINDOW);

        ShowWindow(window, SW_SHOWMAXIMIZED);
    
        isFullScreen = true;
  }

    else if (!enable && isFullScreen)
  {
        SetWindowLongPtr(window, GWL_STYLE, style);
    
        SetWindowPos(
      window, NULL, 
      windowRect.left, windowRect.top,
      windowRect.right - windowRect.left,
      windowRect.bottom - windowRect.top,
      SWP_FRAMECHANGED);
    
        SetWindowPlacement(window, &wp);
    
        isFullScreen = false;
  }

    ActualizeBufferSize();
}

void ConLib::NoResize(void)
{
    LONG_PTR style = GetWindowLongPtr(window, GWL_STYLE);

    style &= ~WS_THICKFRAME;
  style &= ~WS_MAXIMIZEBOX;

    SetWindowLongPtr(window, GWL_STYLE, style);
}

void ConLib::NoScrollBar(void)
{
    LONG style = GetWindowLong(window, GWL_STYLE);
  style &= ~(WS_VSCROLL | WS_HSCROLL);
  SetWindowLong(window, GWL_STYLE, style);

    SetWindowPos(window, NULL, 0, 0, 0, 0, 
    SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

    noScrollBar = true;
}

void ConLib::NoSelect(void)
{
    DWORD mode;
  GetConsoleMode(ihandle, &mode);
  
    mode &= ~ENABLE_QUICK_EDIT_MODE;
  mode |= ENABLE_EXTENDED_FLAGS;
  
    SetConsoleMode(ihandle, mode);
}

void ConLib::SetupFont(const char* name, int size)
{
    CONSOLE_FONT_INFOEX cfi = {0};
  cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
  cfi.nFont = 0;
  cfi.dwFontSize.X = 0;
  cfi.dwFontSize.Y = size;
  cfi.FontFamily = 54;
  cfi.FontWeight = 400;
  
    std::string n(name);
  std::wcscpy(cfi.FaceName, std::wstring(n.begin(), n.end()).c_str());
        
    SetCurrentConsoleFontEx(ohandle, false, &cfi);

    ActualizeBufferSize();
}

void ConLib::Resize(int width, int height)
{    
    SMALL_RECT sr;
  sr.Left = 0;
  sr.Top = 0;
  sr.Right = width - 1;
  sr.Bottom = height - 1;
  SetConsoleWindowInfo(ohandle, TRUE, &sr);
  
    ActualizeBufferSize();
}

int ConLib::Width(void) {
  return bufsize.X;
}

int ConLib::Height(void) {
  return bufsize.Y;
}

void ConLib::InstallFont(const char* path)
{
    std::string name = path;
  size_t pos = name.find_last_of("\\/");
  if (pos != std::string::npos)
    name = name.substr(pos + 1);
  pos = name.find_last_of(".");
  if (pos != std::string::npos)
    name = name.substr(0, pos);
  
    char localAppData[MAX_PATH];
  SHGetFolderPathA(0, CSIDL_LOCAL_APPDATA, 0, 0, localAppData);
  
    std::string dest =
    std::string(localAppData) +
    "\\Microsoft\\Windows\\Fonts\\" + 
    name + ".ttf";
  
    CreateDirectoryA(
    (std::string(localAppData) + "\\Microsoft\\Windows\\Fonts").c_str(), 0);
  CopyFileA(path, dest.c_str(), false);
  
    AddFontResourceA(dest.c_str());
  
    HKEY hKey;
  RegCreateKeyExA(
    HKEY_LOCAL_MACHINE,
    "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts", 
    0, NULL, 0, KEY_SET_VALUE, NULL, &hKey, NULL);
  
    RegSetValueExA(
    hKey, name.c_str(), 0,
    REG_SZ, (BYTE*)dest.c_str(),
    dest.length() + 1);
  RegCloseKey(hKey);
}

void ConLib::Put(unsigned char c, int x, int y, unsigned char fg, unsigned char bg)
{
    if (x < 0 || x >= bufsize.X || y < 0 || y >= bufsize.Y)
    return;
  buffer[y * bufsize.X + x].Char.AsciiChar = c;

    WORD& attr = buffer[y * bufsize.X + x].Attributes;
  if (fg < 16)
    attr = attr & 0xf0 | fg;
  if (bg < 16)
    attr = attr & 0x0f | (bg << 4);
}

unsigned short& ConLib::AttrAt(int x, int y) {
  if (x < 0 || x >= bufsize.X || y < 0 || y >= bufsize.Y)
    return trashbox.Attributes;
  return buffer[y * bufsize.X + x].Attributes;
}

void ConLib::ApplyBuffer(void) {
  COORD zero = {0, 0};
  SMALL_RECT writeRegion = {0, 0, bufsize.X - 1, bufsize.Y - 1};
  WriteConsoleOutputA(ohandle, buffer, bufsize, zero, &writeRegion);
}

void ConLib::CursorPos(int x, int y) {
  COORD pos = {x, y};
  SetConsoleCursorPosition(ohandle, pos);
}

void ConLib::CursorMode(char mode) {
  CONSOLE_CURSOR_INFO cci;
  cci.bVisible = !!mode;
  cci.dwSize = mode == 1 ? 10 : 100;
  SetConsoleCursorInfo(ohandle, &cci);
}

void ConLib::ColorTable(int table[16])
{
    CONSOLE_SCREEN_BUFFER_INFOEX csbi = { sizeof(CONSOLE_SCREEN_BUFFER_INFOEX) };
  GetConsoleScreenBufferInfoEx(ohandle, &csbi);

    for (int i = 0; i < 16; i++) {
    int b = (table[i] & 0xFF0000) >> 16;
    int g = (table[i] & 0x00FF00);
    int r = (table[i] & 0x0000FF) << 16;
    csbi.ColorTable[i] = b | g | r;
  }

    SetConsoleScreenBufferInfoEx(ohandle, &csbi);

    if (noScrollBar)
    ConLib::NoScrollBar();
}

static void ActualizeBufferSize(void)
{   
    CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(ohandle, &csbi);
    
    bufsize.X = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  bufsize.Y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    delete[] buffer;
  buffer = new CHAR_INFO[bufsize.X * bufsize.Y] {};
  
    SetConsoleScreenBufferSize(ohandle, bufsize);
}

void ConLib::Sleep(int ms) {
  ::Sleep(ms);
}

bool ConLib::Pressed(int key) {
  return GetAsyncKeyState(key) < 0;
}

void ConLib::Write(std::string text, int x, int y, int maxw) {
  int fg = 0x07, bg = 0x00;
  for (int i = 0, di = 0; i < text.length(); i++) {
    if (text[i] == '\f')
      i++, fg = text[i] >= '0' && text[i] <= '9' ? text[i] - '0' : text[i] - 'W';
    else if (text[i] == '\b')
      i++, bg = text[i] >= '0' && text[i] <= '9' ? text[i] - '0' : text[i] - 'W';
    else
      ConLib::Put(text[i], x + (di % maxw), y + (di / maxw), fg, bg), di++;
  }
}

void ConLib::Fill(unsigned char c, int x, int y, int w, int h, unsigned char fg, unsigned char bg) {
  for (int i = 0; i < w; i++)
    for (int j = 0; j < h; j++)
      ConLib::Put(c, x + i, y + j, fg, bg);
}