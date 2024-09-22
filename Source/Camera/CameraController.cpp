#include "CameraController.h"
#include "Camera.h"
#include "System/Input/Input.h"
#include "System/Collision/Collision.h"

#include "System/Input/GamePad.h"
#include "System/Math/Easing.h"

#include <imgui.h>
#include <cmath>

#include "Component/Character/Player/player.h"

#define DEDUG_CAMERA 0

CameraController* CameraController::instance = nullptr;

CameraController::CameraController(bool noOwnerCamera)
{
	instance = this;

	// オーナーを設定するか
	this->noOwnerCamera = noOwnerCamera;
	if (!this->noOwnerCamera) return;

	Camera& camera = Camera::Instance();

	DirectX::XMVECTOR Eye    = DirectX::XMLoadFloat3(&camera.GetEye());
	DirectX::XMVECTOR Focus  = DirectX::XMLoadFloat3(&camera.GetFocus());
	DirectX::XMVECTOR Vec    = DirectX::XMVectorSubtract(Eye, Focus);
	DirectX::XMVECTOR Length = DirectX::XMVector3Length(Vec);
	DirectX::XMStoreFloat(&distance, Length);

	DirectX::XMFLOAT3 vec;
	DirectX::XMStoreFloat3(&vec, DirectX::XMVectorDivide(Vec, Length));

	float f = sqrtf(vec.x * vec.x + vec.z * vec.z);

	rotateX = atan2f(vec.y, f);
	rotateY = atan2f(vec.x, vec.z);
}

CameraController::~CameraController()
{
	raycastModels.clear();
}

// 更新処理
void CameraController::Update(const float& elapsedTime)
{
	if(noOwnerCamera)
	{
		// オーナーが設定されてない場合(主にデバッグで使用する)
		D_CameraMoveUsedGamePad(elapsedTime);
		D_CameraMoveUsedMouse();
		return;
	}
	else
	{
		if (owner.lock())
		{
			// オーナーの設定
			ownerPos	= owner.lock()->GetTransform().GetPosition();
			ownerPos.y += 1.3f + cameraOffset;
		}

		if (target.lock())
		{
			// ターゲットの設定
			targetPos	 = target.lock()->GetTransform().GetPosition();
			//targetPos.y += target->GetHeight();

		}
		
		// オーナーとのカメラ距離を変更
		ComputeRange(elapsedTime);

		// ゲームパッドを使用したときのカメラの挙動
		CameraMoveUsedGamePad(elapsedTime);
	}

	

	// 線形保管
	currentEye.x = Mathf::lerp(currentEye.x, eye.x, lerpValue);
	currentEye.y = Mathf::lerp(currentEye.y, eye.y, lerpValue);
	currentEye.z = Mathf::lerp(currentEye.z, eye.z, lerpValue);

	currentFocus.x = Mathf::lerp(currentFocus.x, ownerPos.x, lerpValue);
	currentFocus.y = Mathf::lerp(currentFocus.y, ownerPos.y, lerpValue);
	currentFocus.z = Mathf::lerp(currentFocus.z, ownerPos.z, lerpValue);

	// カメラシェイク
	ComputeShake(currentFocus, shakeData, elapsedTime);

	// レイキャスト
	CameraRaycast(currentFocus, currentEye, currentEye);

	// カメラの視点と注視点を設定
	Camera::Instance().SetLookAt(currentEye, currentFocus, DirectX::XMFLOAT3(0, 1, 0));
}

void CameraController::CameraMoveUsedGamePad(const float& elapsedTime)
{
	if (performFlg) return;
	
	if (lockOn)
	{
		// ロックオンカメラ
		TargetCamera(elapsedTime);
	}
	else
	{
		// ターゲットカメラの回転が終わったら
		if (targetRate >= 1.0f)
		{
			// 通常のカメラ
			NormalCameraMove(elapsedTime);
		}
		else
		{
			// ターゲットカメラ
			TargetCamera(elapsedTime);
		}
	}

}

void CameraController::NormalCameraMove(const float& elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisRX();
	float ay = gamePad.GetAxisRY();

	// カメラの回転速度
	float speed = rollSpeed * elapsedTime;

	// スティックの入力値に合わせてX軸とY軸を回転
	angle.x += -ay * speed;
	angle.y +=  ax * speed;

	// X軸のカメラ回転を制限
	if (angle.x > maxAngle) { angle.x = maxAngle; }
	if (angle.x < minAngle) { angle.x = minAngle; }

	// Y軸の回転値を-3.14～3.14に収まるようにする
	if (angle.y < -DirectX::XM_PI) { angle.y += DirectX::XM_2PI; }
	if (angle.y > DirectX::XM_PI)  { angle.y -= DirectX::XM_2PI; }

	// カメラ回転値を回転行列に変換
	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(
		angle.x,
		angle.y,
		angle.z
	);

	// 回転行列から前方向ベクトルを取り出す
	DirectX::XMVECTOR Front = Transform.r[2];
	DirectX::XMStoreFloat3(&front, Front);

	// 注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
	eye.x = ownerPos.x + front.x * -range;
	eye.y = ownerPos.y + front.y * -range;
	eye.z = ownerPos.z + front.z * -range;
}

