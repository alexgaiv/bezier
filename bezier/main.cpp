#include "appwindow.h"

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR lpCmdLine, int nCmdShow)
{
	AppWindow wnd;
	wnd.Show(SW_SHOW);
	wnd.MainLoop();
    return 0;
}
