#include "Quad.h"

Quad::Quad() :
	pVertexBuffer_(nullptr), pIndexBuffer_(nullptr), pConstantBuffer_(nullptr), pTexture_(nullptr), hr_(E_FAIL), vertexNum_(0), indexNum_(0)
{
}

Quad::~Quad()
{
	pTexture_->Release();
	SAFE_DELETE(pTexture_);

	SAFE_RELEASE(pConstantBuffer_);
	SAFE_RELEASE(pIndexBuffer_);
	SAFE_RELEASE(pVertexBuffer_);
}

HRESULT Quad::Initialize()
{
	MakeVerBf();
	if (FAILED(hr_)) {
		//エラー処理
		MessageBox(nullptr, "頂点バッファの作成に失敗しました", "エラー", MB_OK);
		return hr_;
	}


	MakeIndBf();
	if (FAILED(hr_)) {
		MessageBox(nullptr, "インデックスバッファの作成に失敗しました", "エラー", MB_OK);
		return hr_;
	}

	MakeConBf();
	if (FAILED(hr_)) {
		MessageBox(nullptr, "コンスタントバッファの作成に失敗しました", "エラー", MB_OK);
		return hr_;
	}

	pTexture_ = new Texture;
	pTexture_->Load("Assets\\Dice.png");


	return S_OK;
}

void Quad::Draw(Transform& transform)
{

	////コンスタントバッファに渡す情報
	//XMVECTOR position = { 0, 3, -10, 0 };	//カメラの位置
	//XMVECTOR target = { 0, 0, 0, 0 };	//カメラの焦点

	//ビュー行列 カメラの世界の各頂点を決めるのがビュー行列。XMVectorSetでカメラの上下がどこかを決める
	//XMMATRIX view = XMMatrixLookAtLH(position, target, XMVectorSet(0, 1, 0, 0));	

	//射影行列　遠近感を使用するのに使う：順番大事　回転してからかけるのかかけてから回転するのかで
	//XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, 800.0f / 600.0f, 0.1f, 100.0f);//π÷4という意味で、視野角45度ってこと。PI(π) DIV(÷) 4（πラジアンは180度)
	// 800.0f / 600.0fはアスペクト比　　
	//0.1f, 100.0fニア(近)クリッピング面.ファー(遠)クリッピング面:世界は平らだから描画距離を決める.手前も残さないとスペースなくて計算できない.ゲームによるが差はなるべく小さくないとZfightingが起きる

	Direct3D::SetShader(SHADER_3D);

	transform.Calclation();//トランスフォームを計算
	SetMap(transform);

	SetTexture();
	
	SetPipeline();

	Direct3D::pContext_->DrawIndexed(indexNum_, 0, 0); //インデックス情報の数は何個数字を入れてるか

}

void Quad::Release()
{
	//pTexture_->Release();
	SAFE_RELEASE(pTexture_);
	SAFE_DELETE(pTexture_);

	SAFE_RELEASE(pConstantBuffer_);
	SAFE_RELEASE(pIndexBuffer_);
	SAFE_RELEASE(pVertexBuffer_);
	
	
}

void Quad::MakeVerBf()
{
	
	SetVartices();

	// 頂点データ用バッファの設定
	D3D11_BUFFER_DESC bd_vertex;
	bd_vertex.ByteWidth = sizeof(VERTEX) * vertexNum_;
	bd_vertex.Usage = D3D11_USAGE_DEFAULT;
	bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd_vertex.CPUAccessFlags = 0;
	bd_vertex.MiscFlags = 0;
	bd_vertex.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA data_vertex;
	data_vertex.pSysMem = vertices_.data();
	hr_ = Direct3D::pDevice_->CreateBuffer(&bd_vertex, &data_vertex, &pVertexBuffer_);
}

void Quad::SetVartices()
{
	// 頂点情報
	vertices_ =
	{
		{XMVectorSet(-1.0f,  1.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f) /*, XMVectorSet(0.0f,0.0f,-1.0f, 0.0f)*/},	// 四角形の頂点（左上）
		{XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f),   XMVectorSet(0.25f, 0.0f, 0.0f, 0.0f)/*, XMVectorSet(0.0f,0.0f,-1.0f, 0.0f)*/},	// 四角形の頂点（右上）
		{XMVectorSet(1.0f, -1.0f, 0.0f, 0.0f),  XMVectorSet(0.25f, 0.5f, 0.0f, 0.0f)/*, XMVectorSet(0.0f,0.0f,-1.0f, 0.0f)*/},	// 四角形の頂点（右下）
		{XMVectorSet(-1.0f, -1.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 0.5f, 0.0f, 0.0f) /*, XMVectorSet(0.0f,0.0f,-1.0f, 0.0f)*/},	// 四角形の頂点（左下）		
		//{XMVectorSet(0.0f, 2.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) },		// 追加した点
	};

	vertexNum_ = vertices_.size();
}

void Quad::SetIndex()
{
	//インデックス情報
	index_ = { 0,2,3, 0,1,2 };
	indexNum_ = index_.size();
}

void Quad::MakeIndBf()
{
	SetIndex();

	// インデックスバッファを生成する
	D3D11_BUFFER_DESC   bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(int) * indexNum_;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = index_.data();
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	hr_ = Direct3D::pDevice_->CreateBuffer(&bd, &InitData, &pIndexBuffer_);
	
}

void Quad::MakeConBf()
{
	//コンスタントバッファ作成
	D3D11_BUFFER_DESC cb;
	cb.ByteWidth = sizeof(CONSTANT_BUFFER);
	cb.Usage = D3D11_USAGE_DYNAMIC;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;

	// コンスタントバッファの作成
	hr_ = Direct3D::pDevice_->CreateBuffer(&cb, nullptr, &pConstantBuffer_);
	
}

void Quad::SetTexture()
{
	ID3D11SamplerState* pSampler = pTexture_->GetSampler();
	Direct3D::pContext_->PSSetSamplers(0, 1, &pSampler);

	ID3D11ShaderResourceView* pSRV = pTexture_->GetSRV();
	Direct3D::pContext_->PSSetShaderResources(0, 1, &pSRV);

	Direct3D::pContext_->Unmap(pConstantBuffer_, 0);	//再開
}

void Quad::SetMap(Transform transform)
{
	CONSTANT_BUFFER cb;
	cb.matWVP = XMMatrixTranspose(transform.GetWorldMatrix() * Camera::GetViewMatrix() * Camera::GetProjectionMatrix());
	cb.matNormal = XMMatrixTranspose(transform.GetNormalMatrix());

	D3D11_MAPPED_SUBRESOURCE pdata;
	Direct3D::pContext_->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);	// GPUからのデータアクセスを止める
	memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&cb), sizeof(cb));	// データを値を送る

}

void Quad::SetPipeline()
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

	


}
