#include <d3dcompiler.h>
#include <cassert>
#include "Direct3D.h"


//const int WINDOW_WIDTH = 800;  //�E�B���h�E�̕�
//const int WINDOW_HEIGHT = 600; //�E�B���h�E�̍���

//�ϐ�
namespace Direct3D
{

	ID3D11Device* pDevice_;		//�f�o�C�X
	ID3D11DeviceContext* pContext_;		//�f�o�C�X�R���e�L�X�g
	IDXGISwapChain* pSwapChain_;		//�X���b�v�`�F�C��
	ID3D11RenderTargetView* pRenderTargetView_;	//�����_�[�^�[�Q�b�g�r���[
	ID3D11Texture2D* pDepthStencil;			//�[�x�X�e���V��
	ID3D11DepthStencilView* pDepthStencilView;		//�[�x�X�e���V���r���[

	struct SHADER_BUNDLE {
		ID3D11VertexShader* pVertexShader_ = nullptr;	//���_�V�F�[�_�[
		ID3D11PixelShader* pPixelShader_ = nullptr;		//�s�N�Z���V�F�[�_�[
		ID3D11InputLayout* pVertexLayout_ = nullptr;	//���_�C���v�b�g���C�A�E�g
		ID3D11RasterizerState* pRasterizerState_ = nullptr;	//���X�^���C�U�[
	};
	SHADER_BUNDLE shader_Bandle[SHADER_MAX];

}



