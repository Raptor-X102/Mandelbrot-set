#include"Testing_func.h"

Test_data Single_test(Calc_func_t Calc_func, uint32_t* pixels, Frame_movement movement,
                      int test_num_of_cycles, const char* label) {

    Test_data test_data = {};

    test_data.frequency_start = Frequency_counter();
    test_data.timeh_t1 = clock();
    test_data.start_cycles = Rdtscp();

    for(int i = 0; i < test_num_of_cycles; i++)
        Calc_func(pixels, movement);

    test_data.end_cycles = Rdtscp();
    test_data.timeh_t2 = clock();
    test_data.frequency_end = Frequency_counter();
    Print_test_data(&test_data, test_num_of_cycles, label);

    test_data.test_num_of_cycles = test_num_of_cycles;
    return test_data;
}

void Print_test_data(Test_data* test_data, int num_of_cycles, const char* label) {

    double ticks = ((double)(test_data->end_cycles - test_data->start_cycles));
    double time_consumed_rdtsc = ticks / test_data->frequency_end;
    double duration = 1000.0 * (test_data->timeh_t2 - test_data->timeh_t1) / CLOCKS_PER_SEC;
    if(label)
        printf("%s\n", label);
    printf("+ Ticks = %.2e\n", ticks);
    printf("+ Start frequency = %.2e\n", test_data->frequency_start);
    printf("+ End frequency = %.2e\n", test_data->frequency_end);
    printf("+ Time consumed (time.h): %.2lf ms\n", duration);
    printf("+ Average FPS (time.h): %.2lf\n", num_of_cycles / duration * 1000);
    printf("+ Time consumed (rdtsc): %.2lf ms\n", time_consumed_rdtsc*1000);
    printf("+ Average FPS (rdtsc): %.2lf\n\n", (double)num_of_cycles / time_consumed_rdtsc);
}

void Test_accuracy(Test_data* test_data, Test_data* test_data_void, const char* label) {

    if(label)
        printf("%s\n", label);
    double average_ticks_sum  = (double)(test_data->end_cycles - test_data->start_cycles) /
                                (double) test_data->test_num_of_cycles;
    double average_ticks_void = (double)(test_data_void->end_cycles - test_data_void->start_cycles) /
                                (double) test_data_void->test_num_of_cycles;
    double ticks_differ = average_ticks_sum - average_ticks_void;
    double time_consumed = average_ticks_sum / test_data->frequency_end -
                           average_ticks_void / test_data_void->frequency_end;
    printf("+ Ticks difference = %.2e\n", ticks_differ);
    printf("+ Average ticks (sum) = %.2e\n", average_ticks_sum);
    printf("+ Average ticks (void) = %.2e\n", average_ticks_void);
    printf("+ Average ticks (real) = %.2e\n", ticks_differ);
    printf("+ Error (ticks) = %.2e\n", average_ticks_void * 100 / ticks_differ);
    printf("+ Time consumed (1 cycle): %.2lf ms\n", time_consumed*1000);
    printf("+ Average FPS (real): %.2lf\n\n", 1 / time_consumed);

}

void Master_test(uint32_t* pixels, Frame_movement movement) {

    time_t now = time(NULL);
    struct tm *curr_time = localtime(&now);
    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", curr_time);
    printf("#Test session\n");
    printf("```\n");
    printf("Test start\nTime: %s\n", buffer);
    printf("Resolution: %dx%d\n", X_Pixels_Size, Y_Pixels_Size);
    printf("Center: %.3lf, %.3lf\n", X_0, Y_0);
    printf("Max_iter: %d\n", Max_iter);
    printf("```\n");

    printf("## Calc_func_basic\n");
    Test_data test_data_void = Single_test(Calc_func_basic_void, pixels, movement,
                                           Test_num_of_cycles_void, "### void");
    Test_data test_data = Single_test(Calc_func_basic, pixels, movement,
                                      Test_num_of_cycles_basic, "### full version");
    Test_accuracy(&test_data, &test_data_void, "### Calc_func_basic real data");

    printf("## Calc_func_arrays\n");
    test_data_void = Single_test(Calc_func_arrays_void, pixels, movement,
                                           Test_num_of_cycles_void, "### void");
    test_data = Single_test(Calc_func_arrays, pixels, movement,
                                      Test_num_of_cycles, "### full version");
    Test_accuracy(&test_data, &test_data_void, "### Calc_func_arrays real data");

    printf("## Calc_func_intrinsics128_ps\n");
    test_data_void = Single_test(Calc_func_intrinsics128_ps_void, pixels, movement,
                                           Test_num_of_cycles_void, "### void");
    test_data = Single_test(Calc_func_intrinsics128_ps, pixels, movement,
                                      Test_num_of_cycles, "### full version");
    Test_accuracy(&test_data, &test_data_void, "### Calc_func_intrinsics128_ps real data");

    printf("## Calc_func_intrinsics128_pd\n");
    test_data_void = Single_test(Calc_func_intrinsics128_pd_void, pixels, movement,
                                           Test_num_of_cycles_void, "### void");
    test_data = Single_test(Calc_func_intrinsics128_pd, pixels, movement,
                                      Test_num_of_cycles, "### full version");
    Test_accuracy(&test_data, &test_data_void, "### Calc_func_intrinsics128_pd real data");

    printf("## Calc_func_intrinsics256_ps\n");
    test_data_void = Single_test(Calc_func_intrinsics256_ps_void, pixels, movement,
                                           Test_num_of_cycles_void, "### void");
    test_data = Single_test(Calc_func_intrinsics256_ps, pixels, movement,
                                      Test_num_of_cycles, "### full version");
    Test_accuracy(&test_data, &test_data_void, "### Calc_func_intrinsics256_ps real data");

    printf("## Calc_func_intrinsics256_pd\n");
    test_data_void = Single_test(Calc_func_intrinsics256_pd_void, pixels, movement,
                                           Test_num_of_cycles_void, "### void");
    test_data = Single_test(Calc_func_intrinsics256_pd, pixels, movement,
                                      Test_num_of_cycles, "### full version");
    Test_accuracy(&test_data, &test_data_void, "### Calc_func_intrinsics256_pd real data");

    now = time(NULL);
    curr_time = localtime(&now);

    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", curr_time);

    printf("```\n");
    printf("Test end\nTime: %s\n", buffer);
    printf("```\n\n\n");
}
