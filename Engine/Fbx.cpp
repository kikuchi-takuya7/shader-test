#include "Fbx.h"
//#include "Texture.h"


Fbx::Fbx(): pVertexBuffer_(nullptr), pIndexBuffer_(nullptr), pConstantBuffer_(nullptr), pMaterialList_(nullptr), vertexCount_(0),polygonCount_(0),materialCount_(0)
{
}

Fbx::~Fbx()
{
	//SAFE_RELEASE(pTexture_);
	//SAFE_DELETE(pTexture_);

	/*SAFE_RELEASE(pConstantBuffer_);
	SAFE_DE(pIndexBuffer_);
	SAFE_RELEASE(pVertexBuffer_);*/

	Release();
}



HRESULT Fbx::Load(std::string fileName)
{
	//マネージャを生成
	FbxManager* pFbxManager = FbxManager::Create();

	//インポーターを生成
	FbxImporter* fbxImporter = FbxImporter::Create(pFbxManager, "imp"); //シーンをロードするためのやつ
	fbxImporter->Initialize(fileName.c_str(), -1, pFbxManager->GetIOSettings());

	//シーンオブジェクトにFBXファイルの情報を流し込む
	FbxScene* pFbxScene = FbxScene::Create(pFbxManager, "fbxscene"); //シーンを丸っと読み込む為のやつ
	fbxImporter->Import(pFbxScene); //インポート
	fbxImporter->Destroy();

	//メッシュ情報を取得
	FbxNode* rootNode = pFbxScene->GetRootNode(); //一つ一つのノードを保存（ノードとはfbxオブジェクトの子供のようなもの）
	FbxNode* pNode = rootNode->GetChild(0); //第一子（一番最初の子供）を取得。全て結合を選択したから第一子しかいないためこれが大事
	FbxMesh* mesh = pNode->GetMesh(); // メッシュの中に頂点情報とか入ってたりする

	//各情報の個数を取得

	vertexCount_ = mesh->GetControlPointsCount();	//頂点の数
	polygonCount_ = mesh->GetPolygonCount();	//ポリゴンの数
	materialCount_ = pNode->GetMaterialCount(); //マテリアルの数　一つのノード（子供）の中から情報をとってくる

	//現在のカレントディレクトリを覚えておく
	char defaultCurrentDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, defaultCurrentDir);

	//引数のfileNameからディレクトリ部分を取得
	char dir[MAX_PATH];
	_splitpath_s(fileName.c_str(), nullptr, 0, dir, _MAX_DIR, nullptr, 0, nullptr, 0);
	//wsprintf(dir, "%s", dir);

	//カレントディレクトリ変更
	SetCurrentDirectory(dir);

	InitVertex(mesh);		//頂点バッファ準
	InitIndex(mesh);
	InitConstantBuffer();	//コンスタントバッファ準備
	InitMaterial(pNode); //マテリアル準備

	//終わったら戻す
	SetCurrentDirectory(defaultCurrentDir);

	//マネージャ解放
	pFbxManager->Destroy();
	return S_OK;
}

