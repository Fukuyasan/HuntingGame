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

	// 登録されているキーの経過フレームを計算
	for (auto& data : keyPool){
		++data.frame;
	}

	// 一定フレーム経った入力情報を削除
	// std::remove_if  : イテレーター範囲から条件を満たす要素を削除する
	// ※コンテナの要素数は削除されないのでkeyPool.end()を付けることで、要素数を削除する
	auto eraseKey = std::remove_if(keyPool.begin(), keyPool.end(),
		[](KeyData& data) {return data.frame >= saveFrame; });

	keyPool.erase(eraseKey, keyPool.end());

	// コントローラー取得
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

	// 重複しているキーを外す
	if (key & Key::KeyYB) key &= ~(Key::KeyY | Key::KeyB);

	// キーを登録
	RegisterKey(key);
}

// 押されたキーを登録
void PlayerContorller::RegisterKey(const InputKey& key)
{
	if (key == 0) return;

	KeyData& data = keyPool.emplace_back();
	data.key      = key;
	data.frame    = 0;
}

// 先行入力
bool PlayerContorller::GetKeyPrecede(const InputKey& key, int frame)
{
	// std::find_if : イテレーター範囲から条件を満たす要素を検索する
	auto precedeKey = std::find_if(keyPool.begin(), keyPool.end(),
		[key, frame](KeyData& data) {return data.key == key && data.frame < frame; });

	// 条件を満たした要素があれば true を返す
	if (precedeKey != keyPool.end())
	{
		keyPool.erase(precedeKey);
		return true;
	}
	
	return false;
}

const DirectX::XMFLOAT3& PlayerContorller::GetMoveVec() const
{
	// 入力情報を取得
	// ゲームパッドを取得
	GamePad& gamePad = Input::Instance().GetGamePad();
	// 左スティックの入力情報を取得
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	// 正規化して入力だけを取得できるようにする
	float l = gamePad.GetAxisLStick();

	if (l != 0.0f) {
		ax /= l;
		ay /= l;
	}

	// カメラ方向とスティックの入力値によって進行方向を計算する
	Camera& camera = Camera::Instance();
	DirectX::XMFLOAT3 cameraRight = camera.GetRight();  // カメラの右方向を取り出す    
	DirectX::XMFLOAT3 cameraFront = camera.GetFront();  // カメラの前方向を取り出す
	// ※ &にしておくと処理が早くなるが中身を書き換えられると困るので、
	//    constをつける

	// 単位ベクトル化
	DirectX::XMVECTOR Right = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&cameraRight));
	DirectX::XMVECTOR Front = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&cameraFront));

	DirectX::XMStoreFloat3(&cameraRight, Right);
	DirectX::XMStoreFloat3(&cameraFront, Front);

	// 進行ベクトルを計算する
	DirectX::XMFLOAT3 vec{};
	vec.x = cameraFront.x * ay + cameraRight.x * ax;  // スティックの水平入力値をカメラ右方向に反映
	vec.z = cameraFront.z * ay + cameraRight.z * ax;  // スティックの垂直入力値をカメラ前方向に反映

	// Y軸方向には移動しない
	vec.y = 0.0f;

	return vec;
}