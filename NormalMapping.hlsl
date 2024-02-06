//───────────────────────────────────────
 // テクスチャ＆サンプラーデータのグローバル変数定義
//───────────────────────────────────────
Texture2D		g_texture : register(t0);	//テクスチャー
SamplerState	g_sampler : register(s0);	//サンプラー
Texture2D		normalTex : register(t1);

//───────────────────────────────────────
// コンスタントバッファ
// DirectX 側から送信されてくる、ポリゴン頂点以外の諸情報の定義
//───────────────────────────────────────
cbuffer global:register(b0)
{
	float4x4	matWVP;			// ワールド・ビュー・プロジェクションの合成行列
	float4x4	matW;           // ワールド行列
	float4x4	matNormal;           //ノーマルのローカルへの変換行列から平行移動成分をとったやつ
	float4		diffuseColor;		//マテリアルの色＝拡散反射係数
	float4		ambientColor;		//環境光
	float4		specularColor;		//鏡面反射＝ハイライトの係数
	float		shininess;			//ハイライトの広がりの大きさ
	int			hasTexture;			//テクスチャーが貼られているかどうか
	int			hasNormalMap;		//ノーマルマップがあるかどうか
	float		g_scroll;	//スクロール量の変数
};

cbuffer gmodel:register(b1) {

	float4		lightPosition;		//っ高原の位置
	float4		eyePosition;		//支店位置＝カメラの位置
}

//───────────────────────────────────────
// 頂点シェーダー出力＆ピクセルシェーダー入力データ構造体
//───────────────────────────────────────
struct VS_OUT
{
	float4 pos  : SV_POSITION;	//位置
	float2 uv	: TEXCOORD;		//UV座標
	float4 eyev		:POSITION;	//ワールド座標に変換された視線ベクトル
	float4 Neyev	:POSITION1;	//ノーマルマップ用の接空間に変換された視線ベクトル
	float4 normal	:POSITION2; //法線ベクトル
	float4 light	:POSITION3; //ライトを接空間に変換したベクトル
	float4 color	:POSITION4;	//通常のランバートモデルの拡散反射の色
};

//───────────────────────────────────────
// 頂点シェーダ
//───────────────────────────────────────
VS_OUT VS(float4 pos : POSITION, float4 uv : TEXCOORD, float4 normal : NORMAL, float4 tangent : TANGENT)
{
	//ピクセルシェーダーへ渡す情報
	VS_OUT outData = (VS_OUT)0;

	//ローカル座標に、ワールド・ビュー・プロジェクション行列をかけて
	//スクリーン座標に変換し、ピクセルシェーダーへ
	outData.pos = mul(pos, matWVP);
	outData.uv = uv;

	//法線とtan（従法線）の外積を求める？
	float3  binormal = cross(normal, tangent);

	normal.w = 0;
	normal = mul(normal, matNormal);
	normal = normalize(normal); //法線ベクトルをローカル座標に変換したやつ
	outData.normal = normal;

	tangent.w = 0;
	tangent = mul(tangent, matNormal);
	tangent = normalize(tangent); //接線ベクトルをローカル座標に変換したやつ

	binormal = mul(binormal, matNormal);
	binormal = normalize(binormal); //従法線ベクトルをローカル座標に変換したやつ

	float4 posw = mul(pos, matW);
	float4 eye = normalize(posw - eyePosition); //ワールド座標の視線ベクトル 先生のヒントがここを変えてたから変えたけど、特に変化なし

	//接空間の視線ベクトル
	outData.Neyev.x = dot(eye, tangent);
	outData.Neyev.y = dot(eye, binormal);
	outData.Neyev.z = dot(eye, normal);
	outData.Neyev.w = 0;

	float4 light = normalize(lightPosition);
	light = normalize(light);

	outData.color = mul(light, normal);
	outData.color.w = 0.0;

	//接空間の光源ベクトル
	outData.light.x = dot(light, tangent);
	outData.light.y = dot(light, binormal);
	outData.light.z = dot(light, normal);
	outData.light.w = 0;

	//まとめて出力
	return outData;
}

//───────────────────────────────────────
// ピクセルシェーダ
//───────────────────────────────────────
float4 PS(VS_OUT inData) : SV_Target
{

	float4 lightSource = float4(1.0, 1.0, 1.0, 1.0);//色の4原色
	float4 diffuse;
	float4 ambient;

	float2 tmpUV = inData.uv;
	tmpUV.x += g_scroll;

	if (hasNormalMap)
	{
		//inData.light = normalize(inData.light);

		float4 tmpNormal = normalTex.Sample(g_sampler, tmpUV) * 2.0f - 1.0f;
		tmpNormal.w = 0;
		tmpNormal = normalize(tmpNormal);

		float4 NL = clamp(dot(tmpNormal, inData.light), 0, 1);

		float4 light = normalize(lightPosition);
		light = normalize(light);

		float4 reflection = reflect(light, tmpNormal);
		float4 specular = pow(saturate(dot(reflection, inData.Neyev)), shininess) * specularColor;

		if (hasTexture != 0)
		{
			diffuse = lightSource * g_texture.Sample(g_sampler, tmpUV) * NL;
			ambient = lightSource * g_texture.Sample(g_sampler, tmpUV) * ambientColor;
		}
		else
		{
			diffuse = lightSource * diffuseColor * NL;
			ambient = lightSource * diffuseColor * ambientColor;
		}
		return   ambient + diffuse + specular;
	}
	else
	{
		float4 reflection = reflect(normalize(lightPosition), inData.normal);

		float4 specular = pow(saturate(dot(reflection, normalize(inData.eyev))), shininess) * specularColor;
		if (hasTexture == 0)
		{
			diffuse = lightSource * diffuseColor * inData.color;
			ambient = lightSource * diffuseColor * ambientColor;
		}
		else
		{
			diffuse = lightSource * g_texture.Sample(g_sampler, tmpUV) * inData.color;
			ambient = lightSource * g_texture.Sample(g_sampler, tmpUV) * ambientColor;
		}
		return ambient + diffuse + specular;
	}
}

//色がちらちらしていろんな色に変わるパチンコ仕様になるときはDirect3Dの中のクリエイトシェーダーの中のSimple3Dと2Dの表記が間違ってる可能性大！！