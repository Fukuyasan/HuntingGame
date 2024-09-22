#pragma once

#include <DirectXMath.h>

// カメラ
class Camera final
{
private:
    Camera() {}
    ~Camera() {}
public:
    Camera(Camera&&)                = delete; 
    Camera(const Camera&)           = delete;
    Camera operator=(Camera&&)      = delete;
    Camera operator=(const Camera&) = delete;

public:
    // 唯一のインスタンス取得
    static Camera& Instance()
    {
        static Camera camera;
        return camera;
    }

    // 指定方向を向く
    void SetLookAt(
        const DirectX::XMFLOAT3& eye,
        const DirectX::XMFLOAT3& focus,
        const DirectX::XMFLOAT3& up
    );

    // パースペクティブ設定
    void SetPerspectiveFov(
        float forY,
        float aspect,
        float nearZ,
        float farZ
    );

    /* ゲッター */

    // ビュー行列取得
    const DirectX::XMFLOAT4X4& GetView() const { return view; }

    // プロジェクション行列取得
    const DirectX::XMFLOAT4X4& GetProjection() const { return projection; }

    // 視点取得
    const DirectX::XMFLOAT3& GetEye() const { return eye; }

    // 注視点取得
    const DirectX::XMFLOAT3& GetFocus() const { return focus; }

    // 上方向取得
    const DirectX::XMFLOAT3& GetUp() const { return up; }

    // 前方向取得
    const DirectX::XMFLOAT3& GetFront() const { return front; }

    // 右方向取得
    const DirectX::XMFLOAT3& GetRight() const { return right; }

    // カメラクラスのメンバ変数
private:
    DirectX::XMFLOAT4X4 view;        // ビュー行列
    DirectX::XMFLOAT4X4 projection;  // プロジェクション行列

    DirectX::XMFLOAT3   eye;    // 視点
    DirectX::XMFLOAT3   focus;  // 注視点

    DirectX::XMFLOAT3   up;     // 上方向
    DirectX::XMFLOAT3   front;  // 前方向
    DirectX::XMFLOAT3   right;  // 右方向
};
