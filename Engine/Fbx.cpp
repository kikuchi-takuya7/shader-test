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
	//�}�l�[�W���𐶐�
	FbxManager* pFbxManager = FbxManager::Create();

	//�C���|�[�^�[�𐶐�
	FbxImporter* fbxImporter = FbxImporter::Create(pFbxManager, "imp"); //�V�[�������[�h���邽�߂̂��
	fbxImporter->Initialize(fileName.c_str(), -1, pFbxManager->GetIOSettings());

	//�V�[���I�u�W�F�N�g��FBX�t�@�C���̏��𗬂�����
	FbxScene* pFbxScene = FbxScene::Create(pFbxManager, "fbxscene"); //�V�[�����ۂ��Ɠǂݍ��ވׂ̂��
	fbxImporter->Import(pFbxScene); //�C���|�[�g
	fbxImporter->Destroy();

	//���b�V�������擾
	FbxNode* rootNode = pFbxScene->GetRootNode(); //���̃m�[�h��ۑ��i�m�[�h�Ƃ�fbx�I�u�W�F�N�g�̎q���̂悤�Ȃ��́j
	FbxNode* pNode = rootNode->GetChild(0); //���q�i��ԍŏ��̎q���j���擾�B�S�Č�����I������������q�������Ȃ����߂��ꂪ�厖
	FbxMesh* mesh = pNode->GetMesh(); // ���b�V���̒��ɒ��_���Ƃ������Ă��肷��

	//�e���̌����擾

	vertexCount_ = mesh->GetControlPointsCount();	//���_�̐�
	polygonCount_ = mesh->GetPolygonCount();	//�|���S���̐�
	materialCount_ = pNode->GetMaterialCount(); //�}�e���A���̐��@��̃m�[�h�i�q���j�̒���������Ƃ��Ă���

	//���݂̃J�����g�f�B���N�g�����o���Ă���
	char defaultCurrentDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, defaultCurrentDir);

	//������fileName����f�B���N�g���������擾
	char dir[MAX_PATH];
	_splitpath_s(fileName.c_str(), nullptr, 0, dir, _MAX_DIR, nullptr, 0, nullptr, 0);
	//wsprintf(dir, "%s", dir);

	//�J�����g�f�B���N�g���ύX
	SetCurrentDirectory(dir);

	InitVertex(mesh);		//���_�o�b�t�@��
	InitIndex(mesh);
	InitConstantBuffer();	//�R���X�^���g�o�b�t�@����
	InitMaterial(pNode); //�}�e���A������

	//�I�������߂�
	SetCurrentDirectory(defaultCurrentDir);

	//�}�l�[�W�����
	pFbxManager->Destroy();
	return S_OK;
}

//���_�o�b�t�@����
void Fbx::InitVertex(fbxsdk::FbxMesh* mesh)
{
	//���_��������z��
	VERTEX* vertices = new VERTEX[vertexCount_];

	//�S�|���S��
	for (DWORD poly = 0; poly < polygonCount_; poly++)
	{
		//3���_��
		for (int vertex = 0; vertex < 3; vertex++)
		{
			//���ׂ钸�_�̔ԍ�
			int index = mesh->GetPolygonVertex(poly, vertex);

			//���_�̈ʒu
			FbxVector4 pos = mesh->GetControlPointAt(index);
			vertices[index].position = XMVectorSet((float)pos[0], (float)pos[1], (float)pos[2], 0.0f);

			//���_��UV
			FbxLayerElementUV* pUV = mesh->GetLayer(0)->GetUVs();
			int uvIndex = mesh->GetTextureUVIndex(poly, vertex, FbxLayerElement::eTextureDiffuse);
			FbxVector2  uv = pUV->GetDirectArray().GetAt(uvIndex);
			vertices[index].uv = XMVectorSet((float)uv.mData[0], (float)(1.0f - uv.mData[1]), 0.0f, 0.0f); //v���W�͏㉺�Ŕ��]�����Ȃ���΂����Ȃ�

			//���_�̖@��
			FbxVector4 Normal;
			mesh->GetPolygonVertexNormal(poly, vertex, Normal);	//���Ԗڂ̃|���S���́A���Ԗڂ̒��_�̖@�����Q�b�g
			vertices[index].normal = XMVectorSet((float)Normal[0], (float)Normal[1], (float)Normal[2], 0.0f);
		}
	}

	//���_�o�b�t�@�쐬
	// ���_�f�[�^�p�o�b�t�@�̐ݒ�
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
		MessageBox(nullptr, "���_�o�b�t�@�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);

		//return E_FAIL;
	}

	delete[] vertices;
}


