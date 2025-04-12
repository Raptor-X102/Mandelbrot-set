#include"Rdtscp.h"

uint64_t Rdtscp() {

    uint32_t lo, hi;
    __asm__ volatile (

        "rdtscp\n\t"
        "mov %%eax, %0\n\t"
        "mov %%edx, %1\n\t"
        "cpuid\n\t"
        : "=r"(lo), "=r"(hi)
        :
        : "%rax", "%rbx", "%rcx", "%rdx"
    );
    return ((uint64_t)hi << 32) | lo;
}

double Frequency_counter() {

    uint64_t start_cycles = Rdtscp();

    LARGE_INTEGER qpc_start, qpc_end, freq;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&qpc_start);
    Sleep(100);
    QueryPerformanceCounter(&qpc_end);
    double seconds = (double)(qpc_end.QuadPart - qpc_start.QuadPart) / freq.QuadPart;

    uint64_t end_cycles = Rdtscp();
    return (end_cycles - start_cycles) / seconds;
}

void GenerateCpuLoad(uint64_t iterations) {
    volatile double x = 0.0;
    for (uint64_t i = 0; i < iterations; i++) {
        x += (i * 3.14159);
    }
}

double EstimateCpuFrequencyMHz(DWORD cpu_index, int sleep_ms, uint64_t load_iterations) {

    SetThreadAffinityMask(GetCurrentThread(), 1ULL << cpu_index);

    HMODULE hNtDll = GetModuleHandleA("ntdll.dll");
    if (!hNtDll) return -1;

    NtQuerySystemInformation_t NtQuerySystemInformation =
        (NtQuerySystemInformation_t)GetProcAddress(hNtDll, "NtQuerySystemInformation");
    if (!NtQuerySystemInformation) return -1;

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    DWORD num_cpus = sysInfo.dwNumberOfProcessors;
    if (cpu_index >= num_cpus) return -1;

    SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION* info1 =
        (SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION*)calloc(num_cpus, sizeof(*info1));
    SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION* info2 =
        (SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION*)calloc(num_cpus, sizeof(*info2));

    NtQuerySystemInformation(SystemProcessorPerformanceInformation, info1, sizeof(*info1) * num_cpus, NULL);

    GenerateCpuLoad(load_iterations);

    Sleep(sleep_ms);

    NtQuerySystemInformation(SystemProcessorPerformanceInformation, info2, sizeof(*info2) * num_cpus, NULL);

    ULONGLONG delta_kernel = info2[cpu_index].KernelTime.QuadPart - info1[cpu_index].KernelTime.QuadPart;
    ULONGLONG delta_user   = info2[cpu_index].UserTime.QuadPart - info1[cpu_index].UserTime.QuadPart;
    ULONGLONG delta_time   = delta_kernel + delta_user;

    free(info1);
    free(info2);

    double seconds = sleep_ms / 1000.0;
    double cpu_time_seconds = delta_time * 1e-7;
    double frequency_ghz = cpu_time_seconds / seconds;

    printf("delta_time = %llu ticks, seconds = %.6f\n", delta_time, seconds);
    return frequency_ghz * 1000.0;
}
