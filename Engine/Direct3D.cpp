#include <d3dcompiler.h>
#include <cassert>
#include "Direct3D.h"


//const int WINDOW_WIDTH = 800;  //ウィンドウの幅
//const int WINDOW_HEIGHT = 600; //ウィンドウの高さ

//変数
namespace Direct3D
{

	ID3D11Device* pDevice_;		//デバイス
	ID3D11DeviceContext* pContext_;		//デバイスコンテキスト
	IDXGISwapChain* pSwapChain_;		//スワップチェイン
	ID3D11RenderTargetView* pRenderTargetView_;	//レンダーターゲットビュー
	ID3D11Texture2D* pDepthStencil;			//深度ステンシル
	ID3D11DepthStencilView* pDepthStencilView;		//深度ステンシルビュー

	struct SHADER_BUNDLE {
		ID3D11VertexShader* pVertexShader_ = nullptr;	//頂点シェーダー
		ID3D11PixelShader* pPixelShader_ = nullptr;		//ピクセルシェーダー
		ID3D11InputLayout* pVertexLayout_ = nullptr;	//頂点インプットレイアウト
		ID3D11RasterizerState* pRasterizerState_ = nullptr;	//ラスタライザー
	};
	SHADER_BUNDLE shader_Bandle[SHADER_MAX];

}



