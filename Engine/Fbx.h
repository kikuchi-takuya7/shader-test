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
		XMFLOAT4	ambient;
		XMFLOAT4	specular;
		FLOAT		shininess;
		
	};

	struct CONSTANT_BUFFER
	{
		XMMATRIX	matWVP;//���[���h�r���[�v���W�F�N�V����
		XMMATRIX	matW;//������ŋߒǉ������i12/4�j�@//���[���h�ϊ��̂�
		XMMATRIX	matNormal;//�X�P�[���~���s�ړ��̋t�s��
		XMFLOAT4	diffuseColor;		// �f�B�t���[�Y�J���[�ifbx���玝���Ă����ʂ̐F:�}�e���A���j
		XMFLOAT4	ambientColor;
		XMFLOAT4	specularColor;
		FLOAT		shininess;		//�n�C���C�g�̋����iMaya��CosinePower�j
		BOOL		isTexture;		// �e�N�X�`���\���Ă��邩�ǂ���
		
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

	XMFLOAT4 lightSourcePosition_;//���C�g�̈ʒu��hlsl�ɓ`����ϐ�

public:

	Fbx();
	~Fbx();
	HRESULT Load(std::string fileName);
	void InitVertex(fbxsdk::FbxMesh* mesh);
	void InitIndex(fbxsdk::FbxMesh* mesh);
	void InitConstantBuffer();
	void InitMaterial(fbxsdk::FbxNode* pNode);
	void InitTexture(fbxsdk::FbxSurfaceMaterial* pMaterial, const DWORD& i);
	void Draw(Transform& transform);
	void SetMap(Transform& transform);
	void SetTexture();
	void SetPipeline();
	void Release();
	void SetLightSource(XMFLOAT4 pos) { lightSourcePosition_ = pos; }
};