//頂点バッファ準備
void Fbx::InitVertex(fbxsdk::FbxMesh* mesh)
{
	//頂点情報を入れる配列
	VERTEX* vertices = new VERTEX[vertexCount_];

	//全ポリゴン
	for (DWORD poly = 0; poly < polygonCount_; poly++)
	{
		//3頂点分
		for (int vertex = 0; vertex < 3; vertex++)
		{
			//調べる頂点の番号
			int index = mesh->GetPolygonVertex(poly, vertex);

			//頂点の位置
			FbxVector4 pos = mesh->GetControlPointAt(index);
			vertices[index].position = XMVectorSet((float)pos[0], (float)pos[1], (float)pos[2], 0.0f);

			//頂点のUV
			FbxLayerElementUV* pUV = mesh->GetLayer(0)->GetUVs();
			int uvIndex = mesh->GetTextureUVIndex(poly, vertex, FbxLayerElement::eTextureDiffuse);
			FbxVector2  uv = pUV->GetDirectArray().GetAt(uvIndex);
			vertices[index].uv = XMVectorSet((float)uv.mData[0], (float)(1.0f - uv.mData[1]), 0.0f, 0.0f); //v座標は上下で反転させなければいけない

			//頂点の法線
			FbxVector4 Normal;
			mesh->GetPolygonVertexNormal(poly, vertex, Normal);	//ｉ番目のポリゴンの、ｊ番目の頂点の法線をゲット
			vertices[index].normal = XMVectorSet((float)Normal[0], (float)Normal[1], (float)Normal[2], 0.0f);
		}
	}

	//頂点バッファ作成
	// 頂点データ用バッファの設定
	D3D11_BUFFER_DESC bd_vertex;
	bd_vertex.ByteWidth = sizeof(VERTEX) * vertexCount_;
	bd_vertex.Usage = D3D11_USAGE_DEFAULT;
	bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd_vertex.CPUAccessFlags = 0;
	bd_vertex.MiscFlags = 0;
	bd_vertex.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA data_vertex;
	data_vertex.pSysMem = vertices;
	HRESULT hr = Direct3D::pDevice_->CreateBuffer(&bd_vertex, &data_vertex, &pVertexBuffer_);
	if (FAILED(hr)) {
		MessageBox(nullptr, "頂点バッファの作成に失敗しました", "エラー", MB_OK);

		//return E_FAIL;
	}

	delete[] vertices;
}


//インデックスバッファ準備
void Fbx::InitIndex(fbxsdk::FbxMesh* mesh)
{
	pIndexBuffer_ = new ID3D11Buffer*[materialCount_];
	//int* index = new int[polygonCount_ * 3];
	std::vector<int> index(polygonCount_ * 3);

	indexCount_ = std::vector<int>(materialCount_);

	for (int i = 0; i < materialCount_; i++) { //ここでマテリアル事にループ。赤マテリアルの頂点情報が入った配列と青マテ用みたいな
		
		int count = 0;

		//全ポリゴン
		for (DWORD poly = 0; poly < polygonCount_; poly++) //頂点情報を一つづつ確認していく
		{
			FbxLayerElementMaterial* mtl = mesh->GetLayer(0)->GetMaterials();
			int mtlId = mtl->GetIndexArray().GetAt(poly); //poly番目のマテリアルの番号を取得

			if (mtlId == i) //それがi番目のマテリアル番号なら
			{
				//3頂点分
				for (DWORD vertex = 0; vertex < 3; vertex++)
				{
					index[count] = mesh->GetPolygonVertex(poly, vertex);
					count++;
				}
			}
		}
		indexCount_[i] = count;

		D3D11_BUFFER_DESC   bd;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(int) * polygonCount_ * 3;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = index.data();
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;
		HRESULT hr;
		hr = Direct3D::pDevice_->CreateBuffer(&bd, &InitData, &pIndexBuffer_[i]);
		if (FAILED(hr)) {
			MessageBox(nullptr, "インデックスバッファの作成に失敗しました", "エラー", MB_OK);

			//return E_FAIL;
		}
	
	}

	//delete[] index;
}

void Fbx::InitConstantBuffer()
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
	HRESULT hr;
	hr = Direct3D::pDevice_->CreateBuffer(&cb, nullptr, &pConstantBuffer_);
	if (FAILED(hr)) {
		MessageBox(nullptr, "コンストラクトバッファの作成に失敗しました", "エラー", MB_OK);

		//return E_FAIL;
	}
}

