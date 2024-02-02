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

	////////////�摜�ǂݍ��ݕ���////////////
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
		MessageBox(nullptr, "�摜�̃��[�h�Ɏ��s���܂���", "�G���[", MB_OK);
		return E_FAIL;
	}

	//�T���v���[�̍쐬
	D3D11_SAMPLER_DESC  SamDesc;
	ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));

	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;//LINEAR���Ƒ傫�������Ƃ��ɂڂ������BPOINT���ƃ{�P�Ȃ��悤�ɂȂ�炵��
	
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;//WRAP�ɂ���ƃe�N�X�`�������̃��f���̑傫������]�������������[�v����悤�ɂȂ�i�H�j
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;//MIRROR�ɂ���Ɨ��\�\���݂����Ȋ����Ń��[�v����BWRAP�ƈ���ċ���(�e�N�X�`���I�ȋ���)���o�Ȃ����瑐���Ƃ����R���ɗL��
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;//���Ƃ��烋�[�v���Ă�摜�Ȃ�WRAP�ł悵

	hr = Direct3D::pDevice_->CreateSamplerState(&SamDesc, &pSampler_);
	if (FAILED(hr)) {
		MessageBox(nullptr, "�T���v���[�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return E_FAIL;
	}

	//�V�F�[�_�[���\�[�X�r���[
	D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
	srv.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv.Texture2D.MipLevels = 1;

	hr = CreateShaderResourceView(Direct3D::pDevice_, image.GetImages(), image.GetImageCount(), metadata, &pSRV_);
	
	if (FAILED(hr)) {
		MessageBox(nullptr, "�V�F�[�_�[���\�[�X�r���[�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

void Texture::Release()
{
	SAFE_RELEASE(pSRV_);
	SAFE_RELEASE(pSampler_);
}
