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

	// �I�[�i�[��ݒ肷�邩
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

// �X�V����
void CameraController::Update(const float& elapsedTime)
{
	if(noOwnerCamera)
	{
		// �I�[�i�[���ݒ肳��ĂȂ��ꍇ(��Ƀf�o�b�O�Ŏg�p����)
		D_CameraMoveUsedGamePad(elapsedTime);
		D_CameraMoveUsedMouse();
		return;
	}
	else
	{
		if (owner.lock())
		{
			// �I�[�i�[�̐ݒ�
			ownerPos	= owner.lock()->GetTransform().GetPosition();
			ownerPos.y += 1.3f + cameraOffset;
		}

		if (target.lock())
		{
			// �^�[�Q�b�g�̐ݒ�
			targetPos	 = target.lock()->GetTransform().GetPosition();
			//targetPos.y += target->GetHeight();

		}
		
		// �I�[�i�[�Ƃ̃J����������ύX
		ComputeRange(elapsedTime);

		// �Q�[���p�b�h���g�p�����Ƃ��̃J�����̋���
		CameraMoveUsedGamePad(elapsedTime);
	}

	

	// ���`�ۊ�
	currentEye.x = Mathf::lerp(currentEye.x, eye.x, lerpValue);
	currentEye.y = Mathf::lerp(currentEye.y, eye.y, lerpValue);
	currentEye.z = Mathf::lerp(currentEye.z, eye.z, lerpValue);

	currentFocus.x = Mathf::lerp(currentFocus.x, ownerPos.x, lerpValue);
	currentFocus.y = Mathf::lerp(currentFocus.y, ownerPos.y, lerpValue);
	currentFocus.z = Mathf::lerp(currentFocus.z, ownerPos.z, lerpValue);

	// �J�����V�F�C�N
	ComputeShake(currentFocus, shakeData, elapsedTime);

	// ���C�L���X�g
	CameraRaycast(currentFocus, currentEye, currentEye);

	// �J�����̎��_�ƒ����_��ݒ�
	Camera::Instance().SetLookAt(currentEye, currentFocus, DirectX::XMFLOAT3(0, 1, 0));
}

void CameraController::CameraMoveUsedGamePad(const float& elapsedTime)
{
	if (performFlg) return;
	
	if (lockOn)
	{
		// ���b�N�I���J����
		TargetCamera(elapsedTime);
	}
	else
	{
		// �^�[�Q�b�g�J�����̉�]���I�������
		if (targetRate >= 1.0f)
		{
			// �ʏ�̃J����
			NormalCameraMove(elapsedTime);
		}
		else
		{
			// �^�[�Q�b�g�J����
			TargetCamera(elapsedTime);
		}
	}

}

void CameraController::NormalCameraMove(const float& elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisRX();
	float ay = gamePad.GetAxisRY();

	// �J�����̉�]���x
	float speed = rollSpeed * elapsedTime;

	// �X�e�B�b�N�̓��͒l�ɍ��킹��X����Y������]
	angle.x += -ay * speed;
	angle.y +=  ax * speed;

	// X���̃J������]�𐧌�
	if (angle.x > maxAngle) { angle.x = maxAngle; }
	if (angle.x < minAngle) { angle.x = minAngle; }

	// Y���̉�]�l��-3.14�`3.14�Ɏ��܂�悤�ɂ���
	if (angle.y < -DirectX::XM_PI) { angle.y += DirectX::XM_2PI; }
	if (angle.y > DirectX::XM_PI)  { angle.y -= DirectX::XM_2PI; }

	// �J������]�l����]�s��ɕϊ�
	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(
		angle.x,
		angle.y,
		angle.z
	);

	// ��]�s�񂩂�O�����x�N�g�������o��
	DirectX::XMVECTOR Front = Transform.r[2];
	DirectX::XMStoreFloat3(&front, Front);

	// �����_������x�N�g�������Ɉ�苗�����ꂽ�J�������_�����߂�
	eye.x = ownerPos.x + front.x * -range;
	eye.y = ownerPos.y + front.y * -range;
	eye.z = ownerPos.z + front.z * -range;
}

// �J�����̃��C�L���X�g
void CameraController::CameraRaycast(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, DirectX::XMFLOAT3& result)
{
	float distance = FLT_MAX;

	// �o�^�������f���ƃ��C�L���X�g
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

// �^�[�Q�b�g�J����
void CameraController::TargetCamera(const float& elapsedTime)
{
	DirectX::XMVECTOR Owner  = DirectX::XMLoadFloat3(&ownerPos);   // �v���C���[
	DirectX::XMVECTOR Target = DirectX::XMLoadFloat3(&targetPos);  // �G
	DirectX::XMVECTOR Eye    = DirectX::XMLoadFloat3(&eye);

	// �^�[�Q�b�g�ƃI�[�i�[�̃x�N�g�������߂�(���K��)
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Target, Owner);
	Vec = DirectX::XMVector3Normalize(Vec);

	// �I�[�i�[���� range ���ꂽ�ꏊ�ɉ��̃J�����ʒu��ݒ�
	Vec = DirectX::XMVectorScale(Vec, range * -1);

	// �I�[�i�[���猻�݂̃J�����̈ʒu�ւ̃x�N�g�������߂�
	DirectX::XMVECTOR Owner_Eye = DirectX::XMVectorSubtract(Eye, Owner);

	// ���[�v
	DirectX::XMVECTOR Lerp = DirectX::XMVectorLerp(Owner_Eye, Vec, 1.0f);
	targetRate += elapsedTime * 3.0f;

	DirectX::XMVECTOR L = DirectX::XMVectorAdd(Owner, Lerp);

	// �v�Z���ʂ���
	DirectX::XMStoreFloat3(&eye, L);

	// angle ����]������
	DirectX::XMFLOAT3 vec;
	DirectX::XMStoreFloat3(&vec, Vec);

	float f = sqrtf(vec.x * vec.x + vec.z * vec.z);

	// atan2f(x, y) : y/x�̋t���ڂ̒l���v�Z����B
	angle.x   = atan2f(vec.y, f);
	angle.y   = atan2f(-vec.x, -vec.z);
}

// �J�����̈����A�񂹏���
void CameraController::ComputeRange(const float& elapsedTime)
{
	rangeData.rangeTime += elapsedTime;

	switch (rangeData.rangeMode)
	{
	case RangeMode::Range_First:
		// �^�񒆏���
		performFlg = false;
		range = middleRange;

		break;
	case RangeMode::Range_Middle:
		// �^�񒆏���
		range = Mathf::lerp(farRange, middleRange, (std::min)(1.0f, rangeData.rangeTime * 2.0f));

		break;
	case RangeMode::Range_Far:
		// �����̏���
		range = Mathf::lerp(middleRange, farRange, (std::min)(1.0f, rangeData.rangeTime * 2.0f));

		if (rangeData.rangeTime >= rangeData.rangeLength)
		{
			rangeData.rangeTime = 0.0f;
			rangeData.rangeMode = RangeMode::Range_Middle;
		}

		break;
	case RangeMode::Range_Close:
		// �񂹏���
		range = Mathf::lerp(middleRange, closeRange, (std::min)(1.0f, rangeData.rangeTime * 2.0f));

		if (rangeData.rangeTime >= rangeData.rangeLength)
		{
			rangeData.rangeTime = 0.0f;
			rangeData.rangeMode = RangeMode::Range_Middle;
		}

		break;
	case RangeMode::Range_Perform:
		// �񂹏���
		range = 20.0f;
		performFlg = true;

		break;
	}
}

// �J�����V�F�C�N
void CameraController::ComputeShake(DirectX::XMFLOAT3& focus, const ShakeData& data, const float& elapsedTime)
{
	if (data.shakeTime <= 0.0f) return;
	if (shaketotalTime > data.shakeTime) return;

	shaketotalTime += elapsedTime;

	float raito  = 1.0f - Easing::OutQuart(shaketotalTime, data.shakeTime);
	float powerX = data.power.x * raito;
	float powerY = data.power.y * raito;

	// �h�ꂪ�Ȃ��ꍇ return
	if (powerX == 0.0f && powerY == 0.0f) return;

	float randomX = Mathf::RandomRange(-powerX, powerX);
	float randomY = Mathf::RandomRange(-powerY, powerY);
		
	DirectX::XMFLOAT3 position = focus;
	position.x += randomX * elapsedTime;
	position.y += randomY * elapsedTime;

	focus = position;
}

// �N���A���o
bool CameraController::ClearPerform(const float& elapsedTime)
{
	// �J�����̃A���O����4�b���Ƃɐ؂�ւ��悤�ɂ���
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

#if 0 // ���o�㎩�R�ɓ�����悤�ɂ���Ȃ�
	// ���Ԍo�߂ŏI��
	performData.performTime += elapsedTime;
	if (performData.performTime > performData.PerformLength)
	{
		clearPerform = false;
		return true;
	}

	return false;
#else // �N���A��^�C�g���ɖ߂��̂�
	performData.performTime += elapsedTime;
	return performData.performTime > performData.PerformLength;
#endif
}

#pragma region �f�o�b�O�p

void CameraController::D_CameraMoveUsedMouse()
{
	Mouse& mouse = Input::Instance().GetMouse();

	float moveX = (mouse.GetPositionX() - mouse.GetOldPositionX()) * 0.02f;
	float moveY = (mouse.GetPositionY() - mouse.GetOldPositionY()) * 0.02f;

	Camera& camera = Camera::Instance();
	// �����s��𐶐�
	DirectX::XMMATRIX V;
	{
		DirectX::XMVECTOR up{ DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };
		// �}�E�X����
		{
			if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
			{
				// Y����]
				rotateY += moveX * 0.5f;
				if (rotateY > DirectX::XM_PI)
					rotateY -= DirectX::XM_2PI;
				else if (rotateY < -DirectX::XM_PI)
					rotateY += DirectX::XM_2PI;
				// X����]
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
				// ���s�ړ�
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
			if (mouse.GetWheel() != 0)	// �Y�[��
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
		// �J�����Ɏ��_�𒍎��_��ݒ�
		Camera::Instance().SetLookAt(debugPosition, debugFocus, { 0, 1, 0 });
	}

}

void CameraController::D_CameraMoveUsedGamePad(const float& elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	// ���X�e�B�b�N�̓��͏����擾
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	ax *= moveSpeed;
	ay *= moveSpeed;

	Camera& camera = Camera::Instance();
	// �����s��𐶐�
	DirectX::XMMATRIX V;
	{
		DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		V = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&debugPosition),
			DirectX::XMLoadFloat3(&debugFocus),
			up);
		DirectX::XMFLOAT4X4 W;
		DirectX::XMStoreFloat4x4(&W, DirectX::XMMatrixInverse(nullptr, V));

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