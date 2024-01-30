#include "Sprite.h"

Sprite::Sprite() :
	vertexNum_(0), vertices_{}, indexNum_(0), index_(0),
	pVertexBuffer_(nullptr), pIndexBuffer_(nullptr), pConstantBuffer_(nullptr), pTexture_(nullptr)
{
	//index_ = { 0, 2, 3, 0, 1, 2 };
	indexNum_ = 6;
}

Sprite::~Sprite()
{
	pTexture_->Release();
	SAFE_DELETE(pTexture_);

	SAFE_RELEASE(pConstantBuffer_);
	SAFE_RELEASE(pIndexBuffer_);
	SAFE_RELEASE(pVertexBuffer_);
}

HRESULT Sprite::Initialize()
{
	
	CreateVertexBuffer();
	CreateIndexBuffer();
	CreateConstantBuffer();

	//LoadTexture();

	return S_OK;
}

HRESULT Sprite::Load(string fileName)
{
	if (FAILED(LoadTexture(fileName))) {
		return E_FAIL;
	}

	CreateVertexBuffer();
	CreateIndexBuffer();
	
	if (FAILED(CreateConstantBuffer())) {
		return E_FAIL;
	}

	return S_OK;
}

void Sprite::Draw(Transform& transform)
{
	Direct3D::SetShader(SHADER_2D);

	transform.Calclation();//トランスフォームを計算
	PassDataToCB(transform.GetWorldMatrix());
	SetBufferToPipeline();
}

void Sprite::Draw(Transform& transform, RECT rect, float alpha)
{
	//いろいろ設定
	Direct3D::SetShader(SHADER_TYPE::SHADER_2D);
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	Direct3D::pContext_->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);
	Direct3D::pContext_->VSSetConstantBuffers(0, 1, &pConstantBuffer_);
	Direct3D::pContext_->PSSetConstantBuffers(0, 1, &pConstantBuffer_);
	Direct3D::SetDepthBafferWriteEnable(false);
	// インデックスバッファーをセット
	stride = sizeof(int);
	offset = 0;
	Direct3D::pContext_->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R32_UINT, 0);

	// パラメータの受け渡し
	D3D11_MAPPED_SUBRESOURCE pdata;
	CONSTANT_BUFFER cb;

	transform.Calclation();//トランスフォームを計算

	//表示するサイズに合わせる
	XMMATRIX cut = XMMatrixScaling((float)rect.right, (float)rect.bottom, 1);

	//画面に合わせる
	XMMATRIX view = XMMatrixScaling(1.0f / Direct3D::screenSize.cx, 1.0f / Direct3D::screenSize.cy, 1.0f);

	//最終的な行列
	XMMATRIX world = cut * transform.matScale_ * transform.matRotate_ * view * transform.matTranslate_;
	cb.world = XMMatrixTranspose(world);

	// テクスチャ座標変換行列を渡す
	XMMATRIX mTexTrans = XMMatrixTranslation((float)rect.left / (float)pTexture_->GetTextureSize().x,
		(float)rect.top / (float)pTexture_->GetTextureSize().y, 0.0f);
	XMMATRIX mTexScale = XMMatrixScaling((float)rect.right / (float)pTexture_->GetTextureSize().x,
		(float)rect.bottom / (float)pTexture_->GetTextureSize().y, 1.0f);
	XMMATRIX mTexel = mTexScale * mTexTrans;
	cb.uvTrans = XMMatrixTranspose(mTexel);

	cb.color = XMFLOAT4(1, 1, 1, alpha);

	Direct3D::pContext_->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);	// GPUからのリソースアクセスを一時止める
	memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&cb), sizeof(cb));		// リソースへ値を送る


	ID3D11SamplerState* pSampler = pTexture_->GetSampler();
	Direct3D::pContext_->PSSetSamplers(0, 1, &pSampler);

	ID3D11ShaderResourceView* pSRV = pTexture_->GetSRV();
	Direct3D::pContext_->PSSetShaderResources(0, 1, &pSRV);

	Direct3D::pContext_->Unmap(pConstantBuffer_, 0);	// GPUからのリソースアクセスを再開

	SetBufferToPipeline();

	//ポリゴンメッシュを描画する
	Direct3D::pContext_->DrawIndexed(indexNum, 0, 0);

	Direct3D::SetShader(SHADER_TYPE::SHADER_3D);

	Direct3D::SetDepthBafferWriteEnable(true);
}

void Sprite::Release()
{
	pTexture_->Release();
	SAFE_DELETE(pTexture_);

	SAFE_RELEASE(pConstantBuffer_);
	SAFE_RELEASE(pIndexBuffer_);
	SAFE_RELEASE(pVertexBuffer_);
}