//������
HRESULT Direct3D::Initialize(int winW, int winH, HWND hWnd)
{
	HRESULT hr;
	///////////////////////////���낢�돀�����邽�߂̐ݒ�///////////////////////////////
	//���낢��Ȑݒ荀�ڂ��܂Ƃ߂��\����
	DXGI_SWAP_CHAIN_DESC scDesc;

	//�Ƃ肠�����S��0
	ZeroMemory(&scDesc, sizeof(scDesc));

	//�`���̃t�H�[�}�b�g
	scDesc.BufferDesc.Width = winW;		//��ʕ�
	scDesc.BufferDesc.Height = winH;	//��ʍ���
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// ���F�g���邩

	//FPS�i1/60�b��1��j
	scDesc.BufferDesc.RefreshRate.Numerator = 60;
	scDesc.BufferDesc.RefreshRate.Denominator = 1;

	//���̑�
	scDesc.Windowed = TRUE;			//�E�B���h�E���[�h���t���X�N���[����
	scDesc.OutputWindow = hWnd;		//�E�B���h�E�n���h��
	scDesc.BufferCount = 1;			//�o�b�N�o�b�t�@�̖���
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//�o�b�N�o�b�t�@�̎g��������ʂɕ`�悷�邽�߂�
	scDesc.SampleDesc.Count = 1;		//MSAA�i�A���`�G�C���A�X�j�̐ݒ�
	scDesc.SampleDesc.Quality = 0;		//�@�V

	////////////////��L�ݒ�����ƂɃf�o�C�X�A�R���e�L�X�g�A�X���b�v�`�F�C�����쐬////////////////////////
	D3D_FEATURE_LEVEL level;
	hr = D3D11CreateDeviceAndSwapChain(
		nullptr,				// �ǂ̃r�f�I�A�_�v�^���g�p���邩�H����Ȃ��nullptr��
		D3D_DRIVER_TYPE_HARDWARE,		// �h���C�o�̃^�C�v��n���B�ӂ���HARDWARE
		nullptr,				// ��L��D3D_DRIVER_TYPE_SOFTWARE�ɐݒ肵�Ȃ�������nullptr
		0,					// ���炩�̃t���O���w�肷��B�i�f�o�b�O����D3D11_CREATE_DEVICE_DEBUG�H�j
		nullptr,				// �f�o�C�X�A�R���e�L�X�g�̃��x����ݒ�Bnullptr�ɂ��Ƃ���OK
		0,					// ��̈����Ń��x�������w�肵����
		D3D11_SDK_VERSION,			// SDK�̃o�[�W�����B�K�����̒l
		&scDesc,				// ��ł��낢��ݒ肵���\����
		&pSwapChain_,				// ������������SwapChain�̃A�h���X���Ԃ��Ă���
		&pDevice_,				// ������������Device�A�h���X���Ԃ��Ă���
		&level,					// ������������Device�AContext�̃��x�����Ԃ��Ă���
		&pContext_);				// ������������Context�̃A�h���X���Ԃ��Ă���

	if (FAILED(hr)) {
		MessageBox(nullptr, "�����Ȃ̂̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return hr;
	}

	///////////////////////////�����_�[�^�[�Q�b�g�r���[�쐬///////////////////////////////
	//�X���b�v�`�F�[������o�b�N�o�b�t�@���擾�i�o�b�N�o�b�t�@ �� �����_�[�^�[�Q�b�g�j
	ID3D11Texture2D* pBackBuffer;
	pSwapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	//HRESULT hr;
	//�����_�[�^�[�Q�b�g�r���[���쐬
	hr = pDevice_->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView_);
	if (FAILED(hr)) {
		MessageBox(nullptr, "�����_�[�^�[�Q�b�g�r���[�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return hr;
	}

	//�ꎞ�I�Ƀo�b�N�o�b�t�@���擾���������Ȃ̂ŉ��
	pBackBuffer->Release();

	///////////////////////////�r���[�|�[�g�i�`��͈́j�ݒ�///////////////////////////////
	//�����_�����O���ʂ�\������͈�
	D3D11_VIEWPORT vp;
	vp.Width = (float)winW;	//��
	vp.Height = (float)winH;//����
	vp.MinDepth = 0.0f;	//��O
	vp.MaxDepth = 1.0f;	//��
	vp.TopLeftX = 0;	//��
	vp.TopLeftY = 0;	//��

	//////////////////////////////�k�x�X�e���X�r���[////////////////////////////////
	//�[�x�X�e���V���r���[�̍쐬
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = winW;
	descDepth.Height = winH;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	pDevice_->CreateTexture2D(&descDepth, NULL, &pDepthStencil);
	pDevice_->CreateDepthStencilView(pDepthStencil, NULL, &pDepthStencilView);

	//�����G�t�F�N�g���ɃO���r�f�I�ȈÂ��F�Ƃ��A���邢�F�Ƃ��ς����悤�ɂł���z��ɂ����
	//�u�����h�X�e�[�g
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;

	blendDesc.RenderTarget[0].BlendEnable = TRUE;					//�������g�����ǂ���
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;		//���`�悵�悤�Ƃ��Ă���́iSrc��source�̗��j
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;//���ɕ`�悳��Ă���́B��̂��BLEND_ONE�ɂ����
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;			//���̐F�𑫂��B�������邱�ƂŐF�̍������ł���I�ȁB�����w�肵�Ă�̂͐F*0.5������2�ɂȂ��Ă�I��

	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	pDevice_->CreateBlendState(&blendDesc, &pBlendState);

	float blendFactor[4] = { D3D11_BLEND_ZERO,D3D11_BLEND_ZERO ,D3D11_BLEND_ZERO ,D3D11_BLEND_ZERO };
	pContext_->OMSetBlendState(pBlendState, blendFactor, 0xfffffff);

	//�f�[�^����ʂɕ`�悷�邽�߂̈�ʂ�̐ݒ�i�p�C�v���C���j
	pContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);  // �f�[�^�̓��͎�ނ��w��
	pContext_->OMSetRenderTargets(1, &pRenderTargetView_, pDepthStencilView);            // �`����ݒ�
	//pContext_->OMSetRenderTargets(1, &pRenderTargetView_, nullptr);            // �`����ݒ�
	pContext_->RSSetViewports(1, &vp);

	//�V�F�[�_�[����
	hr = InitShader();

	if (FAILED(hr)) {
		MessageBox(nullptr, "�V�F�[�_�[�̏����Ɏ��s���܂���", "�G���[", MB_OK);
		return hr;
	}

	return S_OK;

}

