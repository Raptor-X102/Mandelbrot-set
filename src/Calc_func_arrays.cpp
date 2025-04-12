#include "Calc_func.h"

void Calc_func_arrays(uint32_t* pixels, Frame_movement movement) {

    real_t dx = 2.0f / (X_Pixels_Size * movement.scale);
    real_t dy = 2.0f / (X_Pixels_Size * movement.scale);

    movement.y_0 = Y_0 + movement.y_offset;
    movement.x_0 = X_0 + movement.x_offset;

    PARALLEL_FOR
    for (int y_pixels = 0; y_pixels < Y_Pixels_Size; y_pixels++) {

        real_t vec_r_t_x[Vector_length] = {};
        real_t vec_r_t_y[Vector_length] = {};
        real_t vec_r_t_sqr_x[Vector_length] = {};
        real_t vec_r_t_sqr_y[Vector_length] = {};
        real_t vec_r_t_mul_xy[Vector_length] = {};
        real_t vec_r_t_sqr_r[Vector_length] = {};
        real_t const_x[Vector_length] = {};
        real_t const_y[Vector_length] = {};

        real_t current_y = movement.y_0 + y_pixels * dy;
        for (int x_pixels = 0; x_pixels < X_Pixels_Size; x_pixels += Vector_length) {

            mask_t vec_status = -1;
            uint32_t vec_colour[Vector_length] = {};
            real_t start_x = movement.x_0 + x_pixels * dx;
            for (int i = 0; i < Vector_length; i++) {

                vec_r_t_x[i] = start_x + i * dx;
                vec_r_t_y[i] = current_y;
                const_x[i] = start_x + i * dx;
                const_y[i] = current_y;
            }

            for (int i = 0; i < Vector_length; i++) {

                vec_r_t_sqr_x[i] = vec_r_t_x[i] * vec_r_t_x[i];
                vec_r_t_sqr_y[i] = vec_r_t_y[i] * vec_r_t_y[i];
                vec_r_t_mul_xy[i] = vec_r_t_x[i] * vec_r_t_y[i];
                vec_r_t_sqr_r[i] = vec_r_t_sqr_x[i] + vec_r_t_sqr_y[i];
            }

            int num_of_iter = 0;
            for (; num_of_iter < Max_iter; num_of_iter++) {

                for (int i = 0; i < Vector_length; i++) {

                    if (vec_r_t_sqr_r[i] >= Max_sqr_r) {

                        vec_status &= ~mask_array[i];
                    }

                    vec_r_t_x[i] = vec_r_t_sqr_x[i] - vec_r_t_sqr_y[i] + const_x[i];
                    vec_r_t_y[i] = vec_r_t_mul_xy[i] + vec_r_t_mul_xy[i] + const_y[i];
                    vec_r_t_sqr_x[i] = vec_r_t_x[i] * vec_r_t_x[i];
                    vec_r_t_sqr_y[i] = vec_r_t_y[i] * vec_r_t_y[i];
                    vec_r_t_mul_xy[i] = vec_r_t_x[i] * vec_r_t_y[i];
                    vec_r_t_sqr_r[i] = vec_r_t_sqr_x[i] + vec_r_t_sqr_y[i];
                }

                if (!vec_status)
                    break;

                for (int i = 0; i < Vector_length; i++) {

                    mask_t mask_cast = vec_status >> (8 * i);
                    vec_colour[i] += (mask_cast & 0xFF) == 0xFF;
                }
            }

            for (int i = 0; i < Vector_length; i++) {

                if ((vec_status & mask_array[i]))
                    pixels[y_pixels * X_Pixels_Size + x_pixels + i] = Max_iter;

                else
                    pixels[y_pixels * X_Pixels_Size + x_pixels + i] = (vec_colour[i] << 24)                                  |
                                                                        (vec_colour[i] + vec_colour[i] << 16)                |
                                                                        (vec_colour[i] + vec_colour[i] + vec_colour[i] << 8) |
                                                                        255;
            }
        }
    }
}

void Calc_func_arrays_void(uint32_t* pixels, Frame_movement movement) {

    __asm__ volatile (
        "" : :
        "r"(pixels),
        "r"(movement.x_offset),
        "r"(movement.scale)
    );
    volatile real_t dx = 2.0f / (X_Pixels_Size * movement.scale);
    volatile real_t dy = 2.0f / (X_Pixels_Size * movement.scale);

    volatile real_t tem_x0 = Y_0 + movement.y_offset;
    volatile real_t tem_y0 = X_0 + movement.x_offset;
    return;
}
