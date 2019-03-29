#include <SDL2/SDL.h>
#include "nc1020.h"
#include <iostream>

#define FRAME_RATE 30
#define FRAME_INTERVAL (1000/FRAME_RATE)

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 80
#define LINE_SIZE 2

SDL_Window* window;
SDL_Renderer* renderer;

static uint8_t lcd_buf[SCREEN_WIDTH * SCREEN_HEIGHT / 8];

bool InitEverything();
bool InitSDL();
bool CreateWindow();
bool CreateRenderer();
void SetupRenderer();

void Render();
void RunGame();


int main( int argc, char* args[] ) {
  if (!InitEverything())
    return -1;

  RunGame();
  wqx::SaveNC1020();
}

void Render() {
  // Clear the window and make it all green
  SDL_RenderClear( renderer );

  // Change color to blue
  SDL_SetRenderDrawColor( renderer, 0, 0, 255, 255 );
  SDL_Rect position;

  for (int i = 0; i < SCREEN_WIDTH; ++i) {
    for (int j = 0; j < SCREEN_HEIGHT ; ++j) {
      size_t bit = SCREEN_WIDTH * j + i;
      bool pixel = (lcd_buf[bit / 8] & (1 << (7 - bit % 8))) != 0;
      if (pixel) {
        // Render the pixel
        for (int u = 0; u < LINE_SIZE; ++u) {
          for (int v = 0; v < LINE_SIZE; ++v) {
            SDL_RenderDrawPoint(renderer, i * LINE_SIZE + u, j * LINE_SIZE + v);
          }
        }
      }
    }
  }
  
  // Change color to green
  SDL_SetRenderDrawColor( renderer, 0, 255, 0, 255 );

  // Render the changes above
  SDL_RenderPresent(renderer);
}

bool InitEverything()
{
  if (!InitSDL())
    return false;

  if (!CreateWindow())
    return false;

  if (!CreateRenderer())
    return false;

  SetupRenderer();

  wqx::WqxRom rom = {
    .romPath = "./obj_lu.bin",
    .norFlashPath = "./nc1020.fls",
    .statesPath = "./nc1020.sts",
  };
  wqx::Initialize(rom);
  wqx::LoadNC1020();
  
  return true;
}

bool InitSDL() {
  if (SDL_Init( SDL_INIT_EVERYTHING ) == -1) {
    std::cout << " Failed to initialize SDL : " << SDL_GetError() << std::endl;
    return false;
  }
  return true;
}

bool CreateWindow() {
  window = SDL_CreateWindow("WQX", 0, 0, LINE_SIZE * SCREEN_WIDTH, LINE_SIZE * SCREEN_HEIGHT, 0);
  if ( window == nullptr ) {
    std::cout << "Failed to create window : " << SDL_GetError() << std::endl;
    return false;
  }
  return true;
}

bool CreateRenderer() {
  renderer = SDL_CreateRenderer( window, -1, 0 );

  if ( renderer == nullptr ) {
    std::cout << "Failed to create renderer : " << SDL_GetError() << std::endl;
    return false;
  }

  return true;
}

void SetupRenderer() {
  // Set size of renderer to the same as window
  SDL_RenderSetLogicalSize( renderer, SCREEN_WIDTH * LINE_SIZE, SCREEN_HEIGHT * LINE_SIZE);

  // Set color of renderer to green
  SDL_SetRenderDrawColor( renderer, 0, 255, 0, 255 );
}

void RunGame() {
  bool loop = true;

  while (loop) {
    wqx::RunTimeSlice(FRAME_INTERVAL, false);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if ( event.type == SDL_QUIT ) {
        loop = false;
      } else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        bool key_down = event.type == SDL_KEYDOWN;
        switch ( event.key.keysym.sym ) {
          case SDLK_RIGHT: // Right
            wqx::SetKey(0x1F, key_down);
            break;
          case SDLK_LEFT:  // Left
            wqx::SetKey(0x3F, key_down);
            break;
          case SDLK_DOWN:  // Down
            wqx::SetKey(0x1B, key_down);
            break;
          case SDLK_UP:  // Up
            wqx::SetKey(0x1A, key_down);
            break;
          case SDLK_u:  // X
            wqx::SetKey(0x10, key_down); // F1 - fly
            break;
          case SDLK_i:  // Y
            wqx::SetKey(0x13, key_down); // F4 - menu
            break;
          case SDLK_j:  // A
            wqx::SetKey(0x1D, key_down); // enter
            break;
          case SDLK_k:  // B
            wqx::SetKey(0x3B, key_down); // ESC - back
            break;
          case SDLK_KP_ENTER:  // START
            wqx::SetKey(0x08, key_down); // F10 - main screen
            break;
          case SDLK_SPACE:  // SELECT
            wqx::SetKey(0x0E, key_down); // F11 - game menu
            break;
          case SDLK_y:  // Shift-X
            wqx::SetKey(0x31, key_down); // X
            break;
          case SDLK_0:  // Shift-Y
            wqx::SetKey(0x25, key_down); // Y
            break;
          case SDLK_h:  // Shift-A
            wqx::SetKey(0x37, key_down); // Page Up
            break;
          case SDLK_l:  // Shift-B
            wqx::SetKey(0x1E, key_down); // Page Down
            break;
          case SDLK_ESCAPE:  // Menu
            wqx::SetKey(0x0F, key_down); // Power
            break;
          case SDLK_BACKSPACE:  // Shift-Menu
            loop = false;
            break;
          default :  // unsupported
            break;
        }
      }
    }
    if (!wqx::CopyLcdBuffer(lcd_buf)) {
      std::cout << "Failed to copy buffer renderer." << std::endl;
    }
    Render();
    SDL_Delay(FRAME_INTERVAL);
  }
}
