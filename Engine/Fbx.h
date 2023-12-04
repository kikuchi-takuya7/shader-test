#pragma once

#include <d3d11.h>
#include <fbxsdk.h>
#include <string>
#include "Transform.h"
#include "Direct3D.h"
#include "Camera.h"
#include "Texture.h"

#pragma comment(lib, "LibFbxSDK-MD.lib")
#pragma comment(lib, "LibXml2-MD.lib")
#pragma comment(lib, "zlib-MD.lib")

//class Textue; //前方宣言と言い、ヘッダーにヘッダーをインクルードして無限ループになるときに、ヘッダーにもインクルードしたい時に使うと無限ループしなくなりＣＰＰにヘッダーかける

class Fbx
{

	//マテリアル
	struct MATERIAL
	{
		Texture* pTexture;
		XMFLOAT4	diffuse;
		
	};

	struct CONSTANT_BUFFER
	{
		XMMATRIX	matWVP;//ワールドビュープロジェクション
		XMMATRIX	matW;//これを最近追加した（12/4）　//ワールド変換のみ
		XMMATRIX	matNormal;//スケール×平行移動の逆行列
		XMFLOAT4	diffuseColor;		// ディフューズカラー（fbxから持ってきた面の色:マテリアル）
		XMFLOAT4	lightDirection;
		XMFLOAT4	eyePos;
		BOOL		isTexture;		// テクスチャ貼ってあるかどうか
		
	};

	struct VERTEX
	{
		XMVECTOR position;
		XMVECTOR uv;
		XMVECTOR normal;
	};

	int vertexCount_;	//頂点数
	int polygonCount_;	//ポリゴン数
	int materialCount_;	//マテリアルの個数

	ID3D11Buffer* pVertexBuffer_;
	ID3D11Buffer** pIndexBuffer_;
	ID3D11Buffer* pConstantBuffer_;
	MATERIAL* pMaterialList_;
	std::vector<int> indexCount_;
	//Texture* pTexture_;

	XMFLOAT4 lightSourcePosition_;//ライトの位置をhlslに伝える変数

public:

	Fbx();
	~Fbx();
	HRESULT Load(std::string fileName);
	void InitVertex(fbxsdk::FbxMesh* mesh);
	void InitIndex(fbxsdk::FbxMesh* mesh);
	void InitConstantBuffer();
	void InitMaterial(fbxsdk::FbxNode* pNode);
	void Draw(Transform& transform);
	void SetMap(Transform& transform);
	void SetTexture();
	void SetPipeline();
	void Release();
	void SetLightSource(XMFLOAT4 pos) { lightSourcePosition_ = pos; }
};