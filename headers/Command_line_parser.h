#ifndef COMMAND_LINE
#define COMMAND_LINE

#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include<stdlib.h>
#include"Debug_printf.h"
#define GRAPHICS_FLAG "--graphics"
#define NO_GRAPHICS_FLAG "--no-graphics"
#define CYCLES_FLAG "--cycles"
#define INTRINSICS_FLAG "INTRINSICS"
#define M128_FLAG "__M_128"
#define M256_FLAG "__M_256"
#define PS_TYPE "--ps"
#define PD_TYPE "--pd"
#define BASIC_FLAG "BASIC"
#define ARRAYS_FLAG "ARRAYS"
#define MASTER_TEST "MASTER_TEST"
const int64_t Graphics = 0x1;
const int64_t No_Graphics = 0x0;
const int64_t Basic = 0x2;
const int64_t Arrays = 0x4;
const int64_t Intrinsics = 0x8;
const int64_t MasterTest = 0x10;
const int64_t MM_128_reg = 0x20;
const int64_t Ps_type = 0x40;


const int64_t No_flag = 0xBEEEAFFF;
int64_t Cmd_l_parser(int argc, char** argv, int* test_num_of_cycles);

#endif
