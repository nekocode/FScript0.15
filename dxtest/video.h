#include<dshow.h>
#include <d3d9.h>
#include <vmr9.h>

bool initVideo(void);
void RefVideo();
void releaseVideo();
bool playVideo(LPCTSTR path);
void stopVideo();
bool VideoIsEnd();