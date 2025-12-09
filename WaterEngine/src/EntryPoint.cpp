#ifdef _WIN32
#ifdef NDEBUG
// -----------------------------------------------
// RELEASE MODE (no console, WinMain entrypoint)
// -----------------------------------------------
#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
#pragma comment(linker, "/ENTRY:WinMainCRTStartup")
#endif
#endif

#include "EntryPoint.h"
#include "Framework/Application.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#if defined(_WIN32) && defined(NDEBUG)

// -------------------------------------------------------
// RELEASE BUILD ? use WinMain (no console)
// -------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    we::Application* App = GetApplication();
    App->Run();
    delete App;
    return 0;
}

#else

// -------------------------------------------------------
// DEBUG BUILD or NON-WINDOWS -> normal main()
// -------------------------------------------------------
int main(int argc, char** argv)
{
    we::Application* App = GetApplication();
    App->Run();
    delete App;
    return 0;
}

#endif
