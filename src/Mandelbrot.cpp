#include"Def_and_decl.h"
#include"WinMain.h"
#include"Testing_func.h"

int main(int argc, char* argv[]) {

    uint32_t* pixels = (uint32_t*) calloc(X_Pixels_Size * Y_Pixels_Size, sizeof(uint32_t));
    if (!pixels) {

        DEBUG_PRINTF("ERROR: pixels is nullptr\n");
        return 0;
    }

    Frame_movement movement = { .scale = 1.0, .x_offset = 0.0f, .y_offset = 0.0f, .x_0 = X_0, .y_0 = Y_0};
    int test_num_of_cycles = 0;
    int64_t config_flags = Cmd_l_parser(argc, argv, &test_num_of_cycles);

    if (config_flags == No_flag) {

        DEBUG_PRINTF("ERROR: no config_flags flag was input\n");
        return 0;
    }

    else {

        if(config_flags & MasterTest) {

            freopen("Testing_data_log.txt", "a", stdout);
            freopen("Testing_data_log.txt", "a", stderr);
            Master_test(pixels, movement);
        }

        else {

            Calc_func_t Calc_func = NULL;
            if (config_flags & Basic)
                Calc_func = Calc_func_basic;

            else if (config_flags & Arrays)
                Calc_func = Calc_func_arrays;

            else if (config_flags & Intrinsics) {

                if(config_flags & MM_128_reg) {

                    if(config_flags & Ps_type)
                        Calc_func = Calc_func_intrinsics128_ps;

                    else
                        Calc_func = Calc_func_intrinsics128_pd;
                }

                else {

                    if(config_flags & Ps_type)
                        Calc_func = Calc_func_intrinsics256_ps;

                    else
                        Calc_func = Calc_func_intrinsics256_pd;
                }
            }

            if(!Calc_func) {

                DEBUG_PRINTF("ERROR: Calc_func null ptr\n");
                return 0;
            }

            if (config_flags & Graphics)
                Graph_mode_loop(pixels, movement, Calc_func);


            else {

                Test_data test_data_void = Single_test(Calc_func_basic_void, pixels, movement,
                                           Test_num_of_cycles_void, NULL);
                Test_data test_data = Single_test(Calc_func_basic, pixels, movement,
                                      test_num_of_cycles, NULL);
                Test_accuracy(&test_data, &test_data_void, NULL);
            }
        }
    }

    return 0;
}
