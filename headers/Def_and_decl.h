#ifndef DEF_AND_DECL
#define DEF_AND_DECL
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <winternl.h>
#include"Rdtscp.h"
#include <xmmintrin.h>
#include <immintrin.h>
extern "C" {
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
}
#include "Debug_printf.h"
#include"Command_line_parser.h"
struct Button {
    SDL_Rect rect;
    const char* label;
    SDL_Texture* textTexture;
    int text_width, text_height;
    bool is_clicked;
};

struct AppState {
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    Button button;
    bool running;
};

#ifdef PARALLEL_ENABLED
    #include <omp.h>
    #define PARALLEL_FOR _Pragma("omp parallel for schedule(dynamic, 2)")
#else
    #define PARALLEL_FOR
#endif

#ifdef DOUBLE
typedef double real_t;
#else
typedef float real_t;
#endif

struct Key_status {

    bool moving_up    = false,
         moving_down  = false,
         moving_left  = false,
         moving_right = false,
         scale_up     = false,
         scale_down   = false;
};

struct Frame_movement {

    double scale;
    double x_offset;
    double y_offset;
    double x_0;
    double y_0;
};

struct Test_data {

    clock_t timeh_t1;
    clock_t timeh_t2;
    uint64_t start_cycles;
    uint64_t end_cycles;
    double frequency_start;
    double frequency_end;
    int test_num_of_cycles;
};

#define SystemProcessorPerformanceInformation 8

#ifdef PARALLEL_ENABLED
#define Test_num_of_cycles 5000
#define Test_num_of_cycles_basic 1000
#else
#define Test_num_of_cycles 1000
#define Test_num_of_cycles_basic 100
#endif

const int Init_num_of_cycles = 1;
const int Test_num_of_cycles_void = 1000;

const int X_Pixels_Size = 1920;
const int Y_Pixels_Size = 1080;
const double Init_dx = 2.0f / X_Pixels_Size;
const double Init_dy = 2.0f / X_Pixels_Size;
const double X_c = -1/sqrt(2)-0.04;
const double X_0 = -((float)X_Pixels_Size)*Init_dx/2+X_c;
const double Y_0 =  (X_0-X_c) + Y_Pixels_Size/X_Pixels_Size+0.25;
const double scale_speed = 0.01f;
const double init_move_speed = 0.02f;
const uint32_t Max_iter = 256;
const  double Max_sqr_r = 10;
typedef void (*Calc_func_t)(uint32_t* pixels, Frame_movement movement);


#include"Graph_mode_loop.h"
#include"Calc_func.h"

#endif
