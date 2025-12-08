#ifdef _WIN32
#ifdef NDEBUG
// In Release: use WinMain and hide console window
#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
#pragma comment(linker, "/ENTRY:WinMainCRTStartup")
#else
// In Debug: use console window
#pragma comment(linker, "/SUBSYSTEM:CONSOLE")
#endif
#endif

#include "EntryPoint.h"
#include "Framework/Application.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef _WIN32

// Windows entry point (Release build uses this)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    we::Application* App = GetApplication();
    App->Run();

    delete App;
    return 0;
}

#else

// Non-Windows entry point
int main(int argc, char** argv)
{
    we::Application* App = GetApplication();
    App->Run();

    delete App;
    return 0;
}

#endif
