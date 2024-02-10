#include <windows.h>
#include "../lib/pch.h"
#include "hacks/hacks.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sstream>

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	c_overlay overlay;

	while (overlay.msg_loop() && !overlay.ready) {}

	return 1;
}

//console
//int main() {
//	c_overlay overlay;
//
//	while (overlay.msg_loop() && !overlay.ready) {}
//
//	return 1;
//}