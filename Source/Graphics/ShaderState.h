#pragma once

#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include "Shader.h"

#include "Graphics/Renderer/DebugRenderer.h"
#include "Graphics/Renderer/LineRenderer.h"
#include "Graphics/Renderer/ImGuiRenderer.h"
#include "Graphics/Renderer/NavMeshRenderer.h"

#include "FrameBuffer.h"

#include "Shader/SpriteShader/SkymapShader.h"
#include "Shader/SpriteShader/DefaultSpriteShader.h"

#define BLUR_MAX 4

// オフスクリーン
enum class OFFSCREEN
{
	OFFSCREEN_SET,
	LUMINANCE,
	HEATHAZE,
	SHOCKBLUR,
	FINAL,
	MAX
};

// モデル
enum class MODEL
{
	DefaultPBR,
	EnemyPBR,
	Max
};

// スプライト
enum class SPRITE
{
	Default,
	UVScroll,
	Mask,
	Max
};

class ShaderState final
{
private:
	ShaderState()  {}
	~ShaderState() {}

public:
	ShaderState(const ShaderState&) = delete;
	ShaderState(ShaderState&&)      = delete;
	ShaderState operator=(const ShaderState&) = delete;
	ShaderState operator=(ShaderState&&)	  = delete;

public:
	// インスタンス取得
	static ShaderState& Instance()
	{
		static ShaderState instance;
		return instance;
	}

	// 初期化
	void Initialize(HWND hWnd);

	// 各シェーダー取得
	Shader* GetModelShader(MODEL model)			 const { return modelShaders[static_cast<int>(model)].get(); }
	Shader* GetShadowmapShader()				 const { return shadowmapShader.get(); }

	SpriteShader* GetSpriteShader(SPRITE sprite) const { return spriteShaders[static_cast<int>(sprite)].get(); }
	SkymapShader* GetSkymapShader()				 const { return skymapShader.get(); }

	// 各レンダラ取得
	DebugRenderer* GetDebugRenderer()	  const { return debugRenderer.get(); }
	LineRenderer* GetLineRenderer()		  const { return lineRenderer.get(); }
	ImGuiRenderer* GetImGuiRenderer()	  const { return imguiRenderer.get(); }
	NavMeshRenderer* GetNavMeshRenderer() const { return navMeshRenderer.get(); }

	// オフスクリーンのバッファ取得
	FrameBuffer* GetOffscreenBuffer(OFFSCREEN offscreen) const { return offscreenBuffer[static_cast<size_t>(offscreen)].get(); }
	FrameBuffer* GetBlurBuffer(int index) const { return blurBuffer[index].get(); }

	// オフスクリーンのシェーダー取得
	ID3D11PixelShader* GetOffscreenShader(OFFSCREEN offscreen) const { return offscreenShader[static_cast<size_t>(offscreen)].Get(); }

private:
	std::unique_ptr<Shader>				      modelShaders[static_cast<int>(MODEL::Max)];
	std::unique_ptr<Shader>					  shadowmapShader;

	std::unique_ptr<SpriteShader>			  spriteShaders[static_cast<int>(SPRITE::Max)];
	std::unique_ptr<SkymapShader>			  skymapShader;

	std::unique_ptr<DebugRenderer>			  debugRenderer;
	std::unique_ptr<LineRenderer>			  lineRenderer;
	std::unique_ptr<ImGuiRenderer>			  imguiRenderer;
	std::unique_ptr<NavMeshRenderer>		  navMeshRenderer;

	std::unique_ptr<FrameBuffer>			  offscreenBuffer[static_cast<size_t>(OFFSCREEN::MAX)];
	std::unique_ptr<FrameBuffer>			  blurBuffer[8];

	Microsoft::WRL::ComPtr<ID3D11PixelShader> offscreenShader[static_cast<size_t>(OFFSCREEN::MAX)];
};
