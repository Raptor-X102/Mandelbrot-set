#ifndef RDTSCP
#define RDTSCP
#define _CRT_SECURE_NO_WARNINGS
#include<stdint.h>
#include<stdio.h>
#include<profileapi.h>
#include<winnt.h>
#include <windows.h>
#include <winternl.h>
#include <synchapi.h>
#include <time.h>
typedef NTSTATUS (NTAPI *NtQuerySystemInformation_t)(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
);
uint64_t Rdtscp();
double Frequency_counter();
void GenerateCpuLoad(uint64_t iterations);
double EstimateCpuFrequencyMHz(DWORD cpu_index, int sleep_ms, uint64_t load_iterations);

#endif