//初期化
HRESULT Direct3D::Initialize(int winW, int winH, HWND hWnd)
{
	HRESULT hr;
	///////////////////////////いろいろ準備するための設定///////////////////////////////
	//いろいろな設定項目をまとめた構造体
	DXGI_SWAP_CHAIN_DESC scDesc;

	//とりあえず全部0
	ZeroMemory(&scDesc, sizeof(scDesc));

	//描画先のフォーマット
	scDesc.BufferDesc.Width = winW;		//画面幅
	scDesc.BufferDesc.Height = winH;	//画面高さ
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 何色使えるか

	//FPS（1/60秒に1回）
	scDesc.BufferDesc.RefreshRate.Numerator = 60;
	scDesc.BufferDesc.RefreshRate.Denominator = 1;

	//その他
	scDesc.Windowed = TRUE;			//ウィンドウモードかフルスクリーンか
	scDesc.OutputWindow = hWnd;		//ウィンドウハンドル
	scDesc.BufferCount = 1;			//バックバッファの枚数
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//バックバッファの使い道＝画面に描画するために
	scDesc.SampleDesc.Count = 1;		//MSAA（アンチエイリアス）の設定
	scDesc.SampleDesc.Quality = 0;		//　〃

	////////////////上記設定をもとにデバイス、コンテキスト、スワップチェインを作成////////////////////////
	D3D_FEATURE_LEVEL level;
	hr = D3D11CreateDeviceAndSwapChain(
		nullptr,				// どのビデオアダプタを使用するか？既定ならばnullptrで
		D3D_DRIVER_TYPE_HARDWARE,		// ドライバのタイプを渡す。ふつうはHARDWARE
		nullptr,				// 上記をD3D_DRIVER_TYPE_SOFTWAREに設定しないかぎりnullptr
		0,					// 何らかのフラグを指定する。（デバッグ時はD3D11_CREATE_DEVICE_DEBUG？）
		nullptr,				// デバイス、コンテキストのレベルを設定。nullptrにしとけばOK
		0,					// 上の引数でレベルを何個指定したか
		D3D11_SDK_VERSION,			// SDKのバージョン。必ずこの値
		&scDesc,				// 上でいろいろ設定した構造体
		&pSwapChain_,				// 無事完成したSwapChainのアドレスが返ってくる
		&pDevice_,				// 無事完成したDeviceアドレスが返ってくる
		&level,					// 無事完成したDevice、Contextのレベルが返ってくる
		&pContext_);				// 無事完成したContextのアドレスが返ってくる

	if (FAILED(hr)) {
		MessageBox(nullptr, "いろんなのの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}

	///////////////////////////レンダーターゲットビュー作成///////////////////////////////
	//スワップチェーンからバックバッファを取得（バックバッファ ＝ レンダーターゲット）
	ID3D11Texture2D* pBackBuffer;
	pSwapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	//HRESULT hr;
	//レンダーターゲットビューを作成
	hr = pDevice_->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView_);
	if (FAILED(hr)) {
		MessageBox(nullptr, "レンダーターゲットビューの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}

	//一時的にバックバッファを取得しただけなので解放
	pBackBuffer->Release();

	///////////////////////////ビューポート（描画範囲）設定///////////////////////////////
	//レンダリング結果を表示する範囲
	D3D11_VIEWPORT vp;
	vp.Width = (float)winW;	//幅
	vp.Height = (float)winH;//高さ
	vp.MinDepth = 0.0f;	//手前
	vp.MaxDepth = 1.0f;	//奥
	vp.TopLeftX = 0;	//左
	vp.TopLeftY = 0;	//上

	//////////////////////////////震度ステルスビュー////////////////////////////////
	//深度ステンシルビューの作成
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

	//もしエフェクト毎にグラビデ的な暗い色とか、明るい色とか変えれるようにできる配列にすれば
	//ブレンドステート
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;

	blendDesc.RenderTarget[0].BlendEnable = TRUE;					//半透明使うかどうか
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;		//今描画しようとしてるもの（Srcはsourceの略）
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;//既に描画されてるもの。上のやつをBLEND_ONEにすると
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;			//上二つの色を足す。そうすることで色の合成ができる的な。上二つが指定してるのは色*0.5した物2つになってる的な

	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	pDevice_->CreateBlendState(&blendDesc, &pBlendState);

	float blendFactor[4] = { D3D11_BLEND_ZERO,D3D11_BLEND_ZERO ,D3D11_BLEND_ZERO ,D3D11_BLEND_ZERO };
	pContext_->OMSetBlendState(pBlendState, blendFactor, 0xfffffff);

	//データを画面に描画するための一通りの設定（パイプライン）
	pContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);  // データの入力種類を指定
	pContext_->OMSetRenderTargets(1, &pRenderTargetView_, pDepthStencilView);            // 描画先を設定
	//pContext_->OMSetRenderTargets(1, &pRenderTargetView_, nullptr);            // 描画先を設定
	pContext_->RSSetViewports(1, &vp);

	//シェーダー準備
	hr = InitShader();

	if (FAILED(hr)) {
		MessageBox(nullptr, "シェーダーの準備に失敗しました", "エラー", MB_OK);
		return hr;
	}

	return S_OK;

}

HRESULT Direct3D::InitShader()
{

	if (FAILED(InitShader2D())) {
		MessageBox(nullptr, "2Dシェーダーの準備に失敗しました", "エラー", MB_OK);
		return E_FAIL;

	}


	if (FAILED(InitShader3D())) {
		MessageBox(nullptr, "3Dシェーダーの準備に失敗しました", "エラー", MB_OK);
		return E_FAIL;

	}

	if (FAILED(InitToonShader())) {
		MessageBox(nullptr, "toonシェーダーの準備に失敗しました", "エラー", MB_OK);
		return E_FAIL;

	}

	if (FAILED(InitOutlineShader())) {
		MessageBox(nullptr, "アウトラインシェーダーの準備に失敗しました", "エラー", MB_OK);
		return E_FAIL;

	}

	if (FAILED(InitNormalMap())) {
		MessageBox(nullptr, "ノーマルマップの準備に失敗しました", "エラー", MB_OK);
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
		MessageBox(nullptr, "頂点シェーダの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}

	//頂点インプットレイアウト
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },	//位置
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::XMVECTOR) , D3D11_INPUT_PER_VERTEX_DATA, 0 },//UV座標
	};
	hr = pDevice_->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &(shader_Bandle[SHADER_2D].pVertexLayout_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "頂点インプットレイアウトの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}

	SAFE_RELEASE(pCompileVS);

	// ピクセルシェーダの作成（コンパイル）
	ID3DBlob* pCompilePS = nullptr;
	D3DCompileFromFile(L"Simple2D.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);
	assert(pCompilePS != nullptr);
	hr = pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &(shader_Bandle[SHADER_2D].pPixelShader_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "ピクセルシェーダの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}


	SAFE_RELEASE(pCompilePS);

	//ラスタライザ作成
	D3D11_RASTERIZER_DESC rdc = {};
	rdc.CullMode = D3D11_CULL_BACK; //後ろ側は描画しない処理
	rdc.FillMode = D3D11_FILL_SOLID;//ワイヤーフレームだけ作るか塗るか
	rdc.FrontCounterClockwise = FALSE; //時計回りに頂点を描画するか。FALSEだと時計回りに読み込むする
	hr = pDevice_->CreateRasterizerState(&rdc, &(shader_Bandle[SHADER_2D].pRasterizerState_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "ラスタライザの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}

	////それぞれをデバイスコンテキストにセット
	//pContext_->VSSetShader(shader_Bandle[SHADER_2D].pVertexShader_, NULL, 0);	//頂点シェーダー
	//pContext_->PSSetShader(shader_Bandle[SHADER_2D].pPixelShader_, NULL, 0);	//ピクセルシェーダー
	//pContext_->IASetInputLayout(shader_Bandle[SHADER_2D].pVertexLayout_);	//頂点インプットレイアウト
	//pContext_->RSSetState(shader_Bandle[SHADER_2D].pRasterizerState_);		//ラスタライザー

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
		MessageBox(nullptr, "頂点シェーダの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}

	//頂点インプットレイアウト
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },	//位置
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::XMVECTOR) , D3D11_INPUT_PER_VERTEX_DATA, 0 },//UV座標
		{ "NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(DirectX::XMVECTOR) * 2 ,	D3D11_INPUT_PER_VERTEX_DATA, 0 },//法線
	};
	hr = pDevice_->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &(shader_Bandle[SHADER_3D].pVertexLayout_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "頂点インプットレイアウトの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}

	SAFE_RELEASE(pCompileVS);

	// ピクセルシェーダの作成（コンパイル）
	ID3DBlob* pCompilePS = nullptr;
	D3DCompileFromFile(L"Simple3D.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);
	assert(pCompilePS != nullptr);
	hr = pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &(shader_Bandle[SHADER_3D].pPixelShader_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "ピクセルシェーダの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}


	SAFE_RELEASE(pCompilePS);

	//ラスタライザ作成
	D3D11_RASTERIZER_DESC rdc = {};
	rdc.CullMode = D3D11_CULL_BACK; //後ろ側は描画しない処理
	rdc.FillMode = D3D11_FILL_SOLID;//ワイヤーフレームだけ作るか塗るか
	rdc.FrontCounterClockwise = FALSE; //時計回りに頂点を描画するか。FALSEだと時計回りに読み込むする
	hr = pDevice_->CreateRasterizerState(&rdc, &(shader_Bandle[SHADER_3D].pRasterizerState_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "ラスタライザの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}

	////それぞれをデバイスコンテキストにセット
	//pContext_->VSSetShader(shader_Bandle[SHADER_3D].pVertexShader_, NULL, 0);	//頂点シェーダー
	//pContext_->PSSetShader(shader_Bandle[SHADER_3D].pPixelShader_, NULL, 0);	//ピクセルシェーダー
	//pContext_->IASetInputLayout(shader_Bandle[SHADER_3D].pVertexLayout_);	//頂点インプットレイアウト
	//pContext_->RSSetState(shader_Bandle[SHADER_3D].pRasterizerState_);		//ラスタライザー

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
		MessageBox(nullptr, "頂点シェーダの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}

	//頂点インプットレイアウト
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },	//位置
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::XMVECTOR) , D3D11_INPUT_PER_VERTEX_DATA, 0 },//UV座標
		{ "NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(DirectX::XMVECTOR) * 2 ,	D3D11_INPUT_PER_VERTEX_DATA, 0 },//法線
	};

	hr = pDevice_->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &(shader_Bandle[SHADER_OUTLINE].pVertexLayout_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "頂点インプットレイアウトの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}

	SAFE_RELEASE(pCompileVS);

	// ピクセルシェーダの作成（コンパイル）
	ID3DBlob* pCompilePS = nullptr;
	D3DCompileFromFile(L"Outline.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);
	assert(pCompilePS != nullptr);
	hr = pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &(shader_Bandle[SHADER_OUTLINE].pPixelShader_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "ピクセルシェーダの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}


	SAFE_RELEASE(pCompilePS);

	//ラスタライザ作成
	D3D11_RASTERIZER_DESC rdc = {};
	rdc.CullMode = D3D11_CULL_FRONT; //表側は描画しない処理
	rdc.FillMode = D3D11_FILL_SOLID;//ワイヤーフレームだけ作るか塗るか
	rdc.FrontCounterClockwise = FALSE; //時計回りに頂点を描画するか。FALSEだと時計回りに読み込むする
	hr = pDevice_->CreateRasterizerState(&rdc, &(shader_Bandle[SHADER_OUTLINE].pRasterizerState_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "ラスタライザの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}

	////それぞれをデバイスコンテキストにセット
	//pContext_->VSSetShader(shader_Bandle[SHADER_3D].pVertexShader_, NULL, 0);	//頂点シェーダー
	//pContext_->PSSetShader(shader_Bandle[SHADER_3D].pPixelShader_, NULL, 0);	//ピクセルシェーダー
	//pContext_->IASetInputLayout(shader_Bandle[SHADER_3D].pVertexLayout_);	//頂点インプットレイアウト
	//pContext_->RSSetState(shader_Bandle[SHADER_3D].pRasterizerState_);		//ラスタライザー

	//SetShader(SHADER_3D);

	return S_OK;
}

//toon画像用のinitShader
HRESULT Direct3D::InitToonShader()
{
	HRESULT hr;
	ID3DBlob* pCompileVS = nullptr;
	D3DCompileFromFile(L"Toon.hlsl", nullptr, nullptr, "VS", "vs_5_0", NULL, 0, &pCompileVS, NULL);
	assert(pCompileVS != nullptr);

	hr = pDevice_->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &(shader_Bandle[SHADER_TOON].pVertexShader_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "頂点シェーダの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}

	//頂点インプットレイアウト
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },	//位置
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::XMVECTOR) , D3D11_INPUT_PER_VERTEX_DATA, 0 },//UV座標
		{ "NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(DirectX::XMVECTOR) * 2 ,	D3D11_INPUT_PER_VERTEX_DATA, 0 },//法線
	};

	hr = pDevice_->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &(shader_Bandle[SHADER_TOON].pVertexLayout_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "頂点インプットレイアウトの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}

	SAFE_RELEASE(pCompileVS);

	// ピクセルシェーダの作成（コンパイル）
	ID3DBlob* pCompilePS = nullptr;
	D3DCompileFromFile(L"Toon.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);
	assert(pCompilePS != nullptr);
	hr = pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &(shader_Bandle[SHADER_TOON].pPixelShader_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "ピクセルシェーダの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}


	SAFE_RELEASE(pCompilePS);

	//ラスタライザ作成
	D3D11_RASTERIZER_DESC rdc = {};
	rdc.CullMode = D3D11_CULL_BACK; //後ろ側は描画しない処理
	rdc.FillMode = D3D11_FILL_SOLID;//ワイヤーフレームだけ作るか塗るか
	rdc.FrontCounterClockwise = FALSE; //時計回りに頂点を描画するか。FALSEだと時計回りに読み込むする
	hr = pDevice_->CreateRasterizerState(&rdc, &(shader_Bandle[SHADER_TOON].pRasterizerState_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "ラスタライザの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}

	////それぞれをデバイスコンテキストにセット
	//pContext_->VSSetShader(shader_Bandle[SHADER_3D].pVertexShader_, NULL, 0);	//頂点シェーダー
	//pContext_->PSSetShader(shader_Bandle[SHADER_3D].pPixelShader_, NULL, 0);	//ピクセルシェーダー
	//pContext_->IASetInputLayout(shader_Bandle[SHADER_3D].pVertexLayout_);	//頂点インプットレイアウト
	//pContext_->RSSetState(shader_Bandle[SHADER_3D].pRasterizerState_);		//ラスタライザー

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
		MessageBox(nullptr, "頂点シェーダの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}

	//頂点インプットレイアウト
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(DirectX::XMVECTOR) * 0 ,	 D3D11_INPUT_PER_VERTEX_DATA, 0 },//位置
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, sizeof(DirectX::XMVECTOR) * 1 ,     D3D11_INPUT_PER_VERTEX_DATA, 0 },//UV座標
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(DirectX::XMVECTOR) * 2 ,	 D3D11_INPUT_PER_VERTEX_DATA, 0 },//法線
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(DirectX::XMVECTOR) * 3 ,	 D3D11_INPUT_PER_VERTEX_DATA, 0 },//接線
	};
	hr = pDevice_->CreateInputLayout(layout, ARRAYSIZE(layout), pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &(shader_Bandle[SHADER_NORMALMAP].pVertexLayout_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "頂点インプットレイアウトの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}

	SAFE_RELEASE(pCompileVS);

	// ピクセルシェーダの作成（コンパイル）
	ID3DBlob* pCompilePS = nullptr;
	D3DCompileFromFile(L"NormalMapping.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);
	assert(pCompilePS != nullptr);
	hr = pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &(shader_Bandle[SHADER_NORMALMAP].pPixelShader_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "ピクセルシェーダの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}


	SAFE_RELEASE(pCompilePS);

	//ラスタライザ作成
	D3D11_RASTERIZER_DESC rdc = {};
	rdc.CullMode = D3D11_CULL_BACK; //後ろ側は描画しない処理
	rdc.FillMode = D3D11_FILL_SOLID;//ワイヤーフレームだけ作るか塗るか
	rdc.FrontCounterClockwise = FALSE; //時計回りに頂点を描画するか。FALSEだと時計回りに読み込むする
	rdc.ScissorEnable = false;
	rdc.MultisampleEnable = false;
	hr = pDevice_->CreateRasterizerState(&rdc, &(shader_Bandle[SHADER_NORMALMAP].pRasterizerState_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "ラスタライザの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}

	////それぞれをデバイスコンテキストにセット
	//pContext_->VSSetShader(shader_Bandle[SHADER_3D].pVertexShader_, NULL, 0);	//頂点シェーダー
	//pContext_->PSSetShader(shader_Bandle[SHADER_3D].pPixelShader_, NULL, 0);	//ピクセルシェーダー
	//pContext_->IASetInputLayout(shader_Bandle[SHADER_3D].pVertexLayout_);	//頂点インプットレイアウト
	//pContext_->RSSetState(shader_Bandle[SHADER_3D].pRasterizerState_);		//ラスタライザー

	//SetShader(SHADER_3D);

	return S_OK;
}

void Direct3D::SetDepthBafferWriteEnable(bool alpha)
{
	//ON
	if (alpha)
	{
		//Zバッファ（デプスステンシルを指定する）
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
	//それぞれのデバイスコンテキストにセット
	pContext_->VSSetShader(shader_Bandle[type].pVertexShader_, NULL, 0);//頂点シェーダー
	pContext_->PSSetShader(shader_Bandle[type].pPixelShader_, NULL, 0);//ピクセルシェーダー
	pContext_->IASetInputLayout(shader_Bandle[type].pVertexLayout_);//頂点インプットレイアウト
	pContext_->RSSetState(shader_Bandle[type].pRasterizerState_);//ラスタライザー
}



//描画開始
void Direct3D::BeginDraw()
{
	//背景の色
	float clearColor[4] = { 0.0f, 0.5f, 0.5f, 1.0f };//R,G,B,A

	//画面をクリア
	pContext_->ClearRenderTargetView(pRenderTargetView_, clearColor);

	//深度バッファクリア
	pContext_->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

}



//描画終了
void Direct3D::EndDraw()
{
	


	//描画処理


	//スワップ（バックバッファを表に表示する）
	pSwapChain_->Present(0, 0);
}



//解放処理
void Direct3D::Release()
{

	for (int i = 0; i < SHADER_MAX; i++) {

		//解放処理
		SAFE_RELEASE(shader_Bandle[i].pRasterizerState_);
		SAFE_RELEASE(shader_Bandle[i].pVertexLayout_);
		SAFE_RELEASE(shader_Bandle[i].pPixelShader_);
		SAFE_RELEASE(shader_Bandle[i].pVertexShader_);
	}

	//解放処理
	SAFE_RELEASE(pRenderTargetView_);
	SAFE_RELEASE(pSwapChain_);
	SAFE_RELEASE(pContext_);
	SAFE_RELEASE(pDevice_);
}