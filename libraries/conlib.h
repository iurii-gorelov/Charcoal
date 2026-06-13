#pragma once
#include <vector>
#include <string>

namespace ConLib {

    
    bool Initialize();

    void Destroy();


  
    void Put(
    unsigned char c, int x, int y,
    unsigned char fg = 255, unsigned char bg = 255);

    unsigned short& AttrAt(int x, int y);

    void Fill(
    unsigned char c,
    int x, int y, int w, int h,
    unsigned char fg = 255, unsigned char bg = 255);

    void Write(
    std::string text, int x, int y,
    int maxw = 999);

    void ApplyBuffer(void);


  
    void Title(const char*);

    bool FullScreen(void);
  void FullScreen(bool);

    void NoResize(void);

    void NoScrollBar(void);

    void NoSelect(void);

    void Resize(int width, int height);

    int Width(void);
  int Height(void);


      bool Pressed(int key);

    template <int key>
  bool JustPressed() {
    static bool pressed = false;
    if (Pressed(key)) {
      if (!pressed)
        return pressed = true;
    }
    else
      pressed = false;
    return false;
  }

    const int KEY_ESC = 0x1B;
  const int KEY_ENTER = 0x0D;
  const int KEY_LEFT = 0x25;
  const int KEY_RIGHT = 0x27;
  const int KEY_UP = 0x26;
  const int KEY_DOWN = 0x28;
  const int KEY_BACKSPACE = 0x08;
  const int KEY_TAB = 0x09;


      void CursorPos(int x, int y);
  
    void CursorMode(char mode);

    const char CM_HIDDEN = 0;
  const char CM_UNDERSCORE = 1;
  const char CM_BLOCK = 2;
  

      void ColorTable(int table[16]);


      void SetupFont(const char* name, int size);

    void InstallFont(const char* path);


      void Sleep(int ms);
};

namespace cl = ConLib;