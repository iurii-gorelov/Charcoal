#pragma once
#include <vector>
#include <string>

// class that contains all the functions
// for the console management
namespace ConLib {

  // BASIC FUNCTIONS
  
  // function that prepares the console, it gets all required stuff
  bool Initialize();

  // function that frees the console data
  void Destroy();


  // GRAPHICS MODE GAMARJOBA

  // put a character on buffer
  void Put(
    unsigned char c, int x, int y,
    unsigned char fg = 255, unsigned char bg = 255);

  // attributes at position
  unsigned short& AttrAt(int x, int y);

  // call put on an area
  void Fill(
    unsigned char c,
    int x, int y, int w, int h,
    unsigned char fg = 255, unsigned char bg = 255);

  // write a text to console
  void Write(
    std::string text, int x, int y,
    int maxw = 999);

  // update buffer to screen
  void ApplyBuffer(void);


  // WINDOW FUNCTIONS

  // function to set the console title
  void Title(const char*);

  // function to make the console fullscreen
  bool FullScreen(void);
  void FullScreen(bool);

  // no resize for window
  void NoResize(void);

  // disable scroll bar
  void NoScrollBar(void);

  // disable mouse selection
  void NoSelect(void);

  // set the size of the console
  void Resize(int width, int height);

  // get width and height of the console
  int Width(void);
  int Height(void);


  // INPUT FUNCTIONS
  // checks if key is pressed
  bool Pressed(int key);

  // just pressed (compile time)
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

  // some keys
  const int KEY_ESC = 0x1B;
  const int KEY_ENTER = 0x0D;
  const int KEY_LEFT = 0x25;
  const int KEY_RIGHT = 0x27;
  const int KEY_UP = 0x26;
  const int KEY_DOWN = 0x28;


  // CURSOR FUNCTIONS
  // set cursor position
  void CursorPos(int x, int y);
  
  // set cursor visibility mode
  void CursorMode(char mode);

  // cursor visibility modes
  const char CM_HIDDEN = 0;
  const char CM_UNDERSCORE = 1;
  const char CM_BLOCK = 2;
  

  // COLOR FUNCTION
  // changes color table
  void ColorTable(int table[16]);


  // FONT FUNCTIONS
  // sets font to the console
  void SetupFont(const char* name, int size);

  // loads font on users pc to be used in the console
  void InstallFont(const char* path);


  // FRAMEWORK FUNCTIONS
  // sleep for a certain amount of time
  void Sleep(int ms);
};

// alias
namespace cl = ConLib;