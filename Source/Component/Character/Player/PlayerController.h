#pragma once
#include <vector>
#include <queue>
#include <DirectXMath.h>

#include "System/Input/GamePad.h"

typedef uint32_t InputKey;

namespace Key {
    // 先行入力キー
    static const InputKey KeyY  = (1 << 1);
    static const InputKey KeyA  = (1 << 2);
    static const InputKey KeyB  = (1 << 3);
    static const InputKey KeyX  = (1 << 4);
    static const InputKey KeyYB = (1 << 5);
}

class PlayerContorller
{
public:
    struct KeyData {
        InputKey key;
        int      frame;
    };

public:
	PlayerContorller(int slot);
    ~PlayerContorller() {};

	void Update();

    // 先行入力
    bool GetKeyPrecede(const InputKey& key, int frame = 20);

    // スティック入力値から移動ベクトルを取得
    const DirectX::XMFLOAT3& GetMoveVec() const;

    const GamePad& GetGamePad() { return this->gamePad; }

private:
    void RegisterKey(const InputKey& key);

private:
	GamePad gamePad;

    std::vector<KeyData> keyPool;
    static inline const int saveFrame = 60;

    const int maxInputKey = 60;
};
