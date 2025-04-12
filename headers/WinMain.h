#ifndef WINDOWS_MAIN
#define WINDOWS_MAIN
#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#include <vector>
#include <string>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    int argc;
    LPWSTR* wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!wargv) return -1;

    std::vector<std::string> args;
    std::vector<char*> argv;

    for (int i = 0; i < argc; i++) {
        int len = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, nullptr, 0, nullptr, nullptr);
        if (len > 0) {
            std::string arg(len - 1, '\0');
            WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, &arg[0], len, nullptr, nullptr);
            args.push_back(std::move(arg));
        }
    }

    for (auto& arg : args) {
        argv.push_back(arg.data());
    }
    argv.push_back(nullptr);

    LocalFree(wargv);

    return SDL_main(argc, argv.data());
}
#endif
#endif
