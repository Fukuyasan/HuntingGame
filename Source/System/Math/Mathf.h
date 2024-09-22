#pragma once
#include <DirectXMath.h>
#include "System/Misc.h"
#include <vector>
#include <random>

class Mathf	
{
public:
	// 線形補間
	template<typename T>
	static T lerp(const T a, const T b, const T t)
	{
		return  (1.0f - t) * a + (t * b);
	}

#pragma region ランダム
private:
	// is_integral : 型Tが整数型(cv修飾も許容される)であればtrue_typeから派生し、
	//				 そうでなければfalse_typeから派生する。(日本語リファレンスから)
	// true_type, false_type : 整数型かそうでないか

	// 整数型の場合のランダム
	template<typename T>
	static T RandomRangeInpl(const T min, const T max, std::true_type)
	{
		// 確率分布を指定
		std::uniform_int_distribution<T> distribution(min, max);

		return distribution(mt);
	}

	// 浮動小数点型の場合のランダム
	template<typename T>
	static T RandomRangeInpl(const T min, const T max, std::false_type)
	{
		// 確率分布を指定
		std::uniform_real_distribution<T> distribution(min, max);

		return distribution(mt);
	}

public:
	// 指定範囲のランダム値を計算する
	template<typename T>
	static T RandomRange(const T min, const T max)
	{
		return RandomRangeInpl(min, max, std::is_integral<T>{});
	}
#pragma endregion

	// クランプ関数
	template<typename T>
	static float Clamp(const T value, const T min, const T max)
	{
		if (value < min) return min;
		if (value > max) return max;

		return value;
	}

	// 確率分布
	static bool RandomProbability(const float probability);
	
public:
	static std::random_device device;
	static std::mt19937		  mt;
};

// ゲームでよく使う処理（長さを求める、ベクトルを求めるなど）長くなる助けて
class GameMath
{
public:
	// 吹き飛ばす
	static void Impulse(const float power, const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& goal, DirectX::XMFLOAT3& impulse);

	// 平面での長さを求める
	static float LengthFromXZ(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& goal);
	
	// 平面での長さの二乗を求める
	static float LengthSqFromXZ(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& goal);
	
	// 長さを求める
	static float Length(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& goal);

	// 長さの二乗を求める
	static float LengthSq(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& goal);

	// 行列から位置情報取得
	static DirectX::XMFLOAT3 GetPositionFrom4x4(const DirectX::XMFLOAT4X4& float4x4);

	// 特定のアニメーション再生時間内での行動を許可する
	static bool PermissionInAnimationSeconds(float animationSeconds, float startSeconds, float goalSeconds);

	// ローカル座標からワールド座標に変換
	static void ConvertWorldCoordinates(const DirectX::XMFLOAT3& localPosition, const DirectX::XMFLOAT4X4& worldTransform, DirectX::XMFLOAT3& worldPosition);
};

// パーリンノイズ
class PerlinNoise
{
	using Pint = std::int_fast8_t;

	const int maxNoiseSize = 512;
	const int halfNoise = maxNoiseSize >> 1;

public:
	PerlinNoise() {}

	// explicit : 暗黙的型変換を防ぐ

	explicit PerlinNoise(const UINT seed) {
		this->SetSeed(seed);
	}

	// 初期SEED値を設定する
	void SetSeed(const UINT seed) {
		pNoise.resize(maxNoiseSize);
		
		// 0 ~ 255までの数字を入れる
		for (int i = 0; i < halfNoise; ++i)
		{
			this->pNoise.emplace_back(static_cast<Pint>(i));
		}
		
		// 0 ~ 255までの数字をシャッフル
		std::shuffle(pNoise.begin(), pNoise.begin() + halfNoise, std::default_random_engine(seed));
		
		// 256 ~ 511までの数字を入れる
		for (int i = 0; i < halfNoise; ++i)
		{
			this->pNoise.emplace_back(this->pNoise[i]);
		}
	}

	// noexcept : 例外を出さない

	// ノイズを取得する
	template<typename... Args>
	double GetNoise(const Args... args) const noexcept
	{
		return this->SetNoise(args...) * 0.5f + 0.5f;
	}

private:
	constexpr double Fade(const double t) const noexcept
	{
		return t * t * t * (t * (t * 6 - 15) + 10);
	}

	constexpr double Lerp(const double a, const double b, const double t) const noexcept
	{
		return (1.0 - t) * a + (t * b);
	}
	
	constexpr double MakeGrad(const Pint hash, const double u, const double v) const noexcept
	{
		const double U = (hash & 1) == 0 ? u : -u;  // hash が奇数だと true
		const double V = (hash & 2) == 0 ? v : -v;  // hash が偶数だと true
		
		return U + V;
	}

	constexpr double MakeGrad(const Pint hash, const double x, const double y, const double z) const noexcept
	{
		const double U = hash < 8 ? x : y;
		const double V = hash < 4 ? y : hash == 12 || hash == 14 ? x : z;

		return MakeGrad(hash, U, V);
	}

	constexpr double GetGrad(const Pint hash, const double x, const double y, const double z) const noexcept
	{
		return MakeGrad(hash, x, y, z);
	}

	double SetNoise(double x = 0.0f, double y = 0.0f, double z = 0.0f) const noexcept
	{
		const size_t X = static_cast<size_t>(static_cast<size_t>(std::floor(x)) & 255);
		const size_t Y = static_cast<size_t>(static_cast<size_t>(std::floor(y)) & 255);
		const size_t Z = static_cast<size_t>(static_cast<size_t>(std::floor(z)) & 255);

		x -= std::floor(x);
		y -= std::floor(y);
		z -= std::floor(z);

		const double U = this->Fade(x);
		const double V = this->Fade(y);
		const double W = this->Fade(z);

		const size_t a0 = static_cast<std::size_t>(this->pNoise[X]		+ Y);
		const size_t a1 = static_cast<std::size_t>(this->pNoise[a0]		+ Z);
		const size_t a2 = static_cast<std::size_t>(this->pNoise[a0 + 1] + Z);

		const size_t b0 = static_cast<std::size_t>(this->pNoise[X + 1]	+ Y);
		const size_t b1 = static_cast<std::size_t>(this->pNoise[b0]		+ Z);
		const size_t b2 = static_cast<std::size_t>(this->pNoise[b0 + 1] + Z);

		return this->Lerp(W,
			this->Lerp(V,
				this->Lerp(U, this->GetGrad(this->pNoise[a1], x, y,		z), this->GetGrad(this->pNoise[b1], x - 1, y,	  z)),
				this->Lerp(U, this->GetGrad(this->pNoise[a2], x, y - 1, z), this->GetGrad(this->pNoise[b2], x - 1, y - 1, z))),
			this->Lerp(V,
				this->Lerp(U, this->GetGrad(this->pNoise[a1 + 1], x, y,		z - 1), this->GetGrad(this->pNoise[b1 + 1], x - 1, y,     z - 1)),
				this->Lerp(U, this->GetGrad(this->pNoise[a2 + 1], x, y - 1, z - 1), this->GetGrad(this->pNoise[b2 + 1], x - 1, y - 1, z - 1))));
	}

private:
	std::vector<Pint> pNoise = {};
};