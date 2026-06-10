// add the windows header if it's windows
#include <conlib.h>
#include <windows.h>
#include <shlobj.h>
#include <string>
#include <vector>

// handle to the console
static HANDLE ohandle = 0;
static HANDLE ihandle = 0;
static HWND   window  = 0;
static bool   noScrollBar = false;
static bool   isFullScreen = false;

// buffer for the console
COORD      bufsize = {0, 0};
CHAR_INFO* buffer  = nullptr;
CHAR_INFO trashbox;

// input buffer
std::vector<std::string> inputbuf;

// local functions
static void ActualizeBufferSize(void);

// prepare the console
bool ConLib::Initialize(void)
{
  // get console handle
  ohandle = GetStdHandle(STD_OUTPUT_HANDLE);
  ihandle = GetStdHandle(STD_INPUT_HANDLE);
  window = GetConsoleWindow();
  ActualizeBufferSize();

  // TODO: return real value
  return true;
}

// destroy console
void ConLib::Destroy()
{
  // delete buffers
  delete[] buffer;
  
  // return quick edit mode
  DWORD mode;
  GetConsoleMode(ihandle, &mode);
  mode |= ENABLE_QUICK_EDIT_MODE;
  SetConsoleMode(ihandle, mode);
}

// set the title for the console
void ConLib::Title(const char* title) {
  SetConsoleTitleA(title);
}

// return current fullscreen state
bool ConLib::FullScreen(void) {
  return isFullScreen;
}

// make the console fullscreen
void ConLib::FullScreen(bool enable)
{
  // to save the window style
  static WINDOWPLACEMENT wp { sizeof(WINDOWPLACEMENT) };
  static LONG_PTR style = 0;
  static RECT windowRect { 0 };

  // enable the fullscreen mode
  if (enable && !isFullScreen)
  {
    // save current window style
    style = GetWindowLongPtr(window, GWL_STYLE);
    GetWindowRect(window, &windowRect);
    GetWindowPlacement(window, &wp);

    // getting system metrics
    int screenWidth  = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // removing window fancy styling
    SetWindowLongPtr(window, GWL_STYLE, WS_VISIBLE | WS_POPUP);

    // set the window to be on top of all windows
    SetWindowPos(
      window,
      HWND_TOPMOST,
      0, 0, 
      screenWidth, screenHeight,
      SWP_FRAMECHANGED | SWP_SHOWWINDOW);

    // pushing window to show as maximized
    ShowWindow(window, SW_SHOWMAXIMIZED);
    
    // setting current state
    isFullScreen = true;
  }

  // disable the fullscreen mode
  else if (!enable && isFullScreen)
  {
    // original window style
    SetWindowLongPtr(window, GWL_STYLE, style);
    
    // original position
    SetWindowPos(
      window, NULL, 
      windowRect.left, windowRect.top,
      windowRect.right - windowRect.left,
      windowRect.bottom - windowRect.top,
      SWP_FRAMECHANGED);
    
    // original window placement
    SetWindowPlacement(window, &wp);
    
    // setting current state
    isFullScreen = false;
  }

  // actualize buffer size
  ActualizeBufferSize();
}

// disable console resizing
void ConLib::NoResize(void)
{
  // getting the styles
  LONG_PTR style = GetWindowLongPtr(window, GWL_STYLE);

  // no resize properties
  style &= ~WS_THICKFRAME;
  style &= ~WS_MAXIMIZEBOX;

  // set the new styles
  SetWindowLongPtr(window, GWL_STYLE, style);
}