//�C���f�b�N�X�o�b�t�@����
void Fbx::InitIndex(fbxsdk::FbxMesh* mesh)
{
	pIndexBuffer_ = new ID3D11Buffer*[materialCount_];
	//int* index = new int[polygonCount_ * 3];
	std::vector<int> index(polygonCount_ * 3);

	indexCount_ = std::vector<int>(materialCount_);

	for (int i = 0; i < materialCount_; i++) { //�����Ń}�e���A�����Ƀ��[�v�B�ԃ}�e���A���̒��_��񂪓������z��Ɛ}�e�p�݂�����
		
		int count = 0;

		//�S�|���S��
		for (DWORD poly = 0; poly < polygonCount_; poly++) //���_������Âm�F���Ă���
		{
			FbxLayerElementMaterial* mtl = mesh->GetLayer(0)->GetMaterials();
			int mtlId = mtl->GetIndexArray().GetAt(poly); //poly�Ԗڂ̃}�e���A���̔ԍ����擾

			if (mtlId == i) //���ꂪi�Ԗڂ̃}�e���A���ԍ��Ȃ�
			{
				//3���_��
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
			MessageBox(nullptr, "�C���f�b�N�X�o�b�t�@�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);

			//return E_FAIL;
		}
	
	}

	//delete[] index;
}

void Fbx::InitConstantBuffer()
{
	//�R���X�^���g�o�b�t�@�쐬
	D3D11_BUFFER_DESC cb;
	cb.ByteWidth = sizeof(CONSTANT_BUFFER);
	cb.Usage = D3D11_USAGE_DYNAMIC;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;

	// �R���X�^���g�o�b�t�@�̍쐬
	HRESULT hr;
	hr = Direct3D::pDevice_->CreateBuffer(&cb, nullptr, &pConstantBuffer_);
	if (FAILED(hr)) {
		MessageBox(nullptr, "�R���X�g���N�g�o�b�t�@�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);

		//return E_FAIL;
	}
}

void Fbx::InitMaterial(fbxsdk::FbxNode* pNode)
{
	pMaterialList_ = new MATERIAL[materialCount_];

	for (int i = 0; i < materialCount_; i++)
	{
		//i�Ԗڂ̃}�e���A�������擾
		FbxSurfaceMaterial* pMaterial = pNode->GetMaterial(i);

		//�e�N�X�`�����
		FbxProperty  lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse); //maya�Ńe�N�X�`���\��������{�^��������H���̏��炵����

		//�e�N�X�`���̐���
		int fileTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>(); //�e�N�X�`���\���Ă���΂P�ȏ�o�Ȃ���΂O

		//�e�N�X�`������
		if (fileTextureCount)
		{

			FbxFileTexture* textureInfo = lProperty.GetSrcObject<FbxFileTexture>(0);
			const char* textureFilePath = textureInfo->GetRelativeFileName();

			//�t�@�C����+�g�������ɂ���
			char name[_MAX_FNAME];	//�t�@�C����
			char ext[_MAX_EXT];	//�g���q
			_splitpath_s(textureFilePath, nullptr, 0, nullptr, 0, name, _MAX_FNAME, ext, _MAX_EXT); //����h���C�u�ƃf�B���N�g�����͂���Ȃ��̂ŁA�t�@�C���̖��O�Ɗg���q����
			wsprintf(name, "%s%s", name, ext);

			//�t�@�C������e�N�X�`���쐬
			pMaterialList_[i].pTexture = new Texture;
			HRESULT hr = pMaterialList_[i].pTexture->Load(name);
			assert(hr == S_OK);

		}

		//�e�N�X�`������
		else
		{
			pMaterialList_[i].pTexture = nullptr;

			//�}�e���A���̐F
			FbxSurfaceLambert* pMaterial = (FbxSurfaceLambert*)pNode->GetMaterial(i); //�e�N�X�`�����Ȃ��������F���f�B�q���[�Y�ɓ����
			FbxDouble3  diffuse = pMaterial->Diffuse;
			pMaterialList_[i].diffuse = XMFLOAT4((float)diffuse[0], (float)diffuse[1], (float)diffuse[2], 1.0f); //�����ŃV���v���RD�p�̃f�B�q���[�Y������
		}
	}
}

void Fbx::Draw(Transform& transform)
{

	Direct3D::SetShader(SHADER_TYPE::SHADER_3D);
	transform.Calclation();//�g�����X�t�H�[�����v�Z
	

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
		//	cb.diffuseColor = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f); //���艟���Ń}�e���A���̐F��ς��邱�Ƃ��\
		//	cb.isTexture = pMaterialList_[i].pTexture != nullptr;
		//}
		

		D3D11_MAPPED_SUBRESOURCE pdata;
		Direct3D::pContext_->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);	// GPU����̃f�[�^�A�N�Z�X���~�߂�
		memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&cb), sizeof(cb));	// �f�[�^��l�𑗂�

		Direct3D::pContext_->Unmap(pConstantBuffer_, 0);	//�ĊJ

		//���_�o�b�t�@
		UINT stride = sizeof(VERTEX);
		UINT offset = 0;
		Direct3D::pContext_->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

		// �C���f�b�N�X�o�b�t�@�[���Z�b�g
		stride = sizeof(int);
		offset = 0;
		Direct3D::pContext_->IASetIndexBuffer(pIndexBuffer_[i], DXGI_FORMAT_R32_UINT, 0);

		Direct3D::pContext_->VSSetConstantBuffers(0, 1, &pConstantBuffer_); //���_�V�F�[�_�[�p
		Direct3D::pContext_->PSSetConstantBuffers(0, 1, &pConstantBuffer_); //�s�N�Z���V�F�[�_�[�p

		if (pMaterialList_[i].pTexture)
		{
			ID3D11SamplerState* pSampler = pMaterialList_[i].pTexture->GetSampler();
			Direct3D::pContext_->PSSetSamplers(0, 1, &pSampler);
			ID3D11ShaderResourceView* pSRV = pMaterialList_[i].pTexture->GetSRV();
			Direct3D::pContext_->PSSetShaderResources(0, 1, &pSRV);
		}

		Direct3D::pContext_->DrawIndexed(indexCount_[i], 0, 0); //�C���f�b�N�X���̐��͉����������Ă邩

	}
	//SetTexture();

	//Direct3D::pContext_->Unmap(pConstantBuffer_, 0);	//�ĊJ

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
	Direct3D::pContext_->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);	// GPU����̃f�[�^�A�N�Z�X���~�߂�
	memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&cb), sizeof(cb));	// �f�[�^��l�𑗂�

	
}

