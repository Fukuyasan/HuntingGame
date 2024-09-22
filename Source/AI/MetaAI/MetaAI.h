#pragma once
#include<DirectXMath.h>
#include<vector>
#include<map>

enum class MessageType
{
	M_ApplicationAreaMove,  // �G���A�ړ��\��
	M_PermitAreaMove,		// �G���A�ړ�����
};

struct Telegram
{
	Telegram(const int sender, const int receiver, MessageType type) 
		: sender(sender)
		, receiver(receiver)
		, type(type) {}

	int sender;        // �����
	int receiver;	   // �󂯎���
	MessageType type;  // ���b�Z�[�W�̃^�C�v
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
	// �f�[�^���擾���邽�߂̍\����
	struct AreaData
	{
		int areaNum;
		DirectX::XMFLOAT3 areaPos;
		float areaRange;

		template<class Archive>
		void serialize(Archive& archive);
	};

	// �G���A�̏��
	struct UseAreaData
	{
		DirectX::XMFLOAT3 areaPos;
		float areaRange;
	};

public:
	enum class Member : int
	{
		MetaAI,  // 0 : ���^AI
		Player,  // 1 : �v���C���[
		Enemy,   // 2 : �G
	};

public:
	// ���^AI�͈������������̂ŃC���X�^���X��
	static MetaAI& Instance()
	{
		static MetaAI instance;
		return instance;
	}

	// ������
	void Initialize();

	// ��ɊĎ�������̂�����΍X�V���s��
	void Update(const float& elapsedTime) {}

	// ���b�Z�[�W����M����
	bool OnMessage(const Telegram& telegram);

	// ���b�Z�[�W���M
	void SendMessaging(const Telegram& telegram);

private:
	void ImportStageData();

public:
#pragma region �Z�b�^�[ �Q�b�^�[
	UseAreaData GetAreaData(const int areaNum) { return areaDataPool[areaNum]; }
#pragma endregion

private:
	std::map<int, UseAreaData> areaDataPool;
	std::vector<AreaData> areaDatas;
};
