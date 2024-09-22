#include "PostEffect.h"
#include "Graphics/Graphics.h"
#include "CreateShader.h"
#include "Camera/Camera.h"
#include "System\Math\Easing.h"
#include <imgui.h>

void PostEffect::Initialize()
{
	Graphics& graphics = Graphics::Instance();

	// デバッグ用定数バッファ
	CreateShader::CreateCB(graphics.GetDevice(), sizeof(CBBloom),     cbBloom.ReleaseAndGetAddressOf());
	CreateShader::CreateCB(graphics.GetDevice(), sizeof(CBHeatHaze),  cbHeatHaze.ReleaseAndGetAddressOf());
	CreateShader::CreateCB(graphics.GetDevice(), sizeof(CbShockBlur), cbShockBlur.ReleaseAndGetAddressOf());
}

void PostEffect::Update(const float& elapsedTime)
{
	heatHaze.time += elapsedTime;

	totalBlurTime += elapsedTime;

	if (totalBlurTime > BlurTime) return;
	float rate = Easing::OutQuart(totalBlurTime, BlurTime);
	shockBlur.blurPower *= 1.0f - rate;
}


void PostEffect::DrawDebubGUI()
{
	if (ImGui::CollapsingHeader("BLOOM"))
	{
		ImGui::SliderFloat("Min", &bloom.min, 0.0f, 10.0f);
		ImGui::SliderFloat("Max", &bloom.max, 0.0f, 10.0f);
		ImGui::SliderFloat("gaussianSigma", &bloom.gaussianSigma, 0.0f, 10.0f);
		ImGui::SliderFloat("bloomIntensity", &bloom.bloomIntensity, 0.0f, 10.0f);
		ImGui::SliderFloat("exposure", &bloom.exposure, 0.0f, 10.0f);
	}
	
	if (ImGui::CollapsingHeader("HEATHAZE"))
	{
		// HEAT_HAZE
		ImGui::SliderFloat("amplitude", &heatHaze.amplitude, +0.0f, +0.5f, "%.4f");
		ImGui::SliderFloat("depthOffset", &heatHaze.depthOffset, +0.0f, +1.0f, "%.4f");
		ImGui::SliderFloat("timeScale", &heatHaze.timeScale, +0.0f, +1.0f, "%.4f");
		ImGui::SliderFloat("seedScale", &heatHaze.seedScale, +0.0f, +1.0f, "%.4f");
		ImGui::ColorEdit3("colorBalance", heatHaze.colorBalance);
		ImGui::SliderFloat("scorchingHeat", &(heatHaze.colorBalance[3]), 1, 100);

	}

	if (ImGui::CollapsingHeader("SHOCKBLUR"))
	{
		ImGui::InputFloat2("centerTexel", &shockBlur.centerTexel.x);
		ImGui::SliderFloat("seed_scale", &shockBlur.blurPower, 0.0f, 100.0f);
	}
}

void PostEffect::UpdateBloom(ID3D11DeviceContext* dc)
{
	// ブルームの定数バッファ
	dc->UpdateSubresource(cbBloom.Get(), 0, 0, &bloom, 0, 0);
	dc->PSSetConstantBuffers(6, 1, cbBloom.GetAddressOf());
}

void PostEffect::UpdateHeatHaze(ID3D11DeviceContext* dc)
{
	// 陽炎の定数バッファ
	dc->UpdateSubresource(cbHeatHaze.Get(), 0, 0, &heatHaze, 0, 0);
	dc->PSSetConstantBuffers(10, 1, cbHeatHaze.GetAddressOf());
}

void PostEffect::UpdateShockBlur(ID3D11DeviceContext* dc)
{
	// ラジアルブラーの定数バッファ
	dc->UpdateSubresource(cbShockBlur.Get(), 0, 0, &shockBlur, 0, 0);
	dc->PSSetConstantBuffers(11, 1, cbShockBlur.GetAddressOf());
}

void PostEffect::SetBlurPos(const DirectX::XMFLOAT3& position)
{
	Graphics& graphics = Graphics::Instance();
	Camera& camera     = Camera::Instance();

	ID3D11DeviceContext* dc = graphics.GetDeviceContext();

	// ビューポート
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	// 変換行列
	DirectX::XMMATRIX View       = DirectX::XMLoadFloat4x4(&camera.GetView());
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&camera.GetProjection());
	DirectX::XMMATRIX World      = DirectX::XMMatrixIdentity();

	DirectX::XMVECTOR ScreenPosition, WorldPosition;

	WorldPosition  = DirectX::XMLoadFloat3(&position);
	ScreenPosition = DirectX::XMVector3Project(
		WorldPosition,
		viewport.TopLeftX,
		viewport.TopLeftY,
		viewport.Width,
		viewport.Height,
		viewport.MinDepth,
		viewport.MaxDepth,
		Projection,
		View,
		World
	);

	DirectX::XMFLOAT3 blurPos;
	DirectX::XMStoreFloat3(&blurPos, ScreenPosition);

	shockBlur.centerTexel.x = blurPos.x / viewport.Width;
	shockBlur.centerTexel.y = blurPos.y / viewport.Height;
}