// カメラのレイキャスト
void CameraController::CameraRaycast(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, DirectX::XMFLOAT3& result)
{
	float distance = FLT_MAX;

	// 登録したモデルとレイキャスト
	for (auto model : raycastModels)
	{
		HitResult hit;
		if (Collision::IntersectRayVsModel(
			start,
			end, 
			&model,
			hit))
		{
			if (hit.distance < distance)
			{
				result   = hit.position;
				distance = hit.distance;
			}
		}
	}
}

// ターゲットカメラ
void CameraController::TargetCamera(const float& elapsedTime)
{
	DirectX::XMVECTOR Owner  = DirectX::XMLoadFloat3(&ownerPos);   // プレイヤー
	DirectX::XMVECTOR Target = DirectX::XMLoadFloat3(&targetPos);  // 敵
	DirectX::XMVECTOR Eye    = DirectX::XMLoadFloat3(&eye);

	// ターゲットとオーナーのベクトルを求める(正規化)
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Target, Owner);
	Vec = DirectX::XMVector3Normalize(Vec);

	// オーナーから range 離れた場所に仮のカメラ位置を設定
	Vec = DirectX::XMVectorScale(Vec, range * -1);

	// オーナーから現在のカメラの位置へのベクトルを求める
	DirectX::XMVECTOR Owner_Eye = DirectX::XMVectorSubtract(Eye, Owner);

	// ラープ
	DirectX::XMVECTOR Lerp = DirectX::XMVectorLerp(Owner_Eye, Vec, 1.0f);
	targetRate += elapsedTime * 3.0f;

	DirectX::XMVECTOR L = DirectX::XMVectorAdd(Owner, Lerp);

	// 計算結果を代入
	DirectX::XMStoreFloat3(&eye, L);

	// angle を回転させる
	DirectX::XMFLOAT3 vec;
	DirectX::XMStoreFloat3(&vec, Vec);

	float f = sqrtf(vec.x * vec.x + vec.z * vec.z);

	// atan2f(x, y) : y/xの逆正接の値を計算する。
	angle.x   = atan2f(vec.y, f);
	angle.y   = atan2f(-vec.x, -vec.z);
}

// カメラの引き、寄せ処理
void CameraController::ComputeRange(const float& elapsedTime)
{
	rangeData.rangeTime += elapsedTime;

	switch (rangeData.rangeMode)
	{
	case RangeMode::Range_First:
		// 真ん中処理
		performFlg = false;
		range = middleRange;

		break;
	case RangeMode::Range_Middle:
		// 真ん中処理
		range = Mathf::lerp(farRange, middleRange, (std::min)(1.0f, rangeData.rangeTime * 2.0f));

		break;
	case RangeMode::Range_Far:
		// 引きの処理
		range = Mathf::lerp(middleRange, farRange, (std::min)(1.0f, rangeData.rangeTime * 2.0f));

		if (rangeData.rangeTime >= rangeData.rangeLength)
		{
			rangeData.rangeTime = 0.0f;
			rangeData.rangeMode = RangeMode::Range_Middle;
		}

		break;
	case RangeMode::Range_Close:
		// 寄せ処理
		range = Mathf::lerp(middleRange, closeRange, (std::min)(1.0f, rangeData.rangeTime * 2.0f));

		if (rangeData.rangeTime >= rangeData.rangeLength)
		{
			rangeData.rangeTime = 0.0f;
			rangeData.rangeMode = RangeMode::Range_Middle;
		}

		break;
	case RangeMode::Range_Perform:
		// 寄せ処理
		range = 20.0f;
		performFlg = true;

		break;
	}
}

// カメラシェイク
void CameraController::ComputeShake(DirectX::XMFLOAT3& focus, const ShakeData& data, const float& elapsedTime)
{
	if (data.shakeTime <= 0.0f) return;
	if (shaketotalTime > data.shakeTime) return;

	shaketotalTime += elapsedTime;

	float raito  = 1.0f - Easing::OutQuart(shaketotalTime, data.shakeTime);
	float powerX = data.power.x * raito;
	float powerY = data.power.y * raito;

	// 揺れがない場合 return
	if (powerX == 0.0f && powerY == 0.0f) return;

	float randomX = Mathf::RandomRange(-powerX, powerX);
	float randomY = Mathf::RandomRange(-powerY, powerY);
		
	DirectX::XMFLOAT3 position = focus;
	position.x += randomX * elapsedTime;
	position.y += randomY * elapsedTime;

	focus = position;
}

// クリア演出
bool CameraController::ClearPerform(const float& elapsedTime)
{
	// カメラのアングルが4秒ごとに切り替わるようにする
	changeTime += elapsedTime;
	if (changeTime > 4.0f)
	{
		float theta = Mathf::RandomRange(-DirectX::XM_PI, DirectX::XM_PI);
		float range = 13.0f;

		eye.x = ownerPos.x + sinf(theta)  * range;
		eye.y = ownerPos.y + 10.0f;
		eye.z = ownerPos.z + cosf(theta)  * range;

		changeTime = 0.0f;
	}

#if 0 // 演出後自由に動けるようにするなら
	// 時間経過で終了
	performData.performTime += elapsedTime;
	if (performData.performTime > performData.PerformLength)
	{
		clearPerform = false;
		return true;
	}

	return false;
#else // クリア後タイトルに戻すので
	performData.performTime += elapsedTime;
	return performData.performTime > performData.PerformLength;
#endif
}

#pragma region デバッグ用

void CameraController::D_CameraMoveUsedMouse()
{
	Mouse& mouse = Input::Instance().GetMouse();

	float moveX = (mouse.GetPositionX() - mouse.GetOldPositionX()) * 0.02f;
	float moveY = (mouse.GetPositionY() - mouse.GetOldPositionY()) * 0.02f;

	Camera& camera = Camera::Instance();
	// 視線行列を生成
	DirectX::XMMATRIX V;
	{
		DirectX::XMVECTOR up{ DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };
		// マウス操作
		{
			if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
			{
				// Y軸回転
				rotateY += moveX * 0.5f;
				if (rotateY > DirectX::XM_PI)
					rotateY -= DirectX::XM_2PI;
				else if (rotateY < -DirectX::XM_PI)
					rotateY += DirectX::XM_2PI;
				// X軸回転
				rotateX += moveY * 0.5f;
				if (rotateX > DirectX::XMConvertToRadians(89.9f))
					rotateX = DirectX::XMConvertToRadians(89.9f);
				else if (rotateX < -DirectX::XMConvertToRadians(89.9f))
					rotateX = -DirectX::XMConvertToRadians(89.9f);
			}
			else if (GetAsyncKeyState(VK_MBUTTON) & 0x8000)
			{
				V = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&debugPosition),
					DirectX::XMLoadFloat3(&debugFocus),
					up);
				DirectX::XMFLOAT4X4 W;
				DirectX::XMStoreFloat4x4(&W, DirectX::XMMatrixInverse(nullptr, V));
				// 平行移動
				const float offset = 0.035f;
				float s = distance * offset;
				float x = moveX * s;
				float y = moveY * s;
				debugFocus.x -= W._11 * x;
				debugFocus.y -= W._12 * x;
				debugFocus.z -= W._13 * x;

				debugFocus.x += W._21 * y;
				debugFocus.y += W._22 * y;
				debugFocus.z += W._23 * y;
			}
			if (mouse.GetWheel() != 0)	// ズーム
			{
				const float offset = 0.0001f;
				distance -= static_cast<float>(mouse.GetWheel()) * distance * offset;
			}
		}
		float sx = sinf(rotateX), cx = cosf(rotateX);
		float sy = sinf(rotateY), cy = cosf(rotateY);
		DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&debugFocus);
		DirectX::XMVECTOR Front = DirectX::XMVectorSet(-cx * sy, -sx, -cx * cy, 0.0f);
		DirectX::XMVECTOR Distance = DirectX::XMVectorSet(distance, distance, distance, 0.0f);
		Front = DirectX::XMVectorMultiply(Front, Distance);
		DirectX::XMVECTOR Eye = DirectX::XMVectorSubtract(Focus, Front);
		DirectX::XMStoreFloat3(&debugPosition, Eye);
		// カメラに視点を注視点を設定
		Camera::Instance().SetLookAt(debugPosition, debugFocus, { 0, 1, 0 });
	}

}

void CameraController::D_CameraMoveUsedGamePad(const float& elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	// 左スティックの入力情報を取得
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	ax *= moveSpeed;
	ay *= moveSpeed;

	Camera& camera = Camera::Instance();
	// 視線行列を生成
	DirectX::XMMATRIX V;
	{
		DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		V = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&debugPosition),
			DirectX::XMLoadFloat3(&debugFocus),
			up);
		DirectX::XMFLOAT4X4 W;
		DirectX::XMStoreFloat4x4(&W, DirectX::XMMatrixInverse(nullptr, V));

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

		debugFocus.x += cameraFront.x * ay + cameraRight.x * ax;
		debugFocus.z += cameraFront.z * ay + cameraRight.z * ax;
	}
}

void CameraController::DrawDebugGUI()
{
	if (ImGui::CollapsingHeader("CameraController"))
	{
		ImGui::DragFloat3("CameraPosition", &eye.x, 1.0f, -100.0f, 100.0f, "%.2f");
		ImGui::SliderFloat("CameraOffset", &offset, -10.0f, 10.0f);
		ImGui::SliderFloat("CameraRange", &range, -15.0f, 15.0f);
		ImGui::InputFloat("CameraSpeed", &moveSpeed);
		ImGui::SliderFloat("CameraOffset", &cameraOffset, -15.0f, 15.0f);
		ImGui::InputFloat("shaketotalTime", &shaketotalTime);

		if (ImGui::Button("Shake"))
		{
			CameraController::Instance().SetShakeMode(5.0f, 50.0f, 50.0f);

		}
	}
}

#pragma endregion