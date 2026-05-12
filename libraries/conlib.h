#pragma once
#include <vector>

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
    int x, int y, unsigned char c,
    unsigned char fg, unsigned char bg);

  // update buffer to screen
  void ApplyBuffer(void);


  // WINDOW FUNCTIONS

  // function to set the console title
  void Title(const char*);

  // function to make the console fullscreen
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
  // gets all keyboard input that appeared
  // std::vector<std::string> InputBuffer(void);

  // removes all input from the buffer
  // should be called after each input processing
  // void ClearInput(void);


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
};