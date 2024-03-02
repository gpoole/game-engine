#ifdef __APPLE__

#include <OpenGL/gl.h>

#else

#ifdef _WIN32
#include <Windows.h>
#endif

#if defined(__linux__) or defined(__FreeBSD__)

bool init_context() { return true; }

#endif

#include <GL/gl.h>

#endif

bool init_context();