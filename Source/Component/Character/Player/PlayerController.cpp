#include "PlayerController.h"

#include "System/Input/Input.h"

#include "Camera/Camera.h"

PlayerContorller::PlayerContorller(int slot)
{
	gamePad.SetSlot(slot);
}

void PlayerContorller::Update()
{
	gamePad.Update();

	// �o�^����Ă���L�[�̌o�߃t���[�����v�Z
	for (auto& data : keyPool){
		++data.frame;
	}

	// ���t���[���o�������͏����폜
	// std::remove_if  : �C�e���[�^�[�͈͂�������𖞂����v�f���폜����
	// ���R���e�i�̗v�f���͍폜����Ȃ��̂�keyPool.end()��t���邱�ƂŁA�v�f�����폜����
	auto eraseKey = std::remove_if(keyPool.begin(), keyPool.end(),
		[](KeyData& data) {return data.frame >= saveFrame; });

	keyPool.erase(eraseKey, keyPool.end());

	// �R���g���[���[�擾
	bool buttonY  = (gamePad.GetButtonDown() & GamePad::BTN_Y) != 0;
	bool buttonX  = (gamePad.GetButtonDown() & GamePad::BTN_X) != 0;
	bool buttonA  = (gamePad.GetButtonDown() & GamePad::BTN_A) != 0;
	bool buttonB  = (gamePad.GetButtonDown() & GamePad::BTN_B) != 0;

	InputKey key = 0;
	if(buttonY)  key |= Key::KeyY;
	if(buttonX)	 key |= Key::KeyX;
	if(buttonA)  key |= Key::KeyA;
	if(buttonB)	 key |= Key::KeyB;
	if(buttonY && buttonB) key |= Key::KeyYB;

	// �d�����Ă���L�[���O��
	if (key & Key::KeyYB) key &= ~(Key::KeyY | Key::KeyB);

	// �L�[��o�^
	RegisterKey(key);
}

// �����ꂽ�L�[��o�^
void PlayerContorller::RegisterKey(const InputKey& key)
{
	if (key == 0) return;

	KeyData& data = keyPool.emplace_back();
	data.key      = key;
	data.frame    = 0;
}

// ��s����
bool PlayerContorller::GetKeyPrecede(const InputKey& key, int frame)
{
	// std::find_if : �C�e���[�^�[�͈͂�������𖞂����v�f����������
	auto precedeKey = std::find_if(keyPool.begin(), keyPool.end(),
		[key, frame](KeyData& data) {return data.key == key && data.frame < frame; });

	// �����𖞂������v�f������� true ��Ԃ�
	if (precedeKey != keyPool.end())
	{
		keyPool.erase(precedeKey);
		return true;
	}
	
	return false;
}

const DirectX::XMFLOAT3& PlayerContorller::GetMoveVec() const
{
	// ���͏����擾
	// �Q�[���p�b�h���擾
	GamePad& gamePad = Input::Instance().GetGamePad();
	// ���X�e�B�b�N�̓��͏����擾
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	// ���K�����ē��͂������擾�ł���悤�ɂ���
	float l = gamePad.GetAxisLStick();

	if (l != 0.0f) {
		ax /= l;
		ay /= l;
	}

	// �J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
	Camera& camera = Camera::Instance();
	DirectX::XMFLOAT3 cameraRight = camera.GetRight();  // �J�����̉E���������o��    
	DirectX::XMFLOAT3 cameraFront = camera.GetFront();  // �J�����̑O���������o��
	// �� &�ɂ��Ă����Ə����������Ȃ邪���g��������������ƍ���̂ŁA
	//    const������

	// �P�ʃx�N�g����
	DirectX::XMVECTOR Right = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&cameraRight));
	DirectX::XMVECTOR Front = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&cameraFront));

	DirectX::XMStoreFloat3(&cameraRight, Right);
	DirectX::XMStoreFloat3(&cameraFront, Front);

	// �i�s�x�N�g�����v�Z����
	DirectX::XMFLOAT3 vec{};
	vec.x = cameraFront.x * ay + cameraRight.x * ax;  // �X�e�B�b�N�̐������͒l���J�����E�����ɔ��f
	vec.z = cameraFront.z * ay + cameraRight.z * ax;  // �X�e�B�b�N�̐������͒l���J�����O�����ɔ��f

	// Y�������ɂ͈ړ����Ȃ�
	vec.y = 0.0f;

	return vec;
}