void Sprite::InitVertexData()
{
	// 頂点情報
	vertices_ =
	{
		//位置、UV座標
		{ XMVectorSet(-1.0f,  1.0f, 0.0f, 0.0f),XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f) },	// 四角形の頂点（左上）
		{ XMVectorSet(1.0f,  1.0f, 0.0f, 0.0f),XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f) },	// 四角形の頂点（右上）
		{ XMVectorSet(1.0f, -1.0f, 0.0f, 0.0f),XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f) },	// 四角形の頂点（右下）
		{ XMVectorSet(-1.0f, -1.0f, 0.0f, 0.0f),XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) },	// 四角形の頂点（左下）
	};

	vertexNum_ = vertices_.size();
}

HRESULT Sprite::CreateVertexBuffer()
{

	InitVertexData();

	// 頂点データ用バッファの設定
	D3D11_BUFFER_DESC bd_vertex;
	bd_vertex.ByteWidth = vertexNum_ * sizeof(VERTEX);
	bd_vertex.Usage = D3D11_USAGE_DEFAULT;
	bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd_vertex.CPUAccessFlags = 0;
	bd_vertex.MiscFlags = 0;
	bd_vertex.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA data_vertex;
	data_vertex.pSysMem = &vertices_.front();
	HRESULT hr;
	hr = Direct3D::pDevice_->CreateBuffer(&bd_vertex, &data_vertex, &pVertexBuffer_);
	if (FAILED(hr)) {
		//エラー処理
		MessageBox(nullptr, "頂点バッファの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}

	return S_OK;
}

void Sprite::InitIndexData()
{
	index_ = { 0,2,3, 0,1,2 };
}

HRESULT Sprite::CreateIndexBuffer()
{

	InitIndexData();

	// インデックスバッファを生成する
	D3D11_BUFFER_DESC   bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(index_);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &index_.front();
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	HRESULT hr;
	hr = Direct3D::pDevice_->CreateBuffer(&bd, &InitData, &pIndexBuffer_);
	if (FAILED(hr)) {
		MessageBox(nullptr, "インデックスバッファの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}

	return S_OK;
}

HRESULT Sprite::CreateConstantBuffer()
{
	//コンスタントバッファ作成
	D3D11_BUFFER_DESC cb;
	cb.ByteWidth = sizeof(CONSTANT_BUFFER);
	cb.Usage = D3D11_USAGE_DYNAMIC;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;

	HRESULT hr;
	hr = Direct3D::pDevice_->CreateBuffer(&cb, nullptr, &pConstantBuffer_);
	if (FAILED(hr)) {
		MessageBox(nullptr, "コンスタントバッファの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}
	return S_OK;
}

HRESULT Sprite::LoadTexture(string fileName)
{
	pTexture_ = new Texture;
	HRESULT hr;
	string fname = "Assets\\" + fileName;
	hr = pTexture_->Load(fname);
	if (FAILED(hr))
	{
		MessageBox(NULL, "テクスチャの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}
	return S_OK;
}

void Sprite::PassDataToCB(DirectX::XMMATRIX worldMatrix)
{
	//コンスタントバッファに渡す情報
	D3D11_MAPPED_SUBRESOURCE pdata;
	CONSTANT_BUFFER cb;
	cb.matNormal = XMMatrixTranspose(worldMatrix);	//ワールド行列を渡す
	Direct3D::pContext_->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);	// GPUからのデータアクセスを止める
	memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&cb), sizeof(cb));	// データを値を送る

	ID3D11SamplerState* pSampler = pTexture_->GetSampler();	//サンプラーをシェーダーに
	Direct3D::pContext_->PSSetSamplers(0, 1, &pSampler);

	ID3D11ShaderResourceView* pSRV = pTexture_->GetSRV();	//テクスチャをシェーダーに
	Direct3D::pContext_->PSSetShaderResources(0, 1, &pSRV);

	Direct3D::pContext_->Unmap(pConstantBuffer_, 0);	//再開
}

void Sprite::SetBufferToPipeline()
{
	//頂点バッファ
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	Direct3D::pContext_->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

	// インデックスバッファーをセット
	stride = sizeof(int);
	offset = 0;
	Direct3D::pContext_->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R32_UINT, 0);

	//コンスタントバッファ
	Direct3D::pContext_->VSSetConstantBuffers(0, 1, &pConstantBuffer_);	//頂点シェーダー用	
	Direct3D::pContext_->PSSetConstantBuffers(0, 1, &pConstantBuffer_);	//ピクセルシェーダー用

	Direct3D::pContext_->DrawIndexed(indexNum_, 0, 0); //頂点の数（ポリゴン数×３）
}
