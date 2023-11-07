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

//class Textue; //�O���錾�ƌ����A�w�b�_�[�Ƀw�b�_�[���C���N���[�h���Ė������[�v�ɂȂ�Ƃ��ɁA�w�b�_�[�ɂ��C���N���[�h���������Ɏg���Ɩ������[�v���Ȃ��Ȃ�b�o�o�Ƀw�b�_�[������

class Fbx
{

	//�}�e���A��
	struct MATERIAL
	{
		Texture* pTexture;
		XMFLOAT4	diffuse;
		
	};

	struct CONSTANT_BUFFER
	{
		XMMATRIX	matWVP;
		XMMATRIX	matNormal;
		XMFLOAT4	diffuseColor;		// �f�B�t���[�Y�J���[�i�}�e���A���̐F�j
		int		isTexture;		// �e�N�X�`���\���Ă��邩�ǂ���
	};

	struct VERTEX
	{
		XMVECTOR position;
		XMVECTOR uv;
		XMVECTOR normal;
	};

	int vertexCount_;	//���_��
	int polygonCount_;	//�|���S����
	int materialCount_;	//�}�e���A���̌�

	ID3D11Buffer* pVertexBuffer_;
	ID3D11Buffer** pIndexBuffer_;
	ID3D11Buffer* pConstantBuffer_;
	MATERIAL* pMaterialList_;
	std::vector<int> indexCount_;
	//Texture* pTexture_;

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
};