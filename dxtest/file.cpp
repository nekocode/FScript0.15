#include "file.h"
#include "posList.h"
#include "funcReturnList.h"
#include "variable.h"
#include <tchar.h>

cmd cmdread;

ifstream scriptF;
long nowpos=0;           //当前指针位置

char line[Buffer];
int i=0;                      //now pos
char cmdstr[cmdBuffer];

char strval[3][valBuffer];    //字符串参数
bool mask[2];              //是否使用遮罩图模式
int intval[5];                //数字参数
int intpath[3][4];            //路径中的数字

int valnum=1;                 //参数数量,可变函数才用到

extern int stop;

void initFile()
{
	scriptF.open(ScriptFirstFileName,ios::in|ios::binary);
}

void OffFile()
{
	scriptF.close();
}

bool read()
{
	if(!scriptF) return false;

	scriptF.getline(line,Buffer);
	nowpos = scriptF.tellg();

	//跳过非命令行
	while((line[0]>'z'||line[0]<'a')&&line[0]!='\"')
	{
		//跳过/* */内的命令   (注释)
		if(line[0]=='/'&&line[1]=='*')
		{
			if(!passNotes(0))  //所有脚本已经读取完毕
			{
				cmdread=endCmd;
//				scriptF.close();
				return false;
			}
		}
		//跳过{}内的命令    (函数)
		if(line[0]=='{')
		{
			if(!hasReturn())
			{
				if(!passNotes(1))  //所有脚本已经读取完毕
				{
					cmdread=endCmd;
//					scriptF.close();
					return false;
				}
			}
		}
		else if(line[0]=='}')
		{
			if(hasReturn())
			{
				scriptF.seekg(getReturn());
				cmdread=passThisCmd;
				return read();
			}
		}

		scriptF.getline(line,Buffer);
		if(scriptF.eof())      //判断是否到结尾
		{
			cmdread=endCmd;
//			scriptF.close();
			return false;      //所有脚本已经读取完毕
		}
	}

	if(line[0]=='\"')   //直接echo命令
	{
		i=0;   //将指针跳到开头,使得获取参数的操作从开头开始
	}
	else
	{
		//脚本命令关键字[cmdstr]的解析         *
		while(line[i]!=' '&&line[i]!=13&&line[i]!='\0')
		{
			cmdstr[i] = line[i];
			i++;
			if(i>cmdBuffer)
				break;
		}
		cmdstr[i] = '\0';
	}

	return getcmd();
}

//判断并设置命令
bool getcmd()
{
	//===========================================================
	//判断命令并根据命令读取参数值
	if(line[0]=='\"'||!strcmp(cmdstr,"echo"))
	{
		cmdread=echoCmd;
		getStr();
		valnum=1;

		if(hasnext())
		{
			getStr(1);
			valnum++;         //valnum=2,说明有两个val 
		}
	}
	//===========================================================
	else if(!strcmp(cmdstr,"role"))
	{
		cmdread=roleCmd;
		getPath();
		valnum=1;

		if(hasnext())
		{
			getPath(1);
			valnum++;         //valnum=2,说明有两个val
		}
	}
	//===========================================================
	else if(!strcmp(cmdstr,"clecho"))
	{
		cmdread=clearechoCmd;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"clrole"))
	{
		cmdread=clearroleCmd;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"bk"))
	{
		cmdread=backgroundCmd;
		getPath();

		if(hasnext())
			getInt();
		else
			intval[0]=0;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"clbk"))
	{
		cmdread=clearbackgroundCmd;
		if(hasnext())
			getInt();
		else
			intval[0]=0;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"stop"))
	{
		cmdread=stopCmd;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"wait"))
	{
		cmdread=sleepCmd;
		getInt();
	}
	//===========================================================
	else if(!strcmp(cmdstr,"playm"))
	{
		cmdread=playmusicCmd;
		getPath();

		if(hasnext())
			getInt();
		else
			intval[0]=0;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"stopm"))
	{
		cmdread=stopmusicCmd;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"playm2"))
	{
		cmdread=playmusic2Cmd;
		getPath();

		if(hasnext())
			getInt();
		else
			intval[0]=0;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"playm3"))
	{
		cmdread=playmusic3Cmd;
		getPath();

		if(hasnext())
			getInt();
		else
			intval[0]=0;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"stopm2"))
	{
		cmdread=stopmusic2Cmd;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"stopm3"))
	{
		cmdread=stopmusic3Cmd;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"mbk"))
	{
		cmdread=movebackgroundCmd;
		getInt(0);
		hasnext();
		getInt(1);
		hasnext();
		getInt(2);
	}
	//===========================================================
	else if(!strcmp(cmdstr,"call"))
	{
		char* funcname = getName(PosNameLen);
		if(!callfucn(funcname))
		{
			return read();
		}
		else
			cmdread=err;

		delete[] funcname;
	}

	//===========================================================
	else if(!strcmp(cmdstr,"if"))
	{
		if(!ifjude())
		{
			do
			{
				i = 0;
				scriptF.getline(line,Buffer);
				if(line[0]=='e'&&line[1]=='l'&&line[2]=='s'&&line[3]=='e')
				{
					return read();
				}
			}
			while(line[0]!='e'||line[1]!='n'||line[2]!='d'||line[3]!='i'||line[4]!='f');
		}
		
		cmdread = passThisCmd;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"else"))
	{
		while(line[0]!='e'||line[1]!='n'||line[2]!='d'||line[3]!='i'||line[4]!='f')
		{
			i = 0;
			scriptF.getline(line,Buffer);
		}

		cmdread = passThisCmd;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"goto"))
	{
		char *posname = getName(PosNameLen);
		int p = getpos(posname);
		delete[] posname;

		if(p!=-1)
		{
			deleteAllReturn();
			scriptF.clear();
			scriptF.seekg(p,ios::beg);
			i=0;
			return read();
		}
		else
			cmdread=err;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"set"))
	{
		calculate();
		cmdread = passThisCmd;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"int"))
	{
		char *varname = getName(varNameLen);
		int val=0;

		if(hasnext())
		{
			getInt(0);
			val = intval[0];
		}

		addVar(varname,val);
		delete[] varname;

		cmdread = passThisCmd;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"nextfile"))
	{
		getPath();

		scriptF.close();
		scriptF.open(strval[0],ios::in);      //读取下一个脚本文件   *
		i=0;
		deleteAllReturn();
		getAllPos();
		return read();
	}
	//===========================================================
	else if(!strcmp(cmdstr,"btn"))
	{
		//加入btn
		getInt(0);

		hasnext();
		getStr(0);

		hasnext();
		getPath(1);
		valnum=3;

		if(hasnext())
		{
			getPath(2);
			valnum=4;
		}

		cmdread = btnCmd;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"clbtn"))
	{
		//清除btn
		getInt(0);
		cmdread = clbtnCmd;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"event"))
	{
		//设置事件
		getPath(0);
		hasnext();
		strcpy_s(strval[1],getName(PosNameLen));
		cmdread = eventCmd;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"lock"))
	{
		//锁定状态,(只有通过event才能重新激活)
		stop = 10;
		cmdread = passThisCmd;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"video"))
	{
		getPath();
		valnum = 1;

		if(hasnext())
		{
			getInt();
			valnum++;
		}

		cmdread=videoCmd;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"ecolor"))
	{
		getInt();
		hasnext();
		getInt(1);
		hasnext();
		getInt(2);
		if(hasnext())
			getInt(3);
		else
			intval[3] = 255;

		cmdread=echocolorCmd;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"rcolor"))
	{
		getInt();
		hasnext();
		getInt(1);
		hasnext();
		getInt(2);
		if(hasnext())
			getInt(3);
		else
			intval[3] = 255;

		cmdread=rolenamecolorCmd;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"epos"))
	{
		getInt();
		hasnext();
		getInt(1);
		if(hasnext())
			getInt(2);
		else
			intval[2]=0;
		cmdread=echoposCmd;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"ewidth"))
	{
		getInt();
		cmdread=echowidthCmd;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"efontsize"))
	{
		getInt();
		cmdread=echofontsizeCmd;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"rpos"))
	{
		getInt();
		hasnext();
		getInt(1);
		if(hasnext())
			getInt(2);
		else
			intval[2]=0;
		cmdread=rolenameposCmd;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"rwidth"))
	{
		getInt();
		cmdread=rolenamewidthCmd;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"rfontsize"))
	{
		getInt();
		cmdread=rolenamefontsizeCmd;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"epic"))
	{
		getPath();
		hasnext();
		getInt();
		hasnext();
		getInt(1);
		cmdread=echopicCmd;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"rpic"))
	{
		getPath();
		hasnext();
		getInt();
		hasnext();
		getInt(1);
		cmdread=rolenamepicCmd;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"fps"))
	{
		cmdread=fpsCmd;
	}
	//===========================================================
	else
	{
		cmdread=err;
	}
	//===========================================================

	i=0;
	return true;
}

//获取所有跳转点(包括函数)
void getAllPos()
{
	delallpos();

	while(1)
	{
		scriptF.getline(line,Buffer);
		if(scriptF.eof())
			break;

		//跳过注释符
		if(line[0]=='\\'&&line[1]=='*')
			passNotes(0);

		if(line[0]==':'||line[0]=='*')
		{
			i++;

			//解析入口点
			char *posname = getName(PosNameLen);

			//将入口点名称存入列表
			nowpos = scriptF.tellg();
			if(line[0]==':')
				addpos(posname,nowpos);
			if(line[0]=='*')
				addpos(posname,nowpos,true);

			delete[] posname;
			i=0;
		}
	}

	//如果遇到文件结束标志的话需要清楚指针信息才可以重新定位到文件头
	scriptF.clear();
	scriptF.seekg(0);

	nowpos = scriptF.tellg();
}

//获取文本(echo)
void getStr(int num)   //返回终止时的地址
{
	int i2=0;

	while(line[i]==' ')
		i++;

	while(++i)                         //这一步实际上已经跳过了开头的 "
	{
		if(line[i]=='\"' && line[i-1]!='\\')
		{
			strval[num][i2] = '\0';
			i++;
			break;
		}
		//双引号内的处理
		else
		{
			if(line[i]=='\\')               //转义符号的处理:\",\n
			{
				if(line[i+1]=='\"')
				{
					i++;
					strval[num][i2] = '\"';
					i2++;
				}
				else if(line[i+1]=='n')
				{
					i++;
					strval[num][i2] = '\n';
					i2++;
				}
				else
				{
					strval[num][i2] = line[i];
					i2++;
				}
			}
			else
			{
				strval[num][i2] = line[i];
				i2++;
			}
		}
	}

}

//获取数字
void getInt(int num)
{
	char str[10];
	int i2=0;
	bool negative=false;    //判断是否负数

	while(line[i]==' ')
		i++;

	//判断是否有负号
	if(line[i]=='-')
	{
		i++;
		negative = true;
	}

	//判断是数字还是变量
	if(isdigit(line[i]))
	{
		while(isdigit(line[i]))    //如果是数字的话
		{
			str[i2] = line[i];
			i++;
			i2++;
		}
		str[i2] = '\0';
		intval[num] = atoi(str);     //转换为数字

		if(negative)
			intval[num] = -intval[num];
	}
	else
	{
		char *var2name = getName(varNameLen);
		intval[num] = getVar(var2name);
		delete[] var2name;

		if(negative)
			intval[num] = -intval[num];
	}
}

//获取路径中的数字
void getPathInt(int num1)
{
	char str[10];
	int i2=0,i3=0;

	for(;i3<4;i3++)
	{
		if(line[i]=='-')
		{
			str[i2] = '-';
			i++;
			i2++;
		}
		while(isdigit(line[i]))    //如果是数字的话
		{
			str[i2] = line[i];
			i++;
			i2++;
		}
		str[i2] = '\0';
		i2 = 0;
		intpath[num1][i3] = atoi(str);
		if(line[i]=='\"')
		{
			i++;break;
		}
		else
			i++;
	}

}

//获取路径
void getPath(int num)
{
	int i2=0;

	//清空原本数组为0
	for(int i3=0;i3<4;i3++)
		intpath[num][i3]=0;

	//跳过空格部分
	while(line[i]==' ')
		i++;

	//判断是否需要使用mask模式
	if(line[i]=='m')
	{
		mask[num] = true;
		i++;
	}
	else
		mask[num] = false;

	while(++i)                         //这一步实际上已经跳过了开头的 "
	{
		if(line[i]=='\"' && line[i-1]!='\\')
		{
			strval[num][i2] = '\0';
			i++;
			break;
		}
		//双引号内的处理
		else
		{
			if(line[i]==':')               //转义符号的处理  :
			{
				strval[num][i2] = '\0';
				i++;
				getPathInt(num);
				break;
			}
			else
			{
				strval[num][i2] = line[i];
				i2++;
			}
		}
	}
}

//获得函数名,跳转点名字或变量名字
char* getName(int len)
{
	char *name = new char[len];
	int i2=0;

	while(line[i]==' ')
		i++;

	while((line[i]>47&&line[i]<58)||line[i]>63)
	{
		name[i2] = line[i];
		i++;i2++;

		if(i2>len)
			break;
	}
	name[i2]='\0';

	//跳过分隔符号
	//i++;

	return name;
}

//是否有"," 即是否还存在下一个数值
bool hasnext()
{
	int i2;
	if(line[i]==13)
		return false;

	while(line[i]==' ')
		i++;

	if(line[i]==',')
	{
		i++;           //跳过逗号 ,
		return true;
	}
	else if(line[i]!='\0')
		return true;
	else
		return false;
}

//跳过注释和函数
bool passNotes(int what)
{
	scriptF.getline(line,Buffer);

	if(what==0)
	{
		while(line[0]!='*'||line[1]!='/')
		{
			scriptF.getline(line,Buffer);
			if(scriptF.eof())      //判断是否到结尾
			{
				return false;      //所有脚本已经读取完毕,仍未发现注释结束符
			}
		}
	}
	else
	{
		while(line[0]!='}')
		{
			scriptF.getline(line,Buffer);
			if(scriptF.eof())      //判断是否到结尾
			{
				return false;      //所有脚本已经读取完毕,仍未发现注释结束符
			}
		}
	}

	return true;
}

//变量表达式计算
void calculate()
{
	char *var1name = getName(varNameLen);
	int val1,val2=0,temppos;

	while(line[i]==' '||line[i]=='=')
		i++;

	getInt(0);
	val1 = intval[0];

	while(line[i]==' ')
		i++;
	//储存符号所在位置
	temppos = i;
	i++;
	while(line[i]==' ')
		i++;

	if(line[i]!=13&&line[i]!='\0')
	{
		getInt(0);
		val2 = intval[0];
	}

	if(line[temppos]=='+')
		val1 = val1 + val2;
	else if(line[temppos]=='-')
		val1 = val1 - val2;
	else if(line[temppos]=='*')
		val1 = val1 * val2;
	else if(line[temppos]=='//')
		val1 = val1 / val2;

	setVar(var1name,val1);
}

//判断语句
bool ifjude()
{
	int val1,val2;
	int temppos;
	bool doub=false;

	while(line[i]==' ')
		i++;

	getInt(0);
	val1 = intval[0];

	while(line[i]==' ')
		i++;
	//储存符号所在位置
	temppos = i;
	i++;
	while(line[i]==' ')
		i++;
	if(line[i]=='='||line[i]=='>'||line[i]=='<')
	{
		doub = true;
		i++;
	}
	while(line[i]==' ')
		i++;

	getInt(0);
	val2 = intval[0];

	//判断符号
	if(!doub)
	{
		if(line[temppos]=='=')
		{
			if(val1==val2)
				return true;
		}
		else if(line[temppos]=='>')
		{
			if(val1>val2)
				return true;
		}
		else if(line[temppos]=='<')
		{
			if(val1<val2)
				return true;
		}
	}
	else
	{
		if(line[temppos]=='='&&line[temppos+1]=='=')
		{
			if(val1==val2)
				return true;
		}
		else if((line[temppos]=='>'&&line[temppos+1]=='=')||(line[temppos+1]=='>'&&line[temppos]=='='))
		{
			if(val1>=val2)
				return true;
		}
		else if((line[temppos]=='<'&&line[temppos+1]=='=')||(line[temppos+1]=='>'&&line[temppos]=='='))
		{
			if(val1<=val2)
				return true;
		}
	}

	return false;
}

bool callfucn(char* funcname)
{
	int p = getpos(funcname,true);

	if(p!=-1)
	{
		addReturn(scriptF.tellg());

		scriptF.clear();
		scriptF.seekg(p,ios::beg);
		i=0;
		return true;
	}
	else
		return false;
}