// disable console scroll bar
void ConLib::NoScrollBar(void)
{
  // changing the style
  LONG style = GetWindowLong(window, GWL_STYLE);
  style &= ~(WS_VSCROLL | WS_HSCROLL);
  SetWindowLong(window, GWL_STYLE, style);

  // updating the window
  SetWindowPos(window, NULL, 0, 0, 0, 0, 
    SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

  // set the flag
  noScrollBar = true;
}

// disables mouse selection
void ConLib::NoSelect(void)
{
  // getting the mode
  DWORD mode;
  GetConsoleMode(ihandle, &mode);
  
  // disable the quick edit mode
  mode &= ~ENABLE_QUICK_EDIT_MODE;
  mode |= ENABLE_EXTENDED_FLAGS;
  
  // set the mode
  SetConsoleMode(ihandle, mode);
}

// set up current font for the console
void ConLib::SetupFont(const char* name, int size)
{
  // defining the properties
  CONSOLE_FONT_INFOEX cfi = {0};
  cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
  cfi.nFont = 0;
  cfi.dwFontSize.X = 0;
  cfi.dwFontSize.Y = size;
  cfi.FontFamily = 54;
  cfi.FontWeight = 400;
  
  // damn those who did this to widechars
  std::string n(name);
  std::wcscpy(cfi.FaceName, std::wstring(n.begin(), n.end()).c_str());
  // int len = MultiByteToWideChar(CP_UTF8, 0, name, -1, NULL, 0);
  // std::wstring wname(len, 0);
  // MultiByteToWideChar(CP_UTF8, 0, name, -1, &wname[0], len);
  // std::wcscpy(cfi.FaceName, wname.c_str());

  // set the font
  SetCurrentConsoleFontEx(ohandle, false, &cfi);

  // actualize buffer size
  ActualizeBufferSize();
}

// resize the console
void ConLib::Resize(int width, int height)
{    
  // set the new size
  SMALL_RECT sr;
  sr.Left = 0;
  sr.Top = 0;
  sr.Right = width - 1;
  sr.Bottom = height - 1;
  SetConsoleWindowInfo(ohandle, TRUE, &sr);
  
  // update the buffer
  ActualizeBufferSize();
}

// get width of the console
int ConLib::Width(void) {
  return bufsize.X;
}

// get height of the console
int ConLib::Height(void) {
  return bufsize.Y;
}

// install user font from file locally
void ConLib::InstallFont(const char* path)
{
  // get filename only with extension, without path
  std::string name = path;
  size_t pos = name.find_last_of("\\/");
  if (pos != std::string::npos)
    name = name.substr(pos + 1);
  pos = name.find_last_of(".");
  if (pos != std::string::npos)
    name = name.substr(0, pos);
  
  // get local appdata path
  char localAppData[MAX_PATH];
  SHGetFolderPathA(0, CSIDL_LOCAL_APPDATA, 0, 0, localAppData);
  
  // create destination path
  std::string dest =
    std::string(localAppData) +
    "\\Microsoft\\Windows\\Fonts\\" + 
    name + ".ttf";
  
  // create folder and copy file
  CreateDirectoryA(
    (std::string(localAppData) + "\\Microsoft\\Windows\\Fonts").c_str(), 0);
  CopyFileA(path, dest.c_str(), false);
  
  // register font
  AddFontResourceA(dest.c_str());
  
  // add to registry
  HKEY hKey;
  RegCreateKeyExA(
    HKEY_LOCAL_MACHINE,
    "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts", 
    0, NULL, 0, KEY_SET_VALUE, NULL, &hKey, NULL);
  
  // set the value and close the key
  RegSetValueExA(
    hKey, name.c_str(), 0,
    REG_SZ, (BYTE*)dest.c_str(),
    dest.length() + 1);
  RegCloseKey(hKey);
}

// put on buffer
void ConLib::Put(unsigned char c, int x, int y, unsigned char fg, unsigned char bg)
{
  // check the bounds and put the character
  if (x < 0 || x >= bufsize.X || y < 0 || y >= bufsize.Y)
    return;
  buffer[y * bufsize.X + x].Char.AsciiChar = c;

  // set the attributes (don't change if possible)
  WORD& attr = buffer[y * bufsize.X + x].Attributes;
  if (fg < 16)
    attr = attr & 0xf0 | fg;
  if (bg < 16)
    attr = attr & 0x0f | (bg << 4);
}

// put attr
unsigned short& ConLib::AttrAt(int x, int y) {
  if (x < 0 || x >= bufsize.X || y < 0 || y >= bufsize.Y)
    return trashbox.Attributes;
  return buffer[y * bufsize.X + x].Attributes;
}

// applies buffer to the console
void ConLib::ApplyBuffer(void) {
  COORD zero = {0, 0};
  SMALL_RECT writeRegion = {0, 0, bufsize.X - 1, bufsize.Y - 1};
  WriteConsoleOutputA(ohandle, buffer, bufsize, zero, &writeRegion);
}

// set cursor position
void ConLib::CursorPos(int x, int y) {
  COORD pos = {x, y};
  SetConsoleCursorPosition(ohandle, pos);
}

// set cursor mode
void ConLib::CursorMode(char mode) {
  CONSOLE_CURSOR_INFO cci;
  cci.bVisible = !!mode;
  cci.dwSize = mode == 1 ? 10 : 100;
  SetConsoleCursorInfo(ohandle, &cci);
}

// set color table
void ConLib::ColorTable(int table[16])
{
  // getting buffer info
  CONSOLE_SCREEN_BUFFER_INFOEX csbi = { sizeof(CONSOLE_SCREEN_BUFFER_INFOEX) };
  GetConsoleScreenBufferInfoEx(ohandle, &csbi);

  // setting color table
  for (int i = 0; i < 16; i++) {
    int b = (table[i] & 0xFF0000) >> 16;
    int g = (table[i] & 0x00FF00);
    int r = (table[i] & 0x0000FF) << 16;
    csbi.ColorTable[i] = b | g | r;
  }

  // writing buffer info
  SetConsoleScreenBufferInfoEx(ohandle, &csbi);

  // keep no scroll bar
  if (noScrollBar)
    ConLib::NoScrollBar();
}

// should be called when the console is resized
static void ActualizeBufferSize(void)
{   
  // getting buffer info
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(ohandle, &csbi);
    
  // getting window size in characters
  bufsize.X = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  bufsize.Y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

  // update buffer allocation
  delete[] buffer;
  buffer = new CHAR_INFO[bufsize.X * bufsize.Y] {};
  
  // set the buffer size
  SetConsoleScreenBufferSize(ohandle, bufsize);
}

// sleep for a certain amount of time
void ConLib::Sleep(int ms) {
  ::Sleep(ms);
}

// get pressed key
bool ConLib::Pressed(int key) {
  return GetAsyncKeyState(key) < 0;
}

// write a text to the console (colored)
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

// call put on an area
void ConLib::Fill(unsigned char c, int x, int y, int w, int h, unsigned char fg, unsigned char bg) {
  for (int i = 0; i < w; i++)
    for (int j = 0; j < h; j++)
      ConLib::Put(c, x + i, y + j, fg, bg);
}