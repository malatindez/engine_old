#include "Config.h"

#include <functional>

#include "core/Core.h"

#ifdef WIN32
#include <Windows.h>
int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR pCmdLine, int) {
#else
int main() {
#endif
	
	return 0;
}