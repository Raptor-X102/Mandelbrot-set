#include"Command_line_parser.h"

int64_t Cmd_l_parser(int argc, char** argv, int* test_num_of_cycles) {

    int64_t ret_val = 0;
    bool flag_status = false;
    bool registers_flag = false;
    bool intrinsics_flag = false;
    bool config_ver = false;
    bool cycles_flag = false;
    bool no_graphics_flag = false;
    bool precision_flag = false;

    for(int i = 0; i < argc; i++) {

        if(!strcmp(argv[i], GRAPHICS_FLAG)) {

            ret_val |= Graphics;
            flag_status = true;
        }

        else
        if(!strcmp(argv[i], NO_GRAPHICS_FLAG)) {

            ret_val |=  No_Graphics;
            flag_status = true;
            no_graphics_flag = true;
        }

        else
        if(!strcmp(argv[i], BASIC_FLAG)) {

            ret_val |=  Basic;
            ret_val &=  ~Arrays;
            ret_val &=  ~Intrinsics;
            ret_val &=  ~MasterTest;
            config_ver = true;
        }

        else
        if(!strcmp(argv[i], ARRAYS_FLAG)) {

            ret_val &=  ~Basic;
            ret_val |=  Arrays;
            ret_val &=  ~Intrinsics;
            ret_val &=  ~MasterTest;
            config_ver = true;
        }

        else
        if(!strcmp(argv[i], INTRINSICS_FLAG)) {

            ret_val &=  ~Basic;
            ret_val &=  ~Arrays;
            ret_val |=  Intrinsics;
            ret_val &=  ~MasterTest;
            config_ver = true;
            intrinsics_flag = true;
        }

        else
        if(!strcmp(argv[i], MASTER_TEST)) {

            ret_val &=  ~Basic;
            ret_val &=  ~Arrays;
            ret_val &=  ~Intrinsics;
            ret_val |=  MasterTest;
            config_ver = true;
            flag_status = true;
        }

        else
        if(!strcmp(argv[i], M128_FLAG)) {

            ret_val |=  MM_128_reg;
            registers_flag = true;
        }

        else
        if(!strcmp(argv[i], M256_FLAG)) {

            ret_val &=  ~MM_128_reg;
            registers_flag = true;
        }

        else
        if(!strcmp(argv[i], PS_TYPE)) {

            ret_val |=  Ps_type;
            precision_flag = true;
        }

        else
        if(!strcmp(argv[i], PD_TYPE)) {

            ret_val &=  ~Ps_type;
            precision_flag = true;
        }

        else
        if(!strcmp(argv[i], CYCLES_FLAG)) {

            char* str_end = NULL;
            int digit_system = 10;
            *test_num_of_cycles = strtol(argv[i+1], &str_end, digit_system);
            cycles_flag = true;
            i +=2;
        }
    }

    if(!flag_status) {

        DEBUG_PRINTF("ERROR: no graphics mode inserted\n");
        ret_val = No_flag;
    }

    if(intrinsics_flag && (!registers_flag || !precision_flag)) {

        DEBUG_PRINTF("ERROR: no registers flag or precision inserted\n");
        ret_val = No_flag;
    }

    if(!config_ver) {

        DEBUG_PRINTF("ERROR: no configuration version inserted\n");
        ret_val = No_flag;
    }

    if(!cycles_flag && no_graphics_flag) {

        DEBUG_PRINTF("ERROR: no test numbers of cycles inserted\n");
        ret_val = No_flag;
    }

    return ret_val;
}
