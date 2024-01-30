#pragma once
#include"Direct3D.h"
#include"Texture.h"
#include<DirectXMath.h>
//#include<vector>
#include<array>
#include"Quad.h"
#include "Transform.h"

using namespace DirectX;
using std::array;

const int VERTEXNUM = 100;



#define SAFE_RELEASE(p) if(p != nullptr){ p->Release(); p = nullptr;}
#define SAFE_DELETE(p) if(p != nullptr){ delete p; p = nullptr;}

class Sprite
{
	struct CONSTANT_BUFFER
	{
		XMMATRIX	matNormal;
	};

	//���_���
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
	ID3D11Buffer* pVertexBuffer_;	//���_�o�b�t�@

	UINT64 indexNum_;
	std::vector<int> index_;

	ID3D11Buffer* pIndexBuffer_;	//�C���f�b�N�X�o�b�t�@
	ID3D11Buffer* pConstantBuffer_;	//�R���X�^���g�o�b�t�@

	Texture* pTexture_;

public:
	Sprite();
	~Sprite();

	HRESULT Initialize();
	HRESULT Load(string fileName);
	void Draw(Transform& transform);
	void Draw(Transform& transform, RECT rect, float alpha);
	void Release();

private:
	//�C�j�V�����C�Y����Ă΂��֐�
	virtual void InitVertexData();
	HRESULT CreateVertexBuffer();

	virtual void InitIndexData();
	HRESULT CreateIndexBuffer();

	HRESULT CreateConstantBuffer();
	HRESULT LoadTexture(string fileName);

	//�h���[�֐�����Ă΂��֐�
	void PassDataToCB(DirectX::XMMATRIX worldMatrix); //�R���X�g���N�g�o�b�t�@
	void SetBufferToPipeline();
};