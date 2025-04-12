#include "Calc_func.h"

void Calc_func_intrinsics256_pd(uint32_t* pixels, Frame_movement movement) {

    #define Vector_length 4

    double dx = 2.0f / (X_Pixels_Size * movement.scale);
    double dy = 2.0f / (X_Pixels_Size * movement.scale);

    movement.y_0 = Y_0 + movement.y_offset;
    movement.x_0 = X_0 + movement.x_offset;
    __m256d max_sqr_r_vec = _mm256_set1_pd(Max_sqr_r);

    PARALLEL_FOR
    for (int y_pixels = 0; y_pixels < Y_Pixels_Size; y_pixels++) {

        __m256d vec_r_t_x;
        __m256d vec_r_t_y;
        __m256d vec_r_t_sqr_x;
        __m256d vec_r_t_sqr_y;
        __m256d vec_r_t_mul_xy;
        __m256d vec_r_t_sqr_r;
        __m256d const_x;
        __m256d const_y;
        __m256d vec_status;
        alignas(32) uint64_t vec_colour_64[4] = {};
        alignas(32) uint64_t vec_status_64[4] = {};

        double current_y = movement.y_0 + y_pixels * dy;
        for (int x_pixels = 0; x_pixels < X_Pixels_Size; x_pixels += Vector_length) {

            double start_x = movement.x_0 + x_pixels * dx;

            vec_r_t_x = MM256_SET_PD(start_x, dx);
            vec_r_t_y = _mm256_set1_pd(current_y);
            const_x = vec_r_t_x;
            const_y = vec_r_t_y;

            vec_r_t_sqr_x  = _mm256_mul_pd(vec_r_t_x, vec_r_t_x);
            vec_r_t_sqr_y  = _mm256_mul_pd(vec_r_t_y, vec_r_t_y);
            vec_r_t_mul_xy = _mm256_mul_pd(vec_r_t_x, vec_r_t_y);
            vec_r_t_sqr_r  = _mm256_add_pd(vec_r_t_sqr_x, vec_r_t_sqr_y);

            int num_of_iter = 0;
            __m256i iterations = _mm256_set_epi64x(0, 0, 0, 0);
            for (; num_of_iter < Max_iter; num_of_iter++) {

                vec_status = _mm256_cmp_pd(vec_r_t_sqr_r, max_sqr_r_vec, _CMP_LE_OQ);
                if (!_mm256_movemask_epi8(_mm256_castpd_si256(vec_status)))
                    break;
                iterations = _mm256_sub_epi64(iterations, _mm256_castpd_si256(vec_status));

                vec_r_t_x      = _mm256_add_pd(_mm256_sub_pd(vec_r_t_sqr_x, vec_r_t_sqr_y), const_x);
                vec_r_t_y      = _mm256_add_pd(_mm256_add_pd(vec_r_t_mul_xy, vec_r_t_mul_xy), const_y);
                vec_r_t_sqr_x  = _mm256_mul_pd(vec_r_t_x, vec_r_t_x);
                vec_r_t_sqr_y  = _mm256_mul_pd(vec_r_t_y, vec_r_t_y);
                vec_r_t_mul_xy = _mm256_mul_pd(vec_r_t_x, vec_r_t_y);
                vec_r_t_sqr_r  = _mm256_add_pd(vec_r_t_sqr_x, vec_r_t_sqr_y);
            }

            _mm256_store_si256((__m256i*) vec_colour_64, iterations);
            _mm256_store_si256((__m256i*) vec_status_64, _mm256_castpd_si256(vec_status));
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

void Calc_func_intrinsics256_pd_void(uint32_t* pixels, Frame_movement movement) {

    #define Vector_length 4
    __asm__ volatile (
        "" : :
        "r"(pixels),
        "r"(movement.x_offset),
        "r"(movement.scale)
    );
    volatile __m256d max_sqr_r_vec = (volatile __m256d) _mm256_set1_ps(Max_sqr_r);

    volatile double dx = 2.0f / (X_Pixels_Size * movement.scale);
    volatile double dy = 2.0f / (X_Pixels_Size * movement.scale);

    volatile real_t tem_x0 = Y_0 + movement.y_offset;
    volatile real_t tem_y0 = X_0 + movement.x_offset;
    #undef Vector_length
}
