#include"Calc_func.h"

void Calc_func_basic(uint32_t* pixels, Frame_movement movement) {

    real_t dx = 2.0f / (X_Pixels_Size * movement.scale);
    real_t dy = 2.0f / (X_Pixels_Size * movement.scale);

    movement.y_0 = Y_0 + movement.y_offset;
    movement.x_0 = X_0 + movement.x_offset;

    PARALLEL_FOR
    for (int y_pixels = 0; y_pixels < Y_Pixels_Size; y_pixels++){

        real_t current_y = movement.y_0 + y_pixels * dy;
        for (int x_pixels = 0; x_pixels < X_Pixels_Size; x_pixels++) {

            real_t start_x = movement.x_0 + x_pixels * dx;
            real_t x = start_x, y = current_y;
            real_t const_x = start_x, const_y = current_y;
            int num_of_iter = 0;
            for (; num_of_iter < Max_iter; num_of_iter++) {

                real_t sqr_x = x * x, sqr_y = y * y, mul_xy = x * y;
                real_t sqr_r = sqr_x + sqr_y;
                if (sqr_r >= Max_sqr_r)
                    break;

                x = sqr_x - sqr_y + const_x;
                y = mul_xy + mul_xy + const_y;
            }

            if (num_of_iter == 256)
                pixels[y_pixels * X_Pixels_Size + x_pixels] = 255;

            else
                pixels[y_pixels * X_Pixels_Size + x_pixels] = (num_of_iter << 24)                         |
                                                              (num_of_iter+num_of_iter << 16)             |
                                                              (num_of_iter+num_of_iter+num_of_iter << 8)  |
                                                               255;
        }
    }
}

void Calc_func_basic_void(uint32_t* pixels, Frame_movement movement) {

    __asm__ volatile (
        "" : :
        "r"(pixels),
        "r"(movement.x_offset),
        "r"(movement.scale)
    );
    volatile real_t dx = 2.0f / (X_Pixels_Size * movement.scale);
    volatile real_t dy = 2.0f / (X_Pixels_Size * movement.scale);
    volatile real_t tem_x0 = X_0 + movement.x_offset;
    return;
}
