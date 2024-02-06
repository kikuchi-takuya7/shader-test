//───────────────────────────────────────
 // テクスチャ＆サンプラーデータのグローバル変数定義
//───────────────────────────────────────
Texture2D		g_texture : register(t0);	//テクスチャー
SamplerState	g_sampler : register(s0);	//サンプラー

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
	float4 color	: COLOR;	//色（明るさ）
};

//───────────────────────────────────────
// 頂点シェーダ
//───────────────────────────────────────
VS_OUT VS(float4 pos : POSITION, float4 uv : TEXCOORD, float4 normal : NORMAL)
{
	//ピクセルシェーダーへ渡す情報
	VS_OUT outData;

	//ローカル座標に、ワールド・ビュー・プロジェクション行列をかけて
	//スクリーン座標に変換し、ピクセルシェーダーへ
	outData.pos = mul(pos, matWVP);
	outData.uv = uv;

	//法線を回転
	normal = mul(normal, matNormal);
	float4 light = float4(1, 0.8, -0.3, 0);//光が出てくる方向。こっち向いてるタイプ
	light = normalize(light);
	outData.color = clamp(dot(normal, light), 0, 1);

	//まとめて出力
	return outData;
}

//───────────────────────────────────────
// ピクセルシェーダ
//───────────────────────────────────────
float4 PS(VS_OUT inData) : SV_Target
{

	float4 lightSource = float4(1.0, 1.0, 1.0, 1.0);//色の4原色
	float4 ambentSource = float4(0.2, 0.2, 0.2, 1.0);//明るくすればツルツルする ここfloat4入れ忘れると値が全部1になって異常な程明るくなるから気をつけろよ！！！
	float4 diffuse;
	float4 ambient;

	if (hasTexture) {
		diffuse = lightSource * g_texture.Sample(g_sampler, inData.uv) * inData.color;//拡散反射色
		ambient = lightSource * g_texture.Sample(g_sampler, inData.uv) * ambentSource;//環境反射色
	}
	else {
		diffuse = lightSource * diffuseColor * inData.color;//拡散反射色
		ambient = lightSource * diffuseColor * ambentSource;//環境反射色
	}
	return (diffuse + ambient);//実際の色
}

//色がちらちらしていろんな色に変わるパチンコ仕様になるときはDirect3Dの中のクリエイトシェーダーの中のSimple3Dと2Dの表記が間違ってる可能性大！！