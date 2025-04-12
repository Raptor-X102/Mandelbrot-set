#include "Calc_func.h"

void Calc_func_intrinsics128_pd(uint32_t* pixels, Frame_movement movement) {

    #define Vector_length 2

    double dx = 2.0f / (X_Pixels_Size * movement.scale);
    double dy = 2.0f / (X_Pixels_Size * movement.scale);

    movement.y_0 = Y_0 + movement.y_offset;
    movement.x_0 = X_0 + movement.x_offset;
    __m128d max_sqr_r_vec = _mm_set_pd1(Max_sqr_r);

    PARALLEL_FOR
    for (int y_pixels = 0; y_pixels < Y_Pixels_Size; y_pixels++) {

        __m128d vec_r_t_x;
        __m128d vec_r_t_y;
        __m128d vec_r_t_sqr_x;
        __m128d vec_r_t_sqr_y;
        __m128d vec_r_t_mul_xy;
        __m128d vec_r_t_sqr_r;
        __m128d const_x;
        __m128d const_y;
        __m128d vec_status;
        alignas(16) uint64_t vec_colour_64[2] = {};
        alignas(16) uint64_t vec_status_64[2] = {};

        double current_y = movement.y_0 + y_pixels * dy;
        for (int x_pixels = 0; x_pixels < X_Pixels_Size; x_pixels += Vector_length) {

            double start_x = movement.x_0 + x_pixels * dx;

            vec_r_t_x = MM_SET_PD(start_x, dx);
            vec_r_t_y = _mm_set_pd1(current_y);
            const_x = vec_r_t_x;
            const_y = vec_r_t_y;

            vec_r_t_sqr_x  = _mm_mul_pd(vec_r_t_x, vec_r_t_x);
            vec_r_t_sqr_y  = _mm_mul_pd(vec_r_t_y, vec_r_t_y);
            vec_r_t_mul_xy = _mm_mul_pd(vec_r_t_x, vec_r_t_y);
            vec_r_t_sqr_r  = _mm_add_pd(vec_r_t_sqr_x, vec_r_t_sqr_y);

            int num_of_iter = 0;
            __m128i iterations = _mm_set_epi64x(0, 0);
            for (; num_of_iter < Max_iter; num_of_iter++) {

                vec_status = _mm_cmple_pd(vec_r_t_sqr_r, max_sqr_r_vec);
                if (!_mm_movemask_epi8(_mm_castpd_si128(vec_status)))
                    break;
                iterations = _mm_sub_epi64(iterations, _mm_castpd_si128(vec_status));

                vec_r_t_x      = _mm_add_pd(_mm_sub_pd(vec_r_t_sqr_x, vec_r_t_sqr_y), const_x);
                vec_r_t_y      = _mm_add_pd(_mm_add_pd(vec_r_t_mul_xy, vec_r_t_mul_xy), const_y);
                vec_r_t_sqr_x  = _mm_mul_pd(vec_r_t_x, vec_r_t_x);
                vec_r_t_sqr_y  = _mm_mul_pd(vec_r_t_y, vec_r_t_y);
                vec_r_t_mul_xy = _mm_mul_pd(vec_r_t_x, vec_r_t_y);
                vec_r_t_sqr_r  = _mm_add_pd(vec_r_t_sqr_x, vec_r_t_sqr_y);
            }

            _mm_store_si128((__m128i*) vec_colour_64, iterations);
            _mm_store_si128((__m128i*) vec_status_64, _mm_castpd_si128(vec_status));
            for (int i = 0; i < Vector_length; i++) {

                if (vec_status_64[i])
                    pixels[y_pixels * X_Pixels_Size + x_pixels + i] = Max_iter;

                else
                    pixels[y_pixels * X_Pixels_Size + x_pixels + i] = (vec_status_64[i] << 24) |
                                                                        (vec_status_64[i] + vec_colour_64[i] << 16) |
                                                                        (vec_status_64[i] + vec_colour_64[i] + vec_colour_64[i] << 8) |
                                                                        255;
            }
        }
    }
    #undef Vector_length
}

void Calc_func_intrinsics128_pd_void(uint32_t* pixels, Frame_movement movement) {

    #define Vector_length 4
    __asm__ volatile (
        "" : :
        "r"(pixels),
        "r"(movement.x_offset),
        "r"(movement.scale)
    );
    volatile __m128d max_sqr_r_vec = _mm_set_pd1(Max_sqr_r);

    volatile double dx = 2.0f / (X_Pixels_Size * movement.scale);
    volatile double dy = 2.0f / (X_Pixels_Size * movement.scale);

    volatile real_t tem_x0 = Y_0 + movement.y_offset;
    volatile real_t tem_y0 = X_0 + movement.x_offset;
    #undef Vector_length
}
