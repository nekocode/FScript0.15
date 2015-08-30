#include <fstream>
using namespace std;

#define ScriptFirstFileName "0.txt"
#define Buffer 260
#define cmdBuffer 20
#define valBuffer (Buffer-cmdBuffer)


void initFile();
void OffFile();
void getAllPos();
bool read();
bool getcmd();
void getStr(int num=0);
void getInt(int num=0);
void getPath(int num=0);
char* getName(int len);
bool hasnext();
bool passNotes(int);
void calculate();
bool ifjude();
bool callfucn(char* funcname);


enum cmd{echoCmd,clearechoCmd,
	playmusicCmd,playmusic2Cmd,playmusic3Cmd,stopmusicCmd,stopmusic2Cmd,stopmusic3Cmd,
	backgroundCmd,clearbackgroundCmd,movebackgroundCmd,
	stopCmd,sleepCmd,
	roleCmd,clearroleCmd,
	videoCmd,
	echopicCmd,rolenamepicCmd,echoposCmd,echowidthCmd,echocolorCmd,echofontsizeCmd,rolenameposCmd,rolenamewidthCmd,rolenamecolorCmd,rolenamefontsizeCmd,fpsCmd,
	btnCmd,clbtnCmd,
	eventCmd,
	endCmd,passThisCmd,err};

