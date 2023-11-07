#pragma once
#include "Direct3D.h"
#include "Camera.h"
#include <DirectXMath.h>
#include "Texture.h"
#include <array>
#include "Transform.h"

using namespace DirectX;
using std::vector;

const UINT64 INDEXNUM = 36;

//�R���X�^���g�o�b�t�@�[

struct CONSTANT_BUFFER
{
	XMMATRIX	matWVP;
	XMMATRIX	matNormal;
};

//���_���
struct VERTEX
{
	XMVECTOR position;
	XMVECTOR uv;
	XMVECTOR normal;
};

class Quad
{
protected:
	ID3D11Buffer* pVertexBuffer_;	//���_�o�b�t�@
	ID3D11Buffer* pIndexBuffer_;	//�C���f�b�N�X�o�b�t�@
	ID3D11Buffer* pConstantBuffer_;	//�R���X�^���g�o�b�t�@
	Texture* pTexture_;
	HRESULT hr_;
	int indexNum_;//�C���f�b�N�X��
	vector<int> index_;
	int vertexNum_;
	vector<VERTEX> vertices_;

public:
	Quad();
	~Quad();
	HRESULT Initialize();
	void Draw(Transform& transform);
	void Release();
	void MakeVerBf();
	virtual void SetVartices();
	virtual void SetIndex();
	void MakeIndBf();
	void MakeConBf();
	void SetTexture();
	void SetMap(Transform transform);
	void SetPipeline();
};