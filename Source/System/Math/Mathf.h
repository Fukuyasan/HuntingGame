#pragma once
#include <DirectXMath.h>
#include "System/Misc.h"
#include <vector>
#include <random>

class Mathf	
{
public:
	// ���`���
	template<typename T>
	static T lerp(const T a, const T b, const T t)
	{
		return  (1.0f - t) * a + (t * b);
	}

#pragma region �����_��
private:
	// is_integral : �^T�������^(cv�C�������e�����)�ł����true_type����h�����A
	//				 �����łȂ����false_type����h������B(���{�ꃊ�t�@�����X����)
	// true_type, false_type : �����^�������łȂ���

	// �����^�̏ꍇ�̃����_��
	template<typename T>
	static T RandomRangeInpl(const T min, const T max, std::true_type)
	{
		// �m�����z���w��
		std::uniform_int_distribution<T> distribution(min, max);

		return distribution(mt);
	}

	// ���������_�^�̏ꍇ�̃����_��
	template<typename T>
	static T RandomRangeInpl(const T min, const T max, std::false_type)
	{
		// �m�����z���w��
		std::uniform_real_distribution<T> distribution(min, max);

		return distribution(mt);
	}

public:
	// �w��͈͂̃����_���l���v�Z����
	template<typename T>
	static T RandomRange(const T min, const T max)
	{
		return RandomRangeInpl(min, max, std::is_integral<T>{});
	}
#pragma endregion

	// �N�����v�֐�
	template<typename T>
	static float Clamp(const T value, const T min, const T max)
	{
		if (value < min) return min;
		if (value > max) return max;

		return value;
	}

	// �m�����z
	static bool RandomProbability(const float probability);
	
public:
	static std::random_device device;
	static std::mt19937		  mt;
};

// �Q�[���ł悭�g�������i���������߂�A�x�N�g�������߂�Ȃǁj�����Ȃ鏕����
class GameMath
{
public:
	// ������΂�
	static void Impulse(const float power, const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& goal, DirectX::XMFLOAT3& impulse);

	// ���ʂł̒��������߂�
	static float LengthFromXZ(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& goal);
	
	// ���ʂł̒����̓������߂�
	static float LengthSqFromXZ(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& goal);
	
	// ���������߂�
	static float Length(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& goal);

	// �����̓������߂�
	static float LengthSq(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& goal);

	// �s�񂩂�ʒu���擾
	static DirectX::XMFLOAT3 GetPositionFrom4x4(const DirectX::XMFLOAT4X4& float4x4);

	// ����̃A�j���[�V�����Đ����ԓ��ł̍s����������
	static bool PermissionInAnimationSeconds(float animationSeconds, float startSeconds, float goalSeconds);

	// ���[�J�����W���烏�[���h���W�ɕϊ�
	static void ConvertWorldCoordinates(const DirectX::XMFLOAT3& localPosition, const DirectX::XMFLOAT4X4& worldTransform, DirectX::XMFLOAT3& worldPosition);
};

// �p�[�����m�C�Y
class PerlinNoise
{
	using Pint = std::int_fast8_t;

	const int maxNoiseSize = 512;
	const int halfNoise = maxNoiseSize >> 1;

public:
	PerlinNoise() {}

	// explicit : �ÖٓI�^�ϊ���h��

	explicit PerlinNoise(const UINT seed) {
		this->SetSeed(seed);
	}

	// ����SEED�l��ݒ肷��
	void SetSeed(const UINT seed) {
		pNoise.resize(maxNoiseSize);
		
		// 0 ~ 255�܂ł̐���������
		for (int i = 0; i < halfNoise; ++i)
		{
			this->pNoise.emplace_back(static_cast<Pint>(i));
		}
		
		// 0 ~ 255�܂ł̐������V���b�t��
		std::shuffle(pNoise.begin(), pNoise.begin() + halfNoise, std::default_random_engine(seed));
		
		// 256 ~ 511�܂ł̐���������
		for (int i = 0; i < halfNoise; ++i)
		{
			this->pNoise.emplace_back(this->pNoise[i]);
		}
	}

	// noexcept : ��O���o���Ȃ�

	// �m�C�Y���擾����
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
		const double U = (hash & 1) == 0 ? u : -u;  // hash ������� true
		const double V = (hash & 2) == 0 ? v : -v;  // hash ���������� true
		
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