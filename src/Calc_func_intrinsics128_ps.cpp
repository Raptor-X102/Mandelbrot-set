#include "Calc_func.h"

void Calc_func_intrinsics128_ps(uint32_t* pixels, Frame_movement movement) {

    #define Vector_length 4

    float dx = 2.0f / (X_Pixels_Size * movement.scale);
    float dy = 2.0f / (X_Pixels_Size * movement.scale);

    movement.y_0 = Y_0 + movement.y_offset;
    movement.x_0 = X_0 + movement.x_offset;
    __m128 max_sqr_r_vec = _mm_set_ps1(Max_sqr_r);

    PARALLEL_FOR
    for (int y_pixels = 0; y_pixels < Y_Pixels_Size; y_pixels++) {

        __m128 vec_r_t_x;
        __m128 vec_r_t_y;
        __m128 vec_r_t_sqr_x;
        __m128 vec_r_t_sqr_y;
        __m128 vec_r_t_mul_xy;
        __m128 vec_r_t_sqr_r;
        __m128 const_x;
        __m128 const_y;
        __m128 vec_status;
        alignas(16) uint32_t vec_colour_32[4] = {};
        alignas(16) uint32_t vec_status_32[4] = {};

        float current_y = movement.y_0 + y_pixels * dy;
        for (int x_pixels = 0; x_pixels < X_Pixels_Size; x_pixels += Vector_length) {

            float start_x = movement.x_0 + x_pixels * dx;

            vec_r_t_x = MM_SET_PS(start_x, dx);
            vec_r_t_y = _mm_set_ps1(current_y);
            const_x = vec_r_t_x;
            const_y = vec_r_t_y;

            vec_r_t_sqr_x  = _mm_mul_ps(vec_r_t_x, vec_r_t_x);
            vec_r_t_sqr_y  = _mm_mul_ps(vec_r_t_y, vec_r_t_y);
            vec_r_t_mul_xy = _mm_mul_ps(vec_r_t_x, vec_r_t_y);
            vec_r_t_sqr_r  = _mm_add_ps(vec_r_t_sqr_x, vec_r_t_sqr_y);

            int num_of_iter = 0;
            __m128i iterations = _mm_set_epi64x(0, 0);
            for (; num_of_iter < Max_iter; num_of_iter++) {

                vec_status = _mm_cmple_ps(vec_r_t_sqr_r, max_sqr_r_vec);
                if (!_mm_movemask_epi8(_mm_castps_si128(vec_status)))
                    break;
                iterations = _mm_sub_epi32(iterations, _mm_castps_si128(vec_status));

                vec_r_t_x      = _mm_add_ps(_mm_sub_ps(vec_r_t_sqr_x, vec_r_t_sqr_y), const_x);
                vec_r_t_y      = _mm_add_ps(_mm_add_ps(vec_r_t_mul_xy, vec_r_t_mul_xy), const_y);
                vec_r_t_sqr_x  = _mm_mul_ps(vec_r_t_x, vec_r_t_x);
                vec_r_t_sqr_y  = _mm_mul_ps(vec_r_t_y, vec_r_t_y);
                vec_r_t_mul_xy = _mm_mul_ps(vec_r_t_x, vec_r_t_y);
                vec_r_t_sqr_r  = _mm_add_ps(vec_r_t_sqr_x, vec_r_t_sqr_y);
            }

            _mm_store_si128((__m128i*) vec_colour_32, iterations);
            _mm_store_si128((__m128i*) vec_status_32, _mm_castps_si128(vec_status));
            for (int i = 0; i < Vector_length; i++) {

                if (vec_status_32[i])
                    pixels[y_pixels * X_Pixels_Size + x_pixels + i] = Max_iter;

                else
                    pixels[y_pixels * X_Pixels_Size + x_pixels + i] = (vec_status_32[i] << 24) |
                                                                        (vec_status_32[i] + vec_colour_32[i] << 16) |
                                                                        (vec_status_32[i] + vec_colour_32[i] + vec_colour_32[i] << 8) |
                                                                        255;
            }
        }
    }
    #undef Vector_length
}

void Calc_func_intrinsics128_ps_void(uint32_t* pixels, Frame_movement movement) {

    #define Vector_length 4
    __asm__ volatile (
        "" : :
        "r"(pixels),
        "r"(movement.x_offset),
        "r"(movement.scale)
    );
    volatile __m128 max_sqr_r_vec = _mm_set_ps1(Max_sqr_r);

    volatile float dx = 2.0f / (X_Pixels_Size * movement.scale);
    volatile float dy = 2.0f / (X_Pixels_Size * movement.scale);

    volatile real_t tem_x0 = Y_0 + movement.y_offset;
    volatile real_t tem_y0 = X_0 + movement.x_offset;
    #undef Vector_length
}
