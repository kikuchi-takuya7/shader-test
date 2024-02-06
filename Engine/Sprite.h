#pragma once
#include"Direct3D.h"
#include"Texture.h"
#include<DirectXMath.h>
//#include<vector>
#include<array>
#include"Quad.h"

using namespace DirectX;
using std::array;

const int VERTEXNUM = 100;



#define SAFE_RELEASE(p) if(p != nullptr){ p->Release(); p = nullptr;}
#define SAFE_DELETE(p) if(p != nullptr){ delete p; p = nullptr;}

class Sprite
{
	struct CONSTANT_BUFFER
	{
		//先生ここにいろんな情報あった。ここか
		XMMATRIX	matNormal;
		//XMMATRIX	uvTrans;
		XMFLOAT4	color;
		float scroll;
	};

	//頂点情報
	struct VERTEX
	{
		XMVECTOR position;
		XMVECTOR uv;
	};

protected:

	/*HRESULT hr_;
	array<int, INDEXNUM> index_;*/

	UINT64 vertexNum_;
	std::vector<VERTEX> vertices_;
	//array<VERTEX,VERTEXNUM> vertices_;
	ID3D11Buffer* pVertexBuffer_;	//頂点バッファ

	UINT64 indexNum_;
	std::vector<int> index_;

	ID3D11Buffer* pIndexBuffer_;	//インデックスバッファ
	ID3D11Buffer* pConstantBuffer_;	//コンスタントバッファ

	Texture* pTexture_;

public:
	Sprite();
	~Sprite();

	HRESULT Initialize();
	void Draw(Transform& transform);
	void Release();
	HRESULT LoadTexture();

	float scrollVal_;

private:
	//イニシャライズから呼ばれる関数
	virtual void InitVertexData();
	HRESULT CreateVertexBuffer();

	virtual void InitIndexData();
	HRESULT CreateIndexBuffer();

	HRESULT CreateConstantBuffer();
	

	//ドロー関数から呼ばれる関数
	void PassDataToCB(DirectX::XMMATRIX worldMatrix); //コンストラクトバッファ
	void SetBufferToPipeline();
};