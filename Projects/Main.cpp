
#include"Common.h"
#include"Map.h"
#include"GameState.h"
#include <windows.h>
#include<fstream>
#include<io.h>
#include"GameSetting.h"
using namespace std;

GameSetting setting;


void InitGame();//��ʼ����Ϸ
void GetInput();//��ȡ����
void UpdateLogic();//������Ϸ�߼�
void Draw();//����
void DestoryGame();//���٣��ͷ���Ϸ��Դ

void ReadMapInfos(string path, vector<MapData>& mapInfos);//��ȡ��Ϸ�Ĺؿ���Ϣ
void ReadMapStr(const char* path,MapData&data);
bool IsRightMap(MapData&data);//�ж�һ����ͼ�����Ƿ���ȷ
bool IsTxtFile(const char* fileName,int len,string&res);


//����װ�ĺ���

int main()
{
	InitGame();
	while (!setting.isGameOver)
	{
		GetInput();
		UpdateLogic();
		Draw();
	}
	DestoryGame();
}


void InitGame()
{
	ReadMapInfos(LEVEL_PATH, setting.mapDatas);//��ȡ��ͼ����

	ChunckTypeMap[CHUNCK_BOX] = MARK_BOX;
	ChunckTypeMap[CHUNCK_EMPTY] = MARK_EMPTY;
	ChunckTypeMap[CHUNCK_PLAYER] = MARK_PLAYER;
	ChunckTypeMap[CHUNCK_WALL] = MARK_WALL;
	ChunckTypeMap[CHUNCK_TARGET] = MARK_TAEGET;

	setting.states[GAMESTATE_NONE] = new GameState();
	setting.states[GAMESTATE_START] = new GameStartState();
	setting.states[GAMESTATE_PLAYING] = new GamePlayState();
	setting.states[GAMESTATE_END] = new GameEndState();

	setting.prevState = setting.currentState = GAMESTATE_START;
}

void GetInput()
{
	char input = '@';
	setting.gInput = INPUT_NONE;


	if (_kbhit())
	{
		input = _getch();
	}
	
	switch (input)
	{
	case 'a':
		//left
		setting.gInput = INPUT_LEFT;
		break;
	case 'd':
		//right
		setting.gInput = INPUT_RIGHT;
		break;
	case 'w':
		//up
		setting.gInput = INPUT_UP;
		break;
	case 's':
		//down
		setting.gInput = INPUT_DOWN;
		break;
	case '\r':
		setting.gInput = INPUT_ENTER;
		break;
	case 'r':
		setting.gInput = INPUT_RESET;
		break;
	case 27:
		setting.gInput = INPUT_ESC;
		break;
	}
}

void UpdateLogic()
{
	auto state = setting.states[setting.currentState];
	if (setting.prevState != setting.currentState)
	{
		//�˳���״̬,�����µ�״̬
		setting.states[setting.prevState]->Exit(&setting);
		state->Enter(&setting);
		setting.prevState = setting.currentState;
	}
	else
	{
		setting.prevState = setting.currentState;
		state->Update(&setting);
		EGameState targetState = state->ChangeState(&setting);

		if (targetState != GAMESTATE_NONE)
		{
			setting.currentState = targetState;
		}
	}
}

void Draw()
{
	auto state = setting.states[setting.currentState];
	state->Draw(&setting);
}

void DestoryGame()
{
}

void ReadMapInfos(string src,vector<MapData>& mapInfos)
{
	intptr_t hFile = 0;
	string fileName;
	string folderPath = src+ "\\*";
	string filepath;
	struct _finddata_t fileinfo;

	if ((hFile = _findfirst(folderPath.c_str(),&fileinfo)) != -1)
	{
		//�ɹ��ҵ�
		do
		{
			fileName = "";
			if (!(fileinfo.attrib&_A_NORMAL)&&!(fileinfo.attrib&_A_HIDDEN)&&IsTxtFile(fileinfo.name,strlen(fileinfo.name),fileName))
			{
				MapData data;
				data.mapName = fileName;
				filepath = src+"\\"+fileinfo.name;
				ReadMapStr(filepath.c_str(),data);

				if (IsRightMap(data))
				{

					setting.mapDatas.push_back(data);
				}
			}
		} while (_findnext(hFile, &fileinfo) == 0);

		//�رվ��
		_findclose(hFile);
	}
}

void ReadMapStr(const char * path, MapData &data)
{
	ifstream inputFile(path,ifstream::binary);
	inputFile.seekg(0,ifstream::end);
	//һ������µ�ͼ�Ĵ�С�����ᳬ��int���͵ķ�Χ
	int fileSize = static_cast<int>(inputFile.tellg());
	inputFile.seekg(0,ifstream::beg);

	if (fileSize > 0)
	{
		char *strBuffer = new char[fileSize];
		inputFile.read(strBuffer,fileSize);
		for (int i = 0,j = 0; i < fileSize; i++)
		{
			if (strBuffer[i] != '\r')
			{
				if (data.map.size() <= j) data.map.push_back("");
				data.map[j] += strBuffer[i];
			}
			else {
				i++;
				j++;
			}
		}

		delete[]strBuffer;
	}
	else
	{
		//TODO ������̫����
	}

}

bool IsRightMap(MapData &data)
{
	//��������Ƿ��б߽磬�Լ����ӵ�������Ŀ��������Ƿ�һ��
	if (data.map.size() == 0)
		return false;


	int r = data.map.size();//��ͼ�м���
	int c = data.map[0].size();//��ͼ�м���
	int boxNum = 0, targetNum = 0;

	for (int j = 0; j < c; j++)
	{
		if (data.map[0][j] != MARK_WALL || data.map[r - 1][j] != MARK_WALL) return false;
	}

	for (int i = 0; i < r; i++)
	{
		if (data.map[i][0] != MARK_WALL || data.map[i][c - 1] != MARK_WALL) return false;
	}


	//������ӵ�������Ŀ��������Ƿ�һ��

	for (int i = 0; i < r; i++)
	{
		for (int j = 0; j < c; j++)
		{
			if (data.map[i][j] == MARK_BOX) boxNum++;
			if (data.map[i][j] == MARK_TAEGET) targetNum++;
		}
	}

	return boxNum == targetNum;


}

bool IsTxtFile(const char * fileName, int len, string & res)
{
	if (len == 0) return false;

	vector<string> tmp(1,"");

	for (int i = 0,j = 0; i < len; i++)
	{
		if (fileName[i] != '.')
			tmp[j] += fileName[i];
		else {
			j++;
			tmp.push_back("");
		}
	}

	//����ļ���
	for (int i = 0; i < tmp.size() - 1; i++)
	{
		res += tmp[i];
		if(i!= tmp.size()-2)
			res += '.';
	}

	return tmp[tmp.size() - 1] == MAPFILETYPE;
}