void Fbx::InitMaterial(fbxsdk::FbxNode* pNode)
{
	pMaterialList_ = new MATERIAL[materialCount_];

	for (int i = 0; i < materialCount_; i++)
	{
		//i番目のマテリアル情報を取得
		FbxSurfaceMaterial* pMaterial = pNode->GetMaterial(i);

		//テクスチャ情報
		FbxProperty  lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse); //mayaでテクスチャ表示させるボタンあるやん？あの情報らしいで

		//テクスチャの数数
		int fileTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>(); //テクスチャ貼ってあれば１以上出なければ０

		//テクスチャあり
		if (fileTextureCount)
		{

			FbxFileTexture* textureInfo = lProperty.GetSrcObject<FbxFileTexture>(0);
			const char* textureFilePath = textureInfo->GetRelativeFileName();

			//ファイル名+拡張だけにする
			char name[_MAX_FNAME];	//ファイル名
			char ext[_MAX_EXT];	//拡張子
			_splitpath_s(textureFilePath, nullptr, 0, nullptr, 0, name, _MAX_FNAME, ext, _MAX_EXT); //今回ドライブとディレクトリ名はいらないので、ファイルの名前と拡張子だけ
			wsprintf(name, "%s%s", name, ext);

			//ファイルからテクスチャ作成
			pMaterialList_[i].pTexture = new Texture;
			HRESULT hr = pMaterialList_[i].pTexture->Load(name);
			assert(hr == S_OK);

		}

		//テクスチャ無し
		else
		{
			pMaterialList_[i].pTexture = nullptr;

			//マテリアルの色
			FbxSurfaceLambert* pMaterial = (FbxSurfaceLambert*)pNode->GetMaterial(i); //テクスチャがない時だけ色をディヒューズに入れる
			FbxDouble3  diffuse = pMaterial->Diffuse;
			pMaterialList_[i].diffuse = XMFLOAT4((float)diffuse[0], (float)diffuse[1], (float)diffuse[2], 1.0f); //ここでシンプル３D用のディヒューズを入れる
		}
	}
}

void Fbx::Draw(Transform& transform)
{

	Direct3D::SetShader(SHADER_TYPE::SHADER_3D);
	transform.Calclation();//トランスフォームを計算
	

	//SetMap(transform);

	for (int i = 0; i < materialCount_; i++) {
	//for (int i = materialCount_ - 1; i >= 0; i--) {



		CONSTANT_BUFFER cb;
		cb.matWVP = XMMatrixTranspose(transform.GetWorldMatrix() * Camera::GetViewMatrix() * Camera::GetProjectionMatrix());
		cb.matNormal = XMMatrixTranspose(transform.GetNormalMatrix());
		cb.matW = XMMatrixTranspose(transform.GetWorldMatrix());
		cb.diffuseColor = pMaterialList_[i].diffuse;
		cb.lightDirection = lightSourcePosition_;
		XMStoreFloat4(&cb.eyePos, Camera::GetEyePosition());
		cb.isTexture = pMaterialList_[i].pTexture != nullptr;

		//if (i == 1) {
		//	cb.diffuseColor = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f); //ごり押しでマテリアルの色を変えることも可能
		//	cb.isTexture = pMaterialList_[i].pTexture != nullptr;
		//}
		

		D3D11_MAPPED_SUBRESOURCE pdata;
		Direct3D::pContext_->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);	// GPUからのデータアクセスを止める
		memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&cb), sizeof(cb));	// データを値を送る

		Direct3D::pContext_->Unmap(pConstantBuffer_, 0);	//再開

		//頂点バッファ
		UINT stride = sizeof(VERTEX);
		UINT offset = 0;
		Direct3D::pContext_->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

		// インデックスバッファーをセット
		stride = sizeof(int);
		offset = 0;
		Direct3D::pContext_->IASetIndexBuffer(pIndexBuffer_[i], DXGI_FORMAT_R32_UINT, 0);

		Direct3D::pContext_->VSSetConstantBuffers(0, 1, &pConstantBuffer_); //頂点シェーダー用
		Direct3D::pContext_->PSSetConstantBuffers(0, 1, &pConstantBuffer_); //ピクセルシェーダー用

		if (pMaterialList_[i].pTexture)
		{
			ID3D11SamplerState* pSampler = pMaterialList_[i].pTexture->GetSampler();
			Direct3D::pContext_->PSSetSamplers(0, 1, &pSampler);
			ID3D11ShaderResourceView* pSRV = pMaterialList_[i].pTexture->GetSRV();
			Direct3D::pContext_->PSSetShaderResources(0, 1, &pSRV);
		}

		Direct3D::pContext_->DrawIndexed(indexCount_[i], 0, 0); //インデックス情報の数は何個数字を入れてるか

	}
	//SetTexture();

	//Direct3D::pContext_->Unmap(pConstantBuffer_, 0);	//再開

	//SetPipeline();
	
}

