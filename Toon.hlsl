//������������������������������������������������������������������������������
// �e�N�X�`�����T���v���[�f�[�^�̃O���[�o���ϐ���`
//������������������������������������������������������������������������������
Texture2D		g_texture : register(t0);	//�e�N�X�`���[
SamplerState	g_sampler : register(s0);	//�T���v���[

Texture2D		g_toon_texture : register(t1); //���W�X�^�[�ɃZ�b�g�e�N�X�`��

//������������������������������������������������������������������������������
// �R���X�^���g�o�b�t�@
// DirectX �����瑗�M����Ă���A�|���S�����_�ȊO�̏����̒�`
//������������������������������������������������������������������������������
cbuffer global:register(b0)
{
	float4x4	matWVP;			// ���[���h�E�r���[�E�v���W�F�N�V�����̍����s��
	float4x4	matW;			// ���[���h�Ԋҍs��Ƃ�����������
	float4x4	matNormal;           // ���[���h�s��
	float4		diffuseColor;		// �f�B�t���[�Y�J���[�i�}�e���A���̐F�j
	float4		ambientColor;	// ����
	float4		specularColor;
	float	    shininess;
	bool		isTexture;		// �e�N�X�`���\���Ă��邩�ǂ���
};

cbuffer global:register(b1)
{
	float4		lightDirection;
	float4		eyePos;
}

//������������������������������������������������������������������������������
// ���_�V�F�[�_�[�o�́��s�N�Z���V�F�[�_�[���̓f�[�^�\����
//������������������������������������������������������������������������������
struct VS_OUT
{
	float4 pos  : SV_POSITION;	//�ʒu
	float2 uv	: TEXCOORD;		//UV���W
	float4 color	: COLOR;	//�F�i���邳�j
	float4 eyev		:POSITION1;
	float4 normal	:POSITION2;
	float4 light	:POSITION3;
};

//������������������������������������������������������������������������������
// ���_�V�F�[�_
//������������������������������������������������������������������������������
VS_OUT VS(float4 pos : POSITION, float4 uv : TEXCOORD, float4 normal : NORMAL)
{
	//�s�N�Z���V�F�[�_�[�֓n�����
	VS_OUT outData = (VS_OUT)0;

	//���[�J�����W�ɁA���[���h�E�r���[�E�v���W�F�N�V�����s���������
	//�X�N���[�����W�ɕϊ����A�s�N�Z���V�F�[�_�[��

	//pos = pos + normal * 0.05;

	outData.pos = mul(pos, matWVP);
	outData.uv = uv;
	normal.w = 0;// w�̏���0
	//�@������]
	normal = mul(normal, matNormal);
	normal = normalize(normal);
	outData.normal = normal;

	//float4 light = float4( 1.0, 0.8, -1.5, 0);    //�����̌����i���̍��W���������"����"�j ����^�C�v������΋t�̃^�C�v������
	float4 light = normalize(lightDirection);

	outData.color = saturate(dot(normal, light));
	float4 posw = mul(pos, matW);
	outData.eyev = eyePos - posw;

	//�܂Ƃ߂ďo��
	return outData;
}

//������������������������������������������������������������������������������
// �s�N�Z���V�F�[�_
//������������������������������������������������������������������������������
float4 PS(VS_OUT inData) : SV_Target
{

	float4 lightSource = float4(1.0, 1.0, 1.0, 1.0);//�F��4���F
	//float4 ambentSource = float4(0.2, 0.2, 0.2, 1.0);//���邭����΃c���c������ ����float4����Y���ƒl���S��1�ɂȂ��Ĉُ�Ȓ����邭�Ȃ邩��C�������I�I�I
	float4 diffuse;
	float4 ambient;
	float4 NL = dot(inData.normal, normalize(lightDirection));
	float4 reflect = normalize(2 * NL * inData.normal - normalize(lightDirection));
	float4 specular = pow(saturate(dot(reflect, normalize(inData.eyev))), shininess) * specularColor;

	//�摜�̏ꏊ�Ŗ��x�݂����Ȃ�ς���Ȃ�if��������˂݂�����
	float4 n1 = float4(1 / 4.0, 1 / 4.0, 1 / 4.0, 1);
	float4 n2 = float4(2 / 4.0, 2 / 4.0, 2 / 4.0, 1);
	float4 n3 = float4(3 / 4.0, 3 / 4.0, 3 / 4.0, 1);
	float4 n4 = float4(4 / 4.0, 4 / 4.0, 4 / 4.0, 1);

	float4 tI = 0.1 * step(n1, inData.color) + 0.2 * step(n2, inData.color)
			  + 0.3 * step(n3, inData.color) + 0.4 * step(n4, inData.color);

	//���ς̌��ʂ��}�C�i�X�̏ꍇ�͋��ʔ��˂͋N����Ȃ���ԁB�}�C�i�X�̂܂܂ł͂Ȃ��O�ɂ��Čv�Z����K�v������
	if (isTexture == false) {
		diffuse = lightSource * diffuseColor * tI;//�g�U���ːF
		ambient = lightSource * diffuseColor * ambientColor;//�����ːF

	}
	else {
		
		diffuse = lightSource * g_texture.Sample(g_sampler, inData.uv) * tI;//�g�U���ːF
		ambient = lightSource * g_texture.Sample(g_sampler, inData.uv) * ambientColor;//�����ːF
	
	}

	return diffuse + ambient + specular;//���ۂ̐F
	//return diffuse + ambient;

	// Postarization
	/*float4 output = floor(g_texture.Sample(g_sampler,inData.uv) * 8.0) / 8;
	return output;*/
}

//�F�����炿�炵�Ă����ȐF�ɕς��p�`���R�d�l�ɂȂ�Ƃ���Direct3D�̒��̃N���G�C�g�V�F�[�_�[�̒���Simple3D��2D�̕\�L���Ԉ���Ă�\����I�I