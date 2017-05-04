#include <windows.h>
#pragma comment(lib,"ffmpeg\\lib\\win\\avcodec.lib")
#pragma comment(lib,"ffmpeg\\lib\\win\\avformat.lib")
#pragma comment(lib,"ffmpeg\\lib\\win\\avutil.lib")
#pragma comment(lib,"ffmpeg\\lib\\win\\swscale.lib")
#pragma comment(lib,"ffmpeg\\lib\\win\\swresample.lib")
int _stdcall DllMain( HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved )
{
	return 1;
}