HRESULT Direct3D::InitShader()
{

	if (FAILED(InitShader2D())) {
		MessageBox(nullptr, "2D�V�F�[�_�[�̏����Ɏ��s���܂���", "�G���[", MB_OK);
		return E_FAIL;

	}


	if (FAILED(InitShader3D())) {
		MessageBox(nullptr, "3D�V�F�[�_�[�̏����Ɏ��s���܂���", "�G���[", MB_OK);
		return E_FAIL;

	}

	if (FAILED(InitToonShader())) {
		MessageBox(nullptr, "toon�V�F�[�_�[�̏����Ɏ��s���܂���", "�G���[", MB_OK);
		return E_FAIL;

	}

	if (FAILED(InitOutlineShader())) {
		MessageBox(nullptr, "�A�E�g���C���V�F�[�_�[�̏����Ɏ��s���܂���", "�G���[", MB_OK);
		return E_FAIL;

	}

	if (FAILED(InitNormalMap())) {
		MessageBox(nullptr, "�m�[�}���}�b�v�̏����Ɏ��s���܂���", "�G���[", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

HRESULT Direct3D::InitShader2D()
{
	HRESULT hr;
	ID3DBlob* pCompileVS = nullptr;
	D3DCompileFromFile(L"Simple2D.hlsl", nullptr, nullptr, "VS", "vs_5_0", NULL, 0, &pCompileVS, NULL);
	assert(pCompileVS != nullptr);
	hr = pDevice_->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &(shader_Bandle[SHADER_2D].pVertexShader_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "���_�V�F�[�_�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return hr;
	}

	//���_�C���v�b�g���C�A�E�g
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },	//�ʒu
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::XMVECTOR) , D3D11_INPUT_PER_VERTEX_DATA, 0 },//UV���W
	};
	hr = pDevice_->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &(shader_Bandle[SHADER_2D].pVertexLayout_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "���_�C���v�b�g���C�A�E�g�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return hr;
	}

	SAFE_RELEASE(pCompileVS);

	// �s�N�Z���V�F�[�_�̍쐬�i�R���p�C���j
	ID3DBlob* pCompilePS = nullptr;
	D3DCompileFromFile(L"Simple2D.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);
	assert(pCompilePS != nullptr);
	hr = pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &(shader_Bandle[SHADER_2D].pPixelShader_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "�s�N�Z���V�F�[�_�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return hr;
	}


	SAFE_RELEASE(pCompilePS);

	//���X�^���C�U�쐬
	D3D11_RASTERIZER_DESC rdc = {};
	rdc.CullMode = D3D11_CULL_BACK; //��둤�͕`�悵�Ȃ�����
	rdc.FillMode = D3D11_FILL_SOLID;//���C���[�t���[��������邩�h�邩
	rdc.FrontCounterClockwise = FALSE; //���v���ɒ��_��`�悷�邩�BFALSE���Ǝ��v���ɓǂݍ��ނ���
	hr = pDevice_->CreateRasterizerState(&rdc, &(shader_Bandle[SHADER_2D].pRasterizerState_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "���X�^���C�U�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return hr;
	}

	////���ꂼ����f�o�C�X�R���e�L�X�g�ɃZ�b�g
	//pContext_->VSSetShader(shader_Bandle[SHADER_2D].pVertexShader_, NULL, 0);	//���_�V�F�[�_�[
	//pContext_->PSSetShader(shader_Bandle[SHADER_2D].pPixelShader_, NULL, 0);	//�s�N�Z���V�F�[�_�[
	//pContext_->IASetInputLayout(shader_Bandle[SHADER_2D].pVertexLayout_);	//���_�C���v�b�g���C�A�E�g
	//pContext_->RSSetState(shader_Bandle[SHADER_2D].pRasterizerState_);		//���X�^���C�U�[

	//SetShader(SHADER_3D);

	return S_OK;
}

HRESULT Direct3D::InitShader3D()
{
	HRESULT hr;
	ID3DBlob* pCompileVS = nullptr;
	D3DCompileFromFile(L"Simple3D.hlsl", nullptr, nullptr, "VS", "vs_5_0", NULL, 0, &pCompileVS, NULL);
	assert(pCompileVS != nullptr);
	hr = pDevice_->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &(shader_Bandle[SHADER_3D].pVertexShader_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "���_�V�F�[�_�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return hr;
	}

	//���_�C���v�b�g���C�A�E�g
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },	//�ʒu
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::XMVECTOR) , D3D11_INPUT_PER_VERTEX_DATA, 0 },//UV���W
		{ "NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(DirectX::XMVECTOR) * 2 ,	D3D11_INPUT_PER_VERTEX_DATA, 0 },//�@��
	};
	hr = pDevice_->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &(shader_Bandle[SHADER_3D].pVertexLayout_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "���_�C���v�b�g���C�A�E�g�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return hr;
	}

	SAFE_RELEASE(pCompileVS);

	// �s�N�Z���V�F�[�_�̍쐬�i�R���p�C���j
	ID3DBlob* pCompilePS = nullptr;
	D3DCompileFromFile(L"Simple3D.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);
	assert(pCompilePS != nullptr);
	hr = pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &(shader_Bandle[SHADER_3D].pPixelShader_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "�s�N�Z���V�F�[�_�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return hr;
	}


	SAFE_RELEASE(pCompilePS);

	//���X�^���C�U�쐬
	D3D11_RASTERIZER_DESC rdc = {};
	rdc.CullMode = D3D11_CULL_BACK; //��둤�͕`�悵�Ȃ�����
	rdc.FillMode = D3D11_FILL_SOLID;//���C���[�t���[��������邩�h�邩
	rdc.FrontCounterClockwise = FALSE; //���v���ɒ��_��`�悷�邩�BFALSE���Ǝ��v���ɓǂݍ��ނ���
	hr = pDevice_->CreateRasterizerState(&rdc, &(shader_Bandle[SHADER_3D].pRasterizerState_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "���X�^���C�U�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return hr;
	}

	////���ꂼ����f�o�C�X�R���e�L�X�g�ɃZ�b�g
	//pContext_->VSSetShader(shader_Bandle[SHADER_3D].pVertexShader_, NULL, 0);	//���_�V�F�[�_�[
	//pContext_->PSSetShader(shader_Bandle[SHADER_3D].pPixelShader_, NULL, 0);	//�s�N�Z���V�F�[�_�[
	//pContext_->IASetInputLayout(shader_Bandle[SHADER_3D].pVertexLayout_);	//���_�C���v�b�g���C�A�E�g
	//pContext_->RSSetState(shader_Bandle[SHADER_3D].pRasterizerState_);		//���X�^���C�U�[

	//SetShader(SHADER_3D);

	return S_OK;
}

HRESULT Direct3D::InitOutlineShader()
{
	HRESULT hr;
	ID3DBlob* pCompileVS = nullptr;
	D3DCompileFromFile(L"Outline.hlsl", nullptr, nullptr, "VS", "vs_5_0", NULL, 0, &pCompileVS, NULL);
	assert(pCompileVS != nullptr);

	hr = pDevice_->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &(shader_Bandle[SHADER_OUTLINE].pVertexShader_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "���_�V�F�[�_�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return hr;
	}

	//���_�C���v�b�g���C�A�E�g
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },	//�ʒu
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::XMVECTOR) , D3D11_INPUT_PER_VERTEX_DATA, 0 },//UV���W
		{ "NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(DirectX::XMVECTOR) * 2 ,	D3D11_INPUT_PER_VERTEX_DATA, 0 },//�@��
	};

	hr = pDevice_->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &(shader_Bandle[SHADER_OUTLINE].pVertexLayout_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "���_�C���v�b�g���C�A�E�g�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return hr;
	}

	SAFE_RELEASE(pCompileVS);

	// �s�N�Z���V�F�[�_�̍쐬�i�R���p�C���j
	ID3DBlob* pCompilePS = nullptr;
	D3DCompileFromFile(L"Outline.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);
	assert(pCompilePS != nullptr);
	hr = pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &(shader_Bandle[SHADER_OUTLINE].pPixelShader_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "�s�N�Z���V�F�[�_�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return hr;
	}


	SAFE_RELEASE(pCompilePS);

	//���X�^���C�U�쐬
	D3D11_RASTERIZER_DESC rdc = {};
	rdc.CullMode = D3D11_CULL_FRONT; //�\���͕`�悵�Ȃ�����
	rdc.FillMode = D3D11_FILL_SOLID;//���C���[�t���[��������邩�h�邩
	rdc.FrontCounterClockwise = FALSE; //���v���ɒ��_��`�悷�邩�BFALSE���Ǝ��v���ɓǂݍ��ނ���
	hr = pDevice_->CreateRasterizerState(&rdc, &(shader_Bandle[SHADER_OUTLINE].pRasterizerState_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "���X�^���C�U�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return hr;
	}

	////���ꂼ����f�o�C�X�R���e�L�X�g�ɃZ�b�g
	//pContext_->VSSetShader(shader_Bandle[SHADER_3D].pVertexShader_, NULL, 0);	//���_�V�F�[�_�[
	//pContext_->PSSetShader(shader_Bandle[SHADER_3D].pPixelShader_, NULL, 0);	//�s�N�Z���V�F�[�_�[
	//pContext_->IASetInputLayout(shader_Bandle[SHADER_3D].pVertexLayout_);	//���_�C���v�b�g���C�A�E�g
	//pContext_->RSSetState(shader_Bandle[SHADER_3D].pRasterizerState_);		//���X�^���C�U�[

	//SetShader(SHADER_3D);

	return S_OK;
}

//toon�摜�p��initShader
HRESULT Direct3D::InitToonShader()
{
	HRESULT hr;
	ID3DBlob* pCompileVS = nullptr;
	D3DCompileFromFile(L"Toon.hlsl", nullptr, nullptr, "VS", "vs_5_0", NULL, 0, &pCompileVS, NULL);
	assert(pCompileVS != nullptr);

	hr = pDevice_->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &(shader_Bandle[SHADER_TOON].pVertexShader_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "���_�V�F�[�_�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return hr;
	}

	//���_�C���v�b�g���C�A�E�g
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },	//�ʒu
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::XMVECTOR) , D3D11_INPUT_PER_VERTEX_DATA, 0 },//UV���W
		{ "NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(DirectX::XMVECTOR) * 2 ,	D3D11_INPUT_PER_VERTEX_DATA, 0 },//�@��
	};

	hr = pDevice_->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &(shader_Bandle[SHADER_TOON].pVertexLayout_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "���_�C���v�b�g���C�A�E�g�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return hr;
	}

	SAFE_RELEASE(pCompileVS);

	// �s�N�Z���V�F�[�_�̍쐬�i�R���p�C���j
	ID3DBlob* pCompilePS = nullptr;
	D3DCompileFromFile(L"Toon.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);
	assert(pCompilePS != nullptr);
	hr = pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &(shader_Bandle[SHADER_TOON].pPixelShader_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "�s�N�Z���V�F�[�_�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return hr;
	}


	SAFE_RELEASE(pCompilePS);

	//���X�^���C�U�쐬
	D3D11_RASTERIZER_DESC rdc = {};
	rdc.CullMode = D3D11_CULL_BACK; //��둤�͕`�悵�Ȃ�����
	rdc.FillMode = D3D11_FILL_SOLID;//���C���[�t���[��������邩�h�邩
	rdc.FrontCounterClockwise = FALSE; //���v���ɒ��_��`�悷�邩�BFALSE���Ǝ��v���ɓǂݍ��ނ���
	hr = pDevice_->CreateRasterizerState(&rdc, &(shader_Bandle[SHADER_TOON].pRasterizerState_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "���X�^���C�U�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return hr;
	}

	////���ꂼ����f�o�C�X�R���e�L�X�g�ɃZ�b�g
	//pContext_->VSSetShader(shader_Bandle[SHADER_3D].pVertexShader_, NULL, 0);	//���_�V�F�[�_�[
	//pContext_->PSSetShader(shader_Bandle[SHADER_3D].pPixelShader_, NULL, 0);	//�s�N�Z���V�F�[�_�[
	//pContext_->IASetInputLayout(shader_Bandle[SHADER_3D].pVertexLayout_);	//���_�C���v�b�g���C�A�E�g
	//pContext_->RSSetState(shader_Bandle[SHADER_3D].pRasterizerState_);		//���X�^���C�U�[

	//SetShader(SHADER_3D);

	return S_OK;
}

HRESULT Direct3D::InitNormalMap()
{
	HRESULT hr;
	ID3DBlob* pCompileVS = nullptr;
	D3DCompileFromFile(L"NormalMapping.hlsl", nullptr, nullptr, "VS", "vs_5_0", NULL, 0, &pCompileVS, NULL);
	assert(pCompileVS != nullptr);
	hr = pDevice_->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &(shader_Bandle[SHADER_NORMALMAP].pVertexShader_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "���_�V�F�[�_�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return hr;
	}

	//���_�C���v�b�g���C�A�E�g
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(DirectX::XMVECTOR) * 0 ,	 D3D11_INPUT_PER_VERTEX_DATA, 0 },//�ʒu
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, sizeof(DirectX::XMVECTOR) * 1 ,     D3D11_INPUT_PER_VERTEX_DATA, 0 },//UV���W
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(DirectX::XMVECTOR) * 2 ,	 D3D11_INPUT_PER_VERTEX_DATA, 0 },//�@��
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(DirectX::XMVECTOR) * 3 ,	 D3D11_INPUT_PER_VERTEX_DATA, 0 },//�ڐ�
	};
	hr = pDevice_->CreateInputLayout(layout, ARRAYSIZE(layout), pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &(shader_Bandle[SHADER_NORMALMAP].pVertexLayout_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "���_�C���v�b�g���C�A�E�g�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return hr;
	}

	SAFE_RELEASE(pCompileVS);

	// �s�N�Z���V�F�[�_�̍쐬�i�R���p�C���j
	ID3DBlob* pCompilePS = nullptr;
	D3DCompileFromFile(L"NormalMapping.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);
	assert(pCompilePS != nullptr);
	hr = pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &(shader_Bandle[SHADER_NORMALMAP].pPixelShader_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "�s�N�Z���V�F�[�_�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return hr;
	}


	SAFE_RELEASE(pCompilePS);

	//���X�^���C�U�쐬
	D3D11_RASTERIZER_DESC rdc = {};
	rdc.CullMode = D3D11_CULL_BACK; //��둤�͕`�悵�Ȃ�����
	rdc.FillMode = D3D11_FILL_SOLID;//���C���[�t���[��������邩�h�邩
	rdc.FrontCounterClockwise = FALSE; //���v���ɒ��_��`�悷�邩�BFALSE���Ǝ��v���ɓǂݍ��ނ���
	rdc.ScissorEnable = false;
	rdc.MultisampleEnable = false;
	hr = pDevice_->CreateRasterizerState(&rdc, &(shader_Bandle[SHADER_NORMALMAP].pRasterizerState_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "���X�^���C�U�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return hr;
	}

	////���ꂼ����f�o�C�X�R���e�L�X�g�ɃZ�b�g
	//pContext_->VSSetShader(shader_Bandle[SHADER_3D].pVertexShader_, NULL, 0);	//���_�V�F�[�_�[
	//pContext_->PSSetShader(shader_Bandle[SHADER_3D].pPixelShader_, NULL, 0);	//�s�N�Z���V�F�[�_�[
	//pContext_->IASetInputLayout(shader_Bandle[SHADER_3D].pVertexLayout_);	//���_�C���v�b�g���C�A�E�g
	//pContext_->RSSetState(shader_Bandle[SHADER_3D].pRasterizerState_);		//���X�^���C�U�[

	//SetShader(SHADER_3D);

	return S_OK;
}

void Direct3D::SetDepthBafferWriteEnable(bool alpha)
{
	//ON
	if (alpha)
	{
		//Z�o�b�t�@�i�f�v�X�X�e���V�����w�肷��j
		pContext_->OMSetRenderTargets(1, &pRenderTargetView_, pDepthStencilView);
	}

	//OFF
	else
	{
		pContext_->OMSetRenderTargets(1, &pRenderTargetView_, nullptr);
	}
}

void Direct3D::SetShader(SHADER_TYPE type)
{
	//���ꂼ��̃f�o�C�X�R���e�L�X�g�ɃZ�b�g
	pContext_->VSSetShader(shader_Bandle[type].pVertexShader_, NULL, 0);//���_�V�F�[�_�[
	pContext_->PSSetShader(shader_Bandle[type].pPixelShader_, NULL, 0);//�s�N�Z���V�F�[�_�[
	pContext_->IASetInputLayout(shader_Bandle[type].pVertexLayout_);//���_�C���v�b�g���C�A�E�g
	pContext_->RSSetState(shader_Bandle[type].pRasterizerState_);//���X�^���C�U�[
}



//�`��J�n
void Direct3D::BeginDraw()
{
	//�w�i�̐F
	float clearColor[4] = { 0.0f, 0.5f, 0.5f, 1.0f };//R,G,B,A

	//��ʂ��N���A
	pContext_->ClearRenderTargetView(pRenderTargetView_, clearColor);

	//�[�x�o�b�t�@�N���A
	pContext_->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

}



//�`��I��
void Direct3D::EndDraw()
{
	


	//�`�揈��


	//�X���b�v�i�o�b�N�o�b�t�@��\�ɕ\������j
	pSwapChain_->Present(0, 0);
}



//�������
void Direct3D::Release()
{

	for (int i = 0; i < SHADER_MAX; i++) {

		//�������
		SAFE_RELEASE(shader_Bandle[i].pRasterizerState_);
		SAFE_RELEASE(shader_Bandle[i].pVertexLayout_);
		SAFE_RELEASE(shader_Bandle[i].pPixelShader_);
		SAFE_RELEASE(shader_Bandle[i].pVertexShader_);
	}

	//�������
	SAFE_RELEASE(pRenderTargetView_);
	SAFE_RELEASE(pSwapChain_);
	SAFE_RELEASE(pContext_);
	SAFE_RELEASE(pDevice_);
}