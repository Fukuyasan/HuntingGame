#pragma once

#include "Audio\Audio.h"
#include "UI/MiniMap/MiniMap.h"
#include <string>

class GameManager
{
public:
	struct QuestData
	{
		std::string monsterName;
		int huntingCount;
		template<class Archive>
		void serialize(Archive& archive);
	};


private:
	GameManager()  = default;
	~GameManager() = default;

public:
	GameManager(const GameManager&) = delete;
	GameManager(GameManager&&)		= delete;
	GameManager operator=(const GameManager&) = delete;
	GameManager operator=(GameManager&&)	  = delete;

public:
	// �C���X�^���X
	static GameManager& Instance()
	{
		static GameManager instance;
		return instance;
	}

	// ������
	void Initialize();

	// �I����
	void Finalize();

	// �X�V����
	void Update(const float& elapsedTime);

	// �f�o�b�O
	void DrawGUI();
	void Save();

	void ReturnTitle() { returnTitle = true; }

	// UI��\������
	void ShowUI(bool flg) { this->showUI = flg; }
	bool IsShowUI() { return showUI; }

private:
	//std::unique_ptr<MiniMap> miniMap = nullptr;

	bool returnTitle = false;
	bool showUI		 = false;

	QuestData questData;
};