void Fbx::SetTexture()
{
	//ID3D11SamplerState* pSampler = pMaterialList_[i].pTexture->GetSampler();
	//Direct3D::pContext_->PSSetSamplers(0, 1, &pSampler);

	//ID3D11ShaderResourceView* pSRV = pMaterialList_[i].pTexture->GetSRV();
	//Direct3D::pContext_->PSSetShaderResources(0, 1, &pSRV);

	//Direct3D::pContext_->Unmap(pConstantBuffer_, 0);	//�ĊJ
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


		Direct3D::pContext_->Unmap(pConstantBuffer_, 0);	//�ĊJ

		//���_�o�b�t�@
		UINT stride = sizeof(VERTEX);
		UINT offset = 0;
		Direct3D::pContext_->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

		// �C���f�b�N�X�o�b�t�@�[���Z�b�g
		stride = sizeof(int);
		offset = 0;
		Direct3D::pContext_->IASetIndexBuffer(pIndexBuffer_[i], DXGI_FORMAT_R32_UINT, 0);

		Direct3D::pContext_->VSSetConstantBuffers(0, 1, &pConstantBuffer_); //���_�V�F�[�_�[�p
		Direct3D::pContext_->PSSetConstantBuffers(0, 1, &pConstantBuffer_); //�s�N�Z���V�F�[�_�[�p

		Direct3D::pContext_->DrawIndexed(indexCount_[i], 0, 0); //�C���f�b�N�X���̐��͉����������Ă邩

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
