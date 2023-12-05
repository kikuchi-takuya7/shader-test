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
	float4x4	matW;			// ワールド返還行列とかだったっけ
	float4x4	matNormal;           // ワールド行列
	float4		diffuseColor;		// ディフューズカラー（マテリアルの色）
	float4		lightDirection;
	float4		eyePos;
	bool		isTexture;		// テクスチャ貼ってあるかどうか
};

//───────────────────────────────────────
// 頂点シェーダー出力＆ピクセルシェーダー入力データ構造体
//───────────────────────────────────────
struct VS_OUT
{
	float4 pos  : SV_POSITION;	//位置
	float2 uv	: TEXCOORD;		//UV座標
	float4 color	: COLOR;	//色（明るさ）
	float4 eyev		:POSITION1;
	float4 normal	:POSITION2;
	float4 light	:POSITION3;
};

//───────────────────────────────────────
// 頂点シェーダ
//───────────────────────────────────────
VS_OUT VS(float4 pos : POSITION, float4 uv : TEXCOORD, float4 normal : NORMAL)
{
	//ピクセルシェーダーへ渡す情報
	VS_OUT outData = (VS_OUT)0;

	//ローカル座標に、ワールド・ビュー・プロジェクション行列をかけて
	//スクリーン座標に変換し、ピクセルシェーダーへ
	outData.pos = mul(pos, matWVP);
	outData.uv = uv;
	normal.w = 0;
	//法線を回転
	normal = mul(normal, matNormal);
	normal = normalize(normal);
	outData.normal = normal;

	//float4 light = float4( 1.0, 0.8, -1.5, 0);    //光源の向き（この座標から光源が"来る"） くるタイプもあれば逆のタイプもある
	float4 light = normalize(lightDirection);
	light = normalize(light);

	outData.color = saturate(dot(normal, light));
	float4 posw = mul(pos, matW);
	outData.eyev = eyePos - posw;

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
	float4 NL = saturate(dot(inData.normal, normalize(lightDirection)));
	float4 reflect = normalize(2 * NL * inData.normal - normalize(lightDirection));
	float4 specular = pow(saturate(dot(reflect, normalize(inData.eyev))), 8);

	//内積の結果がマイナスの場合は鏡面反射は起こらない状態。マイナスのままではなく０にして計算する必要がある
	if (isTexture ==  true) {
		diffuse = lightSource * g_texture.Sample(g_sampler, inData.uv) * inData.color;//拡散反射色
		ambient = lightSource * g_texture.Sample(g_sampler, inData.uv) * ambentSource;//環境反射色
	}
	else {
		diffuse = lightSource * diffuseColor * inData.color;//拡散反射色
		ambient = lightSource * diffuseColor * ambentSource;//環境反射色
	}
	return (diffuse + ambient + specular);//実際の色

	//// Postarization
	//float4 output = floor(g_texture.Sample(g_sampler,inData.uv) * 8.0) / 8;
	//return output;
}

//色がちらちらしていろんな色に変わるパチンコ仕様になるときはDirect3Dの中のクリエイトシェーダーの中のSimple3Dと2Dの表記が間違ってる可能性大！！