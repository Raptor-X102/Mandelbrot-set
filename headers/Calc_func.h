#ifndef ENCOUNTER_FUNC
#define ENCOUNTER_FUNC

#include"Def_and_decl.h"
#include"Intrinsics_defines.h"
#include <omp.h>
void Calc_func_basic(uint32_t* pixels, Frame_movement movement);
void Calc_func_basic_void(uint32_t* pixels, Frame_movement movement);
void Calc_func_arrays(uint32_t* pixels, Frame_movement movement);
void Calc_func_arrays_void(uint32_t* pixels, Frame_movement movement);
void Calc_func_intrinsics128_ps(uint32_t* pixels, Frame_movement movement);
void Calc_func_intrinsics128_ps_void(uint32_t* pixels, Frame_movement movement);
void Calc_func_intrinsics128_pd(uint32_t* pixels, Frame_movement movement);
void Calc_func_intrinsics128_pd_void(uint32_t* pixels, Frame_movement movement);
void Calc_func_intrinsics256_ps(uint32_t* pixels, Frame_movement movement);
void Calc_func_intrinsics256_ps_void(uint32_t* pixels, Frame_movement movement);
void Calc_func_intrinsics256_pd(uint32_t* pixels, Frame_movement movement);
void Calc_func_intrinsics256_pd_void(uint32_t* pixels, Frame_movement movement);


typedef __mmask8 mask_vec_t;
#define Vector_length 4
#if Vector_length == 4
    typedef uint32_t mask_t;
    const mask_t mask_array[Vector_length] = { 0xFF, 0xFF00, 0XFF0000, 0XFF000000};
#elif Vector_length == 8
    typedef uint64_t mask_t;
    const mask_t mask_array[Vector_length] = { 0xFF, 0xFF00, 0XFF0000, 0XFF000000, 0XFF00000000,
                                         0XFF0000000000, 0XFF000000000000, 0XFF00000000000000};
#else
    #error "Unsupported Vector_length"
#endif

#if defined(__M_128)
typedef __m128 reg_t;
const __mmask8 mask_vec_array[Vector_length] = { 0b11, 0b1100, 0b110000, 0b11000000};
#elif defined(__M_256)
typedef __m256 reg_t;
const __mmask8 mask_vec_array[Vector_length] = { 0b1, 0b10, 0b100, 0b1000, 0b10000, 0b100000,
                                                 0b1000000, 0b10000000};
#elif defined(INTRINSICS)
#error "No valid SIMD type defined"

#else
typedef __m128 reg_t;
#endif

#endif
