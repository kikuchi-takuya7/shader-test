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
		//�G���[����
		MessageBox(nullptr, "���_�o�b�t�@�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return hr_;
	}


	MakeIndBf();
	if (FAILED(hr_)) {
		MessageBox(nullptr, "�C���f�b�N�X�o�b�t�@�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return hr_;
	}

	MakeConBf();
	if (FAILED(hr_)) {
		MessageBox(nullptr, "�R���X�^���g�o�b�t�@�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return hr_;
	}

	pTexture_ = new Texture;
	pTexture_->Load("Assets\\Dice.png");


	return S_OK;
}

void Quad::Draw(Transform& transform)
{

	////�R���X�^���g�o�b�t�@�ɓn�����
	//XMVECTOR position = { 0, 3, -10, 0 };	//�J�����̈ʒu
	//XMVECTOR target = { 0, 0, 0, 0 };	//�J�����̏œ_

	//�r���[�s�� �J�����̐��E�̊e���_�����߂�̂��r���[�s��BXMVectorSet�ŃJ�����̏㉺���ǂ��������߂�
	//XMMATRIX view = XMMatrixLookAtLH(position, target, XMVectorSet(0, 1, 0, 0));	

	//�ˉe�s��@���ߊ����g�p����̂Ɏg���F���ԑ厖�@��]���Ă��炩����̂������Ă����]����̂���
	//XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, 800.0f / 600.0f, 0.1f, 100.0f);//�΁�4�Ƃ����Ӗ��ŁA����p45�x���Ă��ƁBPI(��) DIV(��) 4�i�΃��W�A����180�x)
	// 800.0f / 600.0f�̓A�X�y�N�g��@�@
	//0.1f, 100.0f�j�A(��)�N���b�s���O��.�t�@�[(��)�N���b�s���O��:���E�͕��炾����`�拗�������߂�.��O���c���Ȃ��ƃX�y�[�X�Ȃ��Čv�Z�ł��Ȃ�.�Q�[���ɂ�邪���͂Ȃ�ׂ��������Ȃ���Zfighting���N����

	Direct3D::SetShader(SHADER_3D);

	transform.Calclation();//�g�����X�t�H�[�����v�Z
	SetMap(transform);

	SetTexture();
	
	SetPipeline();

	Direct3D::pContext_->DrawIndexed(indexNum_, 0, 0); //�C���f�b�N�X���̐��͉����������Ă邩

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

	// ���_�f�[�^�p�o�b�t�@�̐ݒ�
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
	// ���_���
	vertices_ =
	{
		{XMVectorSet(-1.0f,  1.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f) /*, XMVectorSet(0.0f,0.0f,-1.0f, 0.0f)*/},	// �l�p�`�̒��_�i����j
		{XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f),   XMVectorSet(0.25f, 0.0f, 0.0f, 0.0f)/*, XMVectorSet(0.0f,0.0f,-1.0f, 0.0f)*/},	// �l�p�`�̒��_�i�E��j
		{XMVectorSet(1.0f, -1.0f, 0.0f, 0.0f),  XMVectorSet(0.25f, 0.5f, 0.0f, 0.0f)/*, XMVectorSet(0.0f,0.0f,-1.0f, 0.0f)*/},	// �l�p�`�̒��_�i�E���j
		{XMVectorSet(-1.0f, -1.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 0.5f, 0.0f, 0.0f) /*, XMVectorSet(0.0f,0.0f,-1.0f, 0.0f)*/},	// �l�p�`�̒��_�i�����j		
		//{XMVectorSet(0.0f, 2.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) },		// �ǉ������_
	};

	vertexNum_ = vertices_.size();
}

void Quad::SetIndex()
{
	//�C���f�b�N�X���
	index_ = { 0,2,3, 0,1,2 };
	indexNum_ = index_.size();
}

void Quad::MakeIndBf()
{
	SetIndex();

	// �C���f�b�N�X�o�b�t�@�𐶐�����
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
	//�R���X�^���g�o�b�t�@�쐬
	D3D11_BUFFER_DESC cb;
	cb.ByteWidth = sizeof(CONSTANT_BUFFER);
	cb.Usage = D3D11_USAGE_DYNAMIC;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;

	// �R���X�^���g�o�b�t�@�̍쐬
	hr_ = Direct3D::pDevice_->CreateBuffer(&cb, nullptr, &pConstantBuffer_);
	
}

void Quad::SetTexture()
{
	ID3D11SamplerState* pSampler = pTexture_->GetSampler();
	Direct3D::pContext_->PSSetSamplers(0, 1, &pSampler);

	ID3D11ShaderResourceView* pSRV = pTexture_->GetSRV();
	Direct3D::pContext_->PSSetShaderResources(0, 1, &pSRV);

	Direct3D::pContext_->Unmap(pConstantBuffer_, 0);	//�ĊJ
}

void Quad::SetMap(Transform transform)
{
	CONSTANT_BUFFER cb;
	cb.matWVP = XMMatrixTranspose(transform.GetWorldMatrix() * Camera::GetViewMatrix() * Camera::GetProjectionMatrix());
	cb.matNormal = XMMatrixTranspose(transform.GetNormalMatrix());

	D3D11_MAPPED_SUBRESOURCE pdata;
	Direct3D::pContext_->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);	// GPU����̃f�[�^�A�N�Z�X���~�߂�
	memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&cb), sizeof(cb));	// �f�[�^��l�𑗂�

}

void Quad::SetPipeline()
{
	//���_�o�b�t�@
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	Direct3D::pContext_->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�[���Z�b�g
	stride = sizeof(int);
	offset = 0;
	Direct3D::pContext_->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R32_UINT, 0);

	//�R���X�^���g�o�b�t�@
	Direct3D::pContext_->VSSetConstantBuffers(0, 1, &pConstantBuffer_);	//���_�V�F�[�_�[�p	
	Direct3D::pContext_->PSSetConstantBuffers(0, 1, &pConstantBuffer_);	//�s�N�Z���V�F�[�_�[�p

	


}
