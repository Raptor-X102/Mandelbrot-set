#ifndef TESTING_FUNC
#define TESTING_FUNC

#include"Def_and_decl.h"
#include"Rdtscp.h"

Test_data Single_test(Calc_func_t Calc_func, uint32_t* pixels, Frame_movement movement,
                      int test_num_of_cycles, const char* label);
void Print_test_data(Test_data* test_data, int num_of_cycles, const char* label);
void Master_test(uint32_t* pixels, Frame_movement movement);
void Test_accuracy(Test_data* test_data, Test_data* test_data_void, const char* label);

#endif