void Fbx::SetMap(Transform& transform)
{
	CONSTANT_BUFFER cb;
	cb.matWVP = XMMatrixTranspose(transform.GetWorldMatrix() * Camera::GetViewMatrix() * Camera::GetProjectionMatrix());
	cb.matNormal = XMMatrixTranspose(transform.GetNormalMatrix());
	/*cb.diffuseColor = pMaterialList_[i].diffuse;
	cb.isTexture = pMaterialList_[i].pTexture != nullptr;*/


	D3D11_MAPPED_SUBRESOURCE pdata;
	Direct3D::pContext_->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);	// GPUからのデータアクセスを止める
	memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&cb), sizeof(cb));	// データを値を送る

	
}

void Fbx::SetTexture()
{
	//ID3D11SamplerState* pSampler = pMaterialList_[i].pTexture->GetSampler();
	//Direct3D::pContext_->PSSetSamplers(0, 1, &pSampler);

	//ID3D11ShaderResourceView* pSRV = pMaterialList_[i].pTexture->GetSRV();
	//Direct3D::pContext_->PSSetShaderResources(0, 1, &pSRV);

	//Direct3D::pContext_->Unmap(pConstantBuffer_, 0);	//再開
}


void Fbx::SetPipeline()
{

	//for (int i = 0; i < materialCount_; i++) {
	for (int i = materialCount_ - 1; i >= 0; i--) {

		

		if (pMaterialList_[i].pTexture)
		{
			ID3D11SamplerState* pSampler = pMaterialList_[i].pTexture->GetSampler();
			Direct3D::pContext_->PSSetSamplers(0, 1, &pSampler);
			ID3D11ShaderResourceView* pSRV = pMaterialList_[i].pTexture->GetSRV();
			Direct3D::pContext_->PSSetShaderResources(0, 1, &pSRV);
		}


		Direct3D::pContext_->Unmap(pConstantBuffer_, 0);	//再開

		//頂点バッファ
		UINT stride = sizeof(VERTEX);
		UINT offset = 0;
		Direct3D::pContext_->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

		// インデックスバッファーをセット
		stride = sizeof(int);
		offset = 0;
		Direct3D::pContext_->IASetIndexBuffer(pIndexBuffer_[i], DXGI_FORMAT_R32_UINT, 0);

		Direct3D::pContext_->VSSetConstantBuffers(0, 1, &pConstantBuffer_); //頂点シェーダー用
		Direct3D::pContext_->PSSetConstantBuffers(0, 1, &pConstantBuffer_); //ピクセルシェーダー用

		Direct3D::pContext_->DrawIndexed(indexCount_[i], 0, 0); //インデックス情報の数は何個数字を入れてるか

	}

	

}

void Fbx::Release()
{
	//SAFE_RELEASE(pTexture_);
	//SAFE_DELETE(pTexture_);

	SAFE_RELEASE(pConstantBuffer_);
	//SAFE_RELEASE(pIndexBuffer_);
	SAFE_DELETE(pIndexBuffer_);
	SAFE_RELEASE(pVertexBuffer_);
}
