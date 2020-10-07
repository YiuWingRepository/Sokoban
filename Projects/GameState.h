#pragma once
#include"Common.h"
#include"Map.h"
#include"GameSetting.h"


//��Ϸ״̬����
class GameState
{
public:
	virtual void Enter(GameSetting*setting) {}
	virtual void Update(GameSetting*setting) {}
	virtual void Exit(GameSetting*setting) {}
	virtual EGameState ChangeState(GameSetting*setting) {
		return GAMESTATE_NONE;
	}
	virtual void Draw(GameSetting*setting) {}
	virtual ~GameState() {
		std::cout << "��Ϸ״̬"<<std::endl;
	}

protected:
	EGameState mStateType;
};

//��Ϸ��ʼ��״̬
class GameStartState :public GameState
{
public:

	GameStartState()
	{
		this->mStateType = GAMESTATE_START;
	}

	virtual void Enter(GameSetting*setting)
	{
		setting->ResetGameSetting();
	}


	virtual void Update(GameSetting*setting)
	{
		switch (setting->gInput)
		{
		case INPUT_UP:
			setting->mapIndex = setting->mapIndex - 1 >= 0 ? setting->mapIndex - 1:0;
			break;
		case INPUT_DOWN:
			setting->mapIndex = setting->mapIndex + 1 < setting->MapDataNum() ? setting->mapIndex + 1 : setting->MapDataNum() - 1;
			break;
		case INPUT_ENTER:
			setting->isGameStart = true;
			break;
		case INPUT_ESC:
			setting->isGameOver = true;
			break;
		}
	}

	virtual void Draw(GameSetting*setting)
	{
		system("cls");
		//���Ƶ�ͼѡ�����
		if (setting == nullptr) return;

		std::cout << "������Sokoban"<<std::endl<<std::endl<<std::endl;
		std::cout << "����ESC���˳���Ϸ"<<std::endl << std::endl;
		std::cout << "��ѡ����Ϸ��ͼ"<<std::endl;

		for (int i = 0; i < setting->MapDataNum(); i++)
		{
			MapData &mapData = setting->mapDatas[i];
			if (i == setting->mapIndex)
			{
				std::cout << mChoosedMark<<"";
			}
			std::cout<<i<<" : "<< mapData.mapName<<std::endl;
		}
	}


	virtual void Exit(GameSetting*setting)
	{

	}

	virtual EGameState ChangeState(GameSetting*setting)
	{
		if (setting->isGameStart) return GAMESTATE_PLAYING;
		return GAMESTATE_NONE;
	}


	virtual ~GameStartState() 
	{
		
		
	}

protected:
	std::string mChoosedMark = "->";
};


//��Ϸ�����е�״̬
class GamePlayState :public GameState
{
public:
	GamePlayState()
	{
		this->mStateType = GAMESTATE_PLAYING;
	}

	virtual void Enter(GameSetting*setting)
	{
		setting->gameMap.DestoryMap();
		setting->SetGameMap();
	}

	virtual void Update(GameSetting*setting)
	{

		if (setting->gInput == INPUT_RESET)
		{
			//�������õ�ͼ
			setting->gameMap.DestoryMap();
			setting->SetGameMap();
		}

		auto& gameMap = setting->gameMap;
		Position playerPosition = gameMap.PlayerPos();
		Position moveDir = moveDirs[(int)setting->gInput];
		Position desiredPlayerPos = playerPosition + moveDir;

		if (setting->isGameWin) {
			setting->isGameOver = true;
			return;
		}

		if (gameMap.IsOutsideMap(desiredPlayerPos)) return;

		if (gameMap.IsWall(desiredPlayerPos))
		{
			desiredPlayerPos = playerPosition;
		}
		else if (gameMap.IsBox(desiredPlayerPos))
		{
			Position desiredBoxPos = desiredPlayerPos + moveDir;

			//�ҵ����һ�����ӣ������ƶ�һ�����ӵ����
			while (gameMap.IsBox(desiredBoxPos))
			{
				desiredBoxPos = desiredBoxPos + moveDir;
			}

			if (desiredBoxPos != desiredPlayerPos && gameMap.IsWalkArea(desiredBoxPos))
			{
				while (desiredBoxPos != desiredPlayerPos)
				{
					gameMap.MoveInMap(desiredBoxPos - moveDir, desiredBoxPos);
					desiredBoxPos = desiredBoxPos - moveDir;
				}

			}
			else
				desiredPlayerPos = playerPosition;


		}
		gameMap.MoveInMap(playerPosition, desiredPlayerPos);
		gameMap.SetPlayerPos(desiredPlayerPos);
		
		//�ж�һ�����������Ƿ�������Ϸ����
		IsGameWin(setting);
	}

	virtual void Draw(GameSetting*setting)
	{
		system("cls");
		setting->gameMap.Show();
	}

	virtual void Exit(GameSetting*setting)
	{

	}

	virtual EGameState ChangeState(GameSetting*setting)
	{
		if (setting->isGameWin || setting->isGameOver) return GAMESTATE_END;

		if (setting->gInput == INPUT_ESC)
		{
			return GAMESTATE_START;
		}

		return GAMESTATE_NONE;
	}

	virtual ~GamePlayState() {
	
		
	}

protected:
	void IsGameWin(GameSetting*setting)
	{
		auto targetPositions = setting->gameMap.GetTargetPosition();
		int count = 0;

		for (int i = 0; i < targetPositions.size(); i++)
		{
			if (!setting->gameMap.IsOutsideMap(targetPositions[i]))
			{
				if (setting->gameMap.GetChunckType(targetPositions[i]) == CHUNCK_BOX)
				{
					count++;
				}
			}
		}
		setting->isGameWin = count == targetPositions.size();
	}

};

//��Ϸ������״̬
class GameEndState :public GameState
{
public:
	GameEndState()
	{
		this->mStateType = GAMESTATE_START;
	}


	virtual void Enter(GameSetting*setting)
	{

	}

	virtual void Draw(GameSetting*setting)
	{
		system("cls");

		if (setting->isGameWin)
			std::cout << std::endl << "��Ϸʤ������������������������" << std::endl;
		else
			std::cout << std::endl << "��Ϸʧ�ܣ���������������������"<<std::endl;
	}

	virtual void Update(GameSetting*setting)
	{
		if (setting->gInput == INPUT_ENTER)
		{
			//��Ұ��»س�
			setting->isGameStart = false;
		}
	}

	virtual void Exit(GameSetting*setting)
	{

	}

	virtual EGameState ChangeState(GameSetting*setting)
	{
		if (!setting->isGameStart)
		{
			return GAMESTATE_START;
		}

		return GAMESTATE_NONE;
	}

	virtual ~GameEndState() {}
};






