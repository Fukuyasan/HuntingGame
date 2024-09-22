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

// カメラコントローラ
class CameraController
{
private:
    // カメラの引く、寄る処理
    struct RangeData
    {
        RangeMode rangeMode = RangeMode::Range_First;
        float rangeLength   = 0.0f;
        float rangeTime     = 0.0f;
    };

    // カメラシェイク
    struct ShakeData
    {
        float shakeTime         = 0.0f;
        DirectX::XMFLOAT2 power = {};
    };
    float shaketotalTime = 0.0f;

    // クリア時の演出
    struct ClearPerformData
    {
        float performTime   = 0.0f;
        float PerformLength = 0.0f;
    };

public:
    CameraController(bool titleFlg = false);   // コンストラクタ
    ~CameraController();  // デストラクタ

    // インスタンス取得
    static CameraController& Instance() { return *instance; }

    // 更新処理
    void Update(const float& elapsedTime);

    // GUI
    void DrawDebugGUI();

    // レイキャストするモデルを登録
    void RegisterRaycastModels(Model model) { raycastModels.emplace_back(model); }

    // クリア用の演出
    bool ClearPerform(const float& elapsedTime);

public:
    // オーナー位置設定
    void SetOwner(std::shared_ptr<GameObject> owner) { this->owner = owner; }

    // ターゲット位置設定
    void SetTarget(std::shared_ptr<GameObject> target)
    {
        this->target     = target;
        this->targetRate = 0.0f;
    }

    // 引き寄せフレームセット
    void SetRangeMode(RangeMode mode, float length = 0.0f)
    {
        this->rangeData.rangeTime   = 0.0f;
        this->rangeData.rangeMode   = mode;
        this->rangeData.rangeLength = length;
    }

    // カメラシェイクセット
    void SetShakeMode(float time, float powerX, float powerY)
    {
        this->shaketotalTime      = 0.0f;
        this->shakeData.shakeTime = time;
        this->shakeData.power.x   = powerX;
        this->shakeData.power.y   = powerY;
    }

    // クリア演出
    void SetClearPerform(const float length) 
    {
        this->performFlg = true;
        this->lerpValue    = 1.0f;  // 演出をきっちり見せるため
        this->performData.performTime   = 0.0f;
        this->performData.PerformLength = length;
    }

    // ロックオンカメラ
    void SetLockOnCamera() { this->lockOn = !this->lockOn; }

    void SetEye(const DirectX::XMFLOAT3& eye) { this->eye = eye; }
    void SetDistance(const float distance) { this->distance = distance; }

private:
    // ゲームパッド、キーボードを使ったカメラの動き
    void CameraMoveUsedGamePad(const float& elapsedTime);

    // 通常のカメラ移動
    void NormalCameraMove(const float& elapsedTime);

    // カメラのレイキャスト
    void CameraRaycast(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, DirectX::XMFLOAT3& result);

    // ターゲットカメラ
    void TargetCamera(const float& elapsedTime);

    // カメラの引き、寄せ処理
    void ComputeRange(const float& elapsedTime);

    // カメラシェイク
    void ComputeShake(DirectX::XMFLOAT3& focus, const ShakeData& data, const float& elapsedTime);

#pragma region デバッグ用
    // マウスを使ったカメラの動き
    void D_CameraMoveUsedMouse();

    // ゲームパッド、キーボードを使ったカメラの動き
    void D_CameraMoveUsedGamePad(const float& elapsedTime);
#pragma endregion

#pragma region シーンゲーム用
private:
    static CameraController* instance;

    DirectX::XMFLOAT3 angle  = { 0,0,0 };
    float moveSpeed          = 1.0f;
    float rollSpeed          = DirectX::XMConvertToRadians(90);
    float range              = 7.0f;

    // X軸の最大回転値と最小回転値
    float maxAngle = DirectX::XMConvertToRadians(45);
    float minAngle = DirectX::XMConvertToRadians(-45);

    DirectX::XMFLOAT3 eye          = { 0,0,0 };
    DirectX::XMFLOAT3 front        = { 0,0,0 };
    DirectX::XMFLOAT3 currentEye   = { 0,0,0 };
    DirectX::XMFLOAT3 currentFocus = { 0,0,0 };

    // 注視するもの
    std::weak_ptr<GameObject> owner;
    std::weak_ptr<GameObject> target;
    DirectX::XMFLOAT3 ownerPos  = {};
    DirectX::XMFLOAT3 targetPos = {};

    // カメラの引き、寄せ
    const float middleRange = 7.0f;
    const float farRange    = 15.0f;
    const float closeRange  = 5.0f;

    RangeData        rangeData;
    ShakeData        shakeData;
    ClearPerformData performData;

    float lerpValue = 0.2f;

    float targetRate = 1.0f;

    // クリア演出用
    bool performFlg = false;
    float changeTime = 0.0f;

    // ロックオンカメラ
    bool lockOn = false;

    // カメラとレイキャストするモデル
    std::vector<Model> raycastModels;

public:
    bool noOwnerCamera = false;
#pragma endregion

private:
#pragma region デバッグ用
    float rotateX                    = DirectX::XMConvertToRadians(15);
    float rotateY                    = 0.0f;
    float distance                   = 10.0f;
    DirectX::XMFLOAT3 debugPosition{ 0.0f, 0.0f, -10.0f };
    DirectX::XMFLOAT3 debugFocus{ 0.0f, 0.0f, 0.0f };

    float offset                     = 0.0f;
    
    float cameraOffset = 0.5f;

#pragma endregion
};
