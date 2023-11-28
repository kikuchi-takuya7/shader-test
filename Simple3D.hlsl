//������������������������������������������������������������������������������
 // �e�N�X�`�����T���v���[�f�[�^�̃O���[�o���ϐ���`
//������������������������������������������������������������������������������
Texture2D		g_texture : register(t0);	//�e�N�X�`���[
SamplerState	g_sampler : register(s0);	//�T���v���[

//������������������������������������������������������������������������������
// �R���X�^���g�o�b�t�@
// DirectX �����瑗�M����Ă���A�|���S�����_�ȊO�̏����̒�`
//������������������������������������������������������������������������������
cbuffer global
{
	float4x4	matWVP;			// ���[���h�E�r���[�E�v���W�F�N�V�����̍����s��
	float4x4	matNormal;           // ���[���h�s��
	float4		diffuseColor;		// �f�B�t���[�Y�J���[�i�}�e���A���̐F�j
	float4		lightDirection;
	float4		eyePos;
	bool		isTexture;		// �e�N�X�`���\���Ă��邩�ǂ���
};

//������������������������������������������������������������������������������
// ���_�V�F�[�_�[�o�́��s�N�Z���V�F�[�_�[���̓f�[�^�\����
//������������������������������������������������������������������������������
struct VS_OUT
{
	float4 pos  : SV_POSITION;	//�ʒu
	float2 uv	: TEXCOORD;		//UV���W
	float4 color	: COLOR;	//�F�i���邳�j
};

//������������������������������������������������������������������������������
// ���_�V�F�[�_
//������������������������������������������������������������������������������
VS_OUT VS(float4 pos : POSITION, float4 uv : TEXCOORD, float4 normal : NORMAL)
{
	//�s�N�Z���V�F�[�_�[�֓n�����
	VS_OUT outData;

	//���[�J�����W�ɁA���[���h�E�r���[�E�v���W�F�N�V�����s���������
	//�X�N���[�����W�ɕϊ����A�s�N�Z���V�F�[�_�[��
	outData.pos = mul(pos, matWVP);
	outData.uv = uv;

	//�@������]
	normal = mul(normal, matNormal);

	//float4 light = float4( 1.0, 0.8, -1.5, 0);    //�����̌����i���̍��W���������"����"�j ����^�C�v������΋t�̃^�C�v������
	float4 light = float4(-1, 0, 0, 0);
	light = normalize(light);
	outData.color = clamp(dot(normal, light), 0, 1);

	//�܂Ƃ߂ďo��
	return outData;
}

//������������������������������������������������������������������������������
// �s�N�Z���V�F�[�_
//������������������������������������������������������������������������������
float4 PS(VS_OUT inData) : SV_Target
{

	//float4 lightSource = float4(1.0, 1.0, 1.0, 1.0);//�F��4���F
	//float4 ambentSource = float4(0.2, 0.2, 0.2, 1.0);//���邭����΃c���c������ ����float4����Y���ƒl���S��1�ɂȂ��Ĉُ�Ȓ����邭�Ȃ邩��C�������I�I�I
	//float4 diffuse;
	//float4 ambient;

	//if (isTexture) {
	//	diffuse = lightSource * g_texture.Sample(g_sampler, inData.uv) * inData.color;//�g�U���ːF
	//	ambient = lightSource * g_texture.Sample(g_sampler, inData.uv) * ambentSource;//�����ːF
	//}
	//else {
	//	diffuse = lightSource * diffuseColor * inData.color;//�g�U���ːF
	//	ambient = lightSource * diffuseColor * ambentSource;//�����ːF
	//}
	//return (diffuse + ambient);//���ۂ̐F

	// Postarization
	float4 output = floor(g_texture.Sample(g_sampler,inData.uv) * 8.0) / 8;
	return output;
}

//�F�����炿�炵�Ă����ȐF�ɕς��p�`���R�d�l�ɂȂ�Ƃ���Direct3D�̒��̃N���G�C�g�V�F�[�_�[�̒���Simple3D��2D�̕\�L���Ԉ���Ă�\����I�I