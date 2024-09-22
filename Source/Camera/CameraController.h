#pragma once
#include <DirectXMath.h>
#include <vector>

#include "Graphics/Model/Model.h"

#include "System/Math/Mathf.h"
#include "Component/Component.h"

enum class RangeMode
{
    Range_First,
    Range_Middle,
    Range_Far,
    Range_Close,
    Range_Perform,
};

// �J�����R���g���[��
class CameraController
{
private:
    // �J�����̈����A��鏈��
    struct RangeData
    {
        RangeMode rangeMode = RangeMode::Range_First;
        float rangeLength   = 0.0f;
        float rangeTime     = 0.0f;
    };

    // �J�����V�F�C�N
    struct ShakeData
    {
        float shakeTime         = 0.0f;
        DirectX::XMFLOAT2 power = {};
    };
    float shaketotalTime = 0.0f;

    // �N���A���̉��o
    struct ClearPerformData
    {
        float performTime   = 0.0f;
        float PerformLength = 0.0f;
    };

public:
    CameraController(bool titleFlg = false);   // �R���X�g���N�^
    ~CameraController();  // �f�X�g���N�^

    // �C���X�^���X�擾
    static CameraController& Instance() { return *instance; }

    // �X�V����
    void Update(const float& elapsedTime);

    // GUI
    void DrawDebugGUI();

    // ���C�L���X�g���郂�f����o�^
    void RegisterRaycastModels(Model model) { raycastModels.emplace_back(model); }

    // �N���A�p�̉��o
    bool ClearPerform(const float& elapsedTime);

public:
    // �I�[�i�[�ʒu�ݒ�
    void SetOwner(std::shared_ptr<GameObject> owner) { this->owner = owner; }

    // �^�[�Q�b�g�ʒu�ݒ�
    void SetTarget(std::shared_ptr<GameObject> target)
    {
        this->target     = target;
        this->targetRate = 0.0f;
    }

    // �����񂹃t���[���Z�b�g
    void SetRangeMode(RangeMode mode, float length = 0.0f)
    {
        this->rangeData.rangeTime   = 0.0f;
        this->rangeData.rangeMode   = mode;
        this->rangeData.rangeLength = length;
    }

    // �J�����V�F�C�N�Z�b�g
    void SetShakeMode(float time, float powerX, float powerY)
    {
        this->shaketotalTime      = 0.0f;
        this->shakeData.shakeTime = time;
        this->shakeData.power.x   = powerX;
        this->shakeData.power.y   = powerY;
    }

    // �N���A���o
    void SetClearPerform(const float length) 
    {
        this->performFlg = true;
        this->lerpValue    = 1.0f;  // ���o���������茩���邽��
        this->performData.performTime   = 0.0f;
        this->performData.PerformLength = length;
    }

    // ���b�N�I���J����
    void SetLockOnCamera() { this->lockOn = !this->lockOn; }

    void SetEye(const DirectX::XMFLOAT3& eye) { this->eye = eye; }
    void SetDistance(const float distance) { this->distance = distance; }

private:
    // �Q�[���p�b�h�A�L�[�{�[�h���g�����J�����̓���
    void CameraMoveUsedGamePad(const float& elapsedTime);

    // �ʏ�̃J�����ړ�
    void NormalCameraMove(const float& elapsedTime);

    // �J�����̃��C�L���X�g
    void CameraRaycast(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, DirectX::XMFLOAT3& result);

    // �^�[�Q�b�g�J����
    void TargetCamera(const float& elapsedTime);

    // �J�����̈����A�񂹏���
    void ComputeRange(const float& elapsedTime);

    // �J�����V�F�C�N
    void ComputeShake(DirectX::XMFLOAT3& focus, const ShakeData& data, const float& elapsedTime);

#pragma region �f�o�b�O�p
    // �}�E�X���g�����J�����̓���
    void D_CameraMoveUsedMouse();

    // �Q�[���p�b�h�A�L�[�{�[�h���g�����J�����̓���
    void D_CameraMoveUsedGamePad(const float& elapsedTime);
#pragma endregion

#pragma region �V�[���Q�[���p
private:
    static CameraController* instance;

    DirectX::XMFLOAT3 angle  = { 0,0,0 };
    float moveSpeed          = 1.0f;
    float rollSpeed          = DirectX::XMConvertToRadians(90);
    float range              = 7.0f;

    // X���̍ő��]�l�ƍŏ���]�l
    float maxAngle = DirectX::XMConvertToRadians(45);
    float minAngle = DirectX::XMConvertToRadians(-45);

    DirectX::XMFLOAT3 eye          = { 0,0,0 };
    DirectX::XMFLOAT3 front        = { 0,0,0 };
    DirectX::XMFLOAT3 currentEye   = { 0,0,0 };
    DirectX::XMFLOAT3 currentFocus = { 0,0,0 };

    // �����������
    std::weak_ptr<GameObject> owner;
    std::weak_ptr<GameObject> target;
    DirectX::XMFLOAT3 ownerPos  = {};
    DirectX::XMFLOAT3 targetPos = {};

    // �J�����̈����A��
    const float middleRange = 7.0f;
    const float farRange    = 15.0f;
    const float closeRange  = 5.0f;

    RangeData        rangeData;
    ShakeData        shakeData;
    ClearPerformData performData;

    float lerpValue = 0.2f;

    float targetRate = 1.0f;

    // �N���A���o�p
    bool performFlg = false;
    float changeTime = 0.0f;

    // ���b�N�I���J����
    bool lockOn = false;

    // �J�����ƃ��C�L���X�g���郂�f��
    std::vector<Model> raycastModels;

public:
    bool noOwnerCamera = false;
#pragma endregion

private:
#pragma region �f�o�b�O�p
    float rotateX                    = DirectX::XMConvertToRadians(15);
    float rotateY                    = 0.0f;
    float distance                   = 10.0f;
    DirectX::XMFLOAT3 debugPosition{ 0.0f, 0.0f, -10.0f };
    DirectX::XMFLOAT3 debugFocus{ 0.0f, 0.0f, 0.0f };

    float offset                     = 0.0f;
    
    float cameraOffset = 0.5f;

#pragma endregion
};
