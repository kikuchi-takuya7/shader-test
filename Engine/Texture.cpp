#include "Texture.h"
#include "Direct3D.h"
#include<DirectXTex.h>

#pragma comment(lib,"DirectXTex.lib")


Texture::Texture() :pSampler_(nullptr), pSRV_(nullptr)
{
}

Texture::~Texture()
{
	Release();
}

HRESULT Texture::Load(string filename)
{
	using namespace DirectX;

	////////////画像読み込み部分////////////
	wchar_t wtext[FILENAME_MAX];
	size_t ret;
	mbstowcs_s(&ret, wtext, filename.c_str(), filename.length());
	
	TexMetadata metadata;
	ScratchImage image;
	HRESULT hr;
	hr = LoadFromWICFile(wtext, WIC_FLAGS::WIC_FLAGS_NONE, &metadata, image);
	//imgSize_ = XMFLOAT2{ (float)image.GetImages()->width, (float)image.GetImages()->height };
	///////////////////////////////////////////

	if (FAILED(hr)) {
		MessageBox(nullptr, "画像のロードに失敗しました", "エラー", MB_OK);
		return E_FAIL;
	}

	//サンプラーの作成
	D3D11_SAMPLER_DESC  SamDesc;
	ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));

	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;//LINEARだと大きくしたときにぼかされる。POINTだとボケないようになるらしい
	
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;//WRAPにするとテクスチャがそのモデルの大きさから余った分だけループするようになる（？）
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;//MIRRORにすると裏表表裏みたいな感じでループする。WRAPと違って境目(テクスチャ的な境目)が出ないから草原とか自然物に有効
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;//もとからループしてる画像ならWRAPでよし

	hr = Direct3D::pDevice_->CreateSamplerState(&SamDesc, &pSampler_);
	if (FAILED(hr)) {
		MessageBox(nullptr, "サンプラーの作成に失敗しました", "エラー", MB_OK);
		return E_FAIL;
	}

	//シェーダーリソースビュー
	D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
	srv.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv.Texture2D.MipLevels = 1;

	hr = CreateShaderResourceView(Direct3D::pDevice_, image.GetImages(), image.GetImageCount(), metadata, &pSRV_);
	
	if (FAILED(hr)) {
		MessageBox(nullptr, "シェーダーリソースビューの作成に失敗しました", "エラー", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

void Texture::Release()
{
	SAFE_RELEASE(pSRV_);
	SAFE_RELEASE(pSampler_);
}
