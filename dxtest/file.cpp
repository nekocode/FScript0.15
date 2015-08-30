#include "file.h"
#include "posList.h"
#include "funcReturnList.h"
#include "variable.h"
#include <tchar.h>

cmd cmdread;

ifstream scriptF;
long nowpos=0;           //��ǰָ��λ��

char line[Buffer];
int i=0;                      //now pos
char cmdstr[cmdBuffer];

char strval[3][valBuffer];    //�ַ�������
bool mask[2];              //�Ƿ�ʹ������ͼģʽ
int intval[5];                //���ֲ���
int intpath[3][4];            //·���е�����

int valnum=1;                 //��������,�ɱ亯�����õ�

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

	//������������
	while((line[0]>'z'||line[0]<'a')&&line[0]!='\"')
	{
		//����/* */�ڵ�����   (ע��)
		if(line[0]=='/'&&line[1]=='*')
		{
			if(!passNotes(0))  //���нű��Ѿ���ȡ���
			{
				cmdread=endCmd;
//				scriptF.close();
				return false;
			}
		}
		//����{}�ڵ�����    (����)
		if(line[0]=='{')
		{
			if(!hasReturn())
			{
				if(!passNotes(1))  //���нű��Ѿ���ȡ���
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
		if(scriptF.eof())      //�ж��Ƿ񵽽�β
		{
			cmdread=endCmd;
//			scriptF.close();
			return false;      //���нű��Ѿ���ȡ���
		}
	}

	if(line[0]=='\"')   //ֱ��echo����
	{
		i=0;   //��ָ��������ͷ,ʹ�û�ȡ�����Ĳ����ӿ�ͷ��ʼ
	}
	else
	{
		//�ű�����ؼ���[cmdstr]�Ľ���         *
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

//�жϲ���������
bool getcmd()
{
	//===========================================================
	//�ж�������������ȡ����ֵ
	if(line[0]=='\"'||!strcmp(cmdstr,"echo"))
	{
		cmdread=echoCmd;
		getStr();
		valnum=1;

		if(hasnext())
		{
			getStr(1);
			valnum++;         //valnum=2,˵��������val 
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
			valnum++;         //valnum=2,˵��������val
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
		scriptF.open(strval[0],ios::in);      //��ȡ��һ���ű��ļ�   *
		i=0;
		deleteAllReturn();
		getAllPos();
		return read();
	}
	//===========================================================
	else if(!strcmp(cmdstr,"btn"))
	{
		//����btn
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
		//���btn
		getInt(0);
		cmdread = clbtnCmd;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"event"))
	{
		//�����¼�
		getPath(0);
		hasnext();
		strcpy_s(strval[1],getName(PosNameLen));
		cmdread = eventCmd;
	}
	//===========================================================
	else if(!strcmp(cmdstr,"lock"))
	{
		//����״̬,(ֻ��ͨ��event�������¼���)
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

//��ȡ������ת��(��������)
void getAllPos()
{
	delallpos();

	while(1)
	{
		scriptF.getline(line,Buffer);
		if(scriptF.eof())
			break;

		//����ע�ͷ�
		if(line[0]=='\\'&&line[1]=='*')
			passNotes(0);

		if(line[0]==':'||line[0]=='*')
		{
			i++;

			//������ڵ�
			char *posname = getName(PosNameLen);

			//����ڵ����ƴ����б�
			nowpos = scriptF.tellg();
			if(line[0]==':')
				addpos(posname,nowpos);
			if(line[0]=='*')
				addpos(posname,nowpos,true);

			delete[] posname;
			i=0;
		}
	}

	//��������ļ�������־�Ļ���Ҫ���ָ����Ϣ�ſ������¶�λ���ļ�ͷ
	scriptF.clear();
	scriptF.seekg(0);

	nowpos = scriptF.tellg();
}

//��ȡ�ı�(echo)
void getStr(int num)   //������ֹʱ�ĵ�ַ
{
	int i2=0;

	while(line[i]==' ')
		i++;

	while(++i)                         //��һ��ʵ�����Ѿ������˿�ͷ�� "
	{
		if(line[i]=='\"' && line[i-1]!='\\')
		{
			strval[num][i2] = '\0';
			i++;
			break;
		}
		//˫�����ڵĴ���
		else
		{
			if(line[i]=='\\')               //ת����ŵĴ���:\",\n
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

//��ȡ����
void getInt(int num)
{
	char str[10];
	int i2=0;
	bool negative=false;    //�ж��Ƿ���

	while(line[i]==' ')
		i++;

	//�ж��Ƿ��и���
	if(line[i]=='-')
	{
		i++;
		negative = true;
	}

	//�ж������ֻ��Ǳ���
	if(isdigit(line[i]))
	{
		while(isdigit(line[i]))    //��������ֵĻ�
		{
			str[i2] = line[i];
			i++;
			i2++;
		}
		str[i2] = '\0';
		intval[num] = atoi(str);     //ת��Ϊ����

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

//��ȡ·���е�����
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
		while(isdigit(line[i]))    //��������ֵĻ�
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

//��ȡ·��
void getPath(int num)
{
	int i2=0;

	//���ԭ������Ϊ0
	for(int i3=0;i3<4;i3++)
		intpath[num][i3]=0;

	//�����ո񲿷�
	while(line[i]==' ')
		i++;

	//�ж��Ƿ���Ҫʹ��maskģʽ
	if(line[i]=='m')
	{
		mask[num] = true;
		i++;
	}
	else
		mask[num] = false;

	while(++i)                         //��һ��ʵ�����Ѿ������˿�ͷ�� "
	{
		if(line[i]=='\"' && line[i-1]!='\\')
		{
			strval[num][i2] = '\0';
			i++;
			break;
		}
		//˫�����ڵĴ���
		else
		{
			if(line[i]==':')               //ת����ŵĴ���  :
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

//��ú�����,��ת�����ֻ��������
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

	//�����ָ�����
	//i++;

	return name;
}

//�Ƿ���"," ���Ƿ񻹴�����һ����ֵ
bool hasnext()
{
	int i2;
	if(line[i]==13)
		return false;

	while(line[i]==' ')
		i++;

	if(line[i]==',')
	{
		i++;           //�������� ,
		return true;
	}
	else if(line[i]!='\0')
		return true;
	else
		return false;
}

//����ע�ͺͺ���
bool passNotes(int what)
{
	scriptF.getline(line,Buffer);

	if(what==0)
	{
		while(line[0]!='*'||line[1]!='/')
		{
			scriptF.getline(line,Buffer);
			if(scriptF.eof())      //�ж��Ƿ񵽽�β
			{
				return false;      //���нű��Ѿ���ȡ���,��δ����ע�ͽ�����
			}
		}
	}
	else
	{
		while(line[0]!='}')
		{
			scriptF.getline(line,Buffer);
			if(scriptF.eof())      //�ж��Ƿ񵽽�β
			{
				return false;      //���нű��Ѿ���ȡ���,��δ����ע�ͽ�����
			}
		}
	}

	return true;
}

//�������ʽ����
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
	//�����������λ��
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

//�ж����
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
	//�����������λ��
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

	//�жϷ���
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