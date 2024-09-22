#pragma once
#include<DirectXMath.h>
#include<vector>
#include<map>

enum class MessageType
{
	M_ApplicationAreaMove,  // エリア移動申請
	M_PermitAreaMove,		// エリア移動許可
};

struct Telegram
{
	Telegram(const int sender, const int receiver, MessageType type) 
		: sender(sender)
		, receiver(receiver)
		, type(type) {}

	int sender;        // 送り主
	int receiver;	   // 受け取り主
	MessageType type;  // メッセージのタイプ
};

class MetaAI final
{
private:
	MetaAI()  = default;
	~MetaAI() = default;
public:
	MetaAI(const MetaAI&) = delete;
	MetaAI(MetaAI&&)	  = delete;
	MetaAI operator=(const MetaAI&) = delete;
	MetaAI operator=(MetaAI&&)		= delete;

public:
	// データを取得するための構造体
	struct AreaData
	{
		int areaNum;
		DirectX::XMFLOAT3 areaPos;
		float areaRange;

		template<class Archive>
		void serialize(Archive& archive);
	};

	// エリアの情報
	struct UseAreaData
	{
		DirectX::XMFLOAT3 areaPos;
		float areaRange;
	};

public:
	enum class Member : int
	{
		MetaAI,  // 0 : メタAI
		Player,  // 1 : プレイヤー
		Enemy,   // 2 : 敵
	};

public:
	// メタAIは一つだけ実装するのでインスタンス化
	static MetaAI& Instance()
	{
		static MetaAI instance;
		return instance;
	}

	// 初期化
	void Initialize();

	// 常に監視するものがあれば更新を行う
	void Update(const float& elapsedTime) {}

	// メッセージを受信する
	bool OnMessage(const Telegram& telegram);

	// メッセージ送信
	void SendMessaging(const Telegram& telegram);

private:
	void ImportStageData();

public:
#pragma region セッター ゲッター
	UseAreaData GetAreaData(const int areaNum) { return areaDataPool[areaNum]; }
#pragma endregion

private:
	std::map<int, UseAreaData> areaDataPool;
	std::vector<AreaData> areaDatas;
};
