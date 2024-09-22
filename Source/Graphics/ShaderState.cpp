#include "Graphics.h"
#include "System/Misc.h"

#include "ShaderState.h"
#include "CreateShader.h"

#include "Shader/ModelShader/PBRShader.h"
#include "Shader/ModelShader/Shadowmap/ShadowmapCasterShader.h"

void ShaderState::Initialize(HWND hWnd)
{
	Graphics& graphics   = Graphics::Instance();
	ID3D11Device* device = graphics.GetDevice();

	// シェーダー
	{
		// 3Dシェーダー
		modelShaders[static_cast<size_t>(MODEL::DefaultPBR)] = std::make_unique<PBRShader>("Default");
		modelShaders[static_cast<size_t>(MODEL::EnemyPBR)]   = std::make_unique<PBRShader>("Enemy");
		shadowmapShader = std::make_unique<ShadowmapCasterShader>(device);

		// 2Dシェーダー
		spriteShaders[static_cast<size_t>(SPRITE::Default)] = std::make_unique<SpriteShader>("Sprite");
		spriteShaders[static_cast<size_t>(SPRITE::Mask)]	= std::make_unique<SpriteShader>("Mask");
		skymapShader									    = std::make_unique<SkymapShader>(device);
	}

	// レンダラ
	{
		debugRenderer   = std::make_unique<DebugRenderer>(device);
		lineRenderer    = std::make_unique<LineRenderer>(device, 1024);
		imguiRenderer   = std::make_unique<ImGuiRenderer>(hWnd, device);
		navMeshRenderer = std::make_unique<NavMeshRenderer>(device, 1024);
	}
	
	// オフスクリーンレンダリング
	{
		uint32_t width  = static_cast<uint32_t>(graphics.GetScreenWidth());
		uint32_t height = static_cast<uint32_t>(graphics.GetScreenHeight());

		// 各オフスクリーンバッファ
		for (int i = 0; i < static_cast<int>(OFFSCREEN::MAX); ++i)
		{
			offscreenBuffer[i] = std::make_unique<FrameBuffer>(device, width, height);
		}
		
		// ブラー用のバッファ ※ 4枚
		uint32_t blurWidth  = width;
		uint32_t blurHeight = height;
		for (int i = 0; i < BLUR_MAX; ++i)
		{
			blurBuffer[i] = std::make_unique<FrameBuffer>(device, blurWidth, blurHeight);

			blurWidth  /= 2;
			blurHeight /= 2;
		}

		// オフスクリーンのシェーダー
		CreateShader::CreatePSFromCso(device, "Shader\\LuminanceExtraction.cso", offscreenShader[static_cast<size_t>(OFFSCREEN::LUMINANCE)].GetAddressOf());
		CreateShader::CreatePSFromCso(device, "Shader\\HeatHaze.cso",			 offscreenShader[static_cast<size_t>(OFFSCREEN::HEATHAZE)].GetAddressOf());
		CreateShader::CreatePSFromCso(device, "Shader\\FinalPass.cso",		     offscreenShader[static_cast<size_t>(OFFSCREEN::FINAL)].GetAddressOf());
		CreateShader::CreatePSFromCso(device, "Shader\\ShockBlur.cso",		     offscreenShader[static_cast<size_t>(OFFSCREEN::SHOCKBLUR)].GetAddressOf());
	}
}
