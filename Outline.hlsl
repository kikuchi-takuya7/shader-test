//───────────────────────────────────────
// テクスチャ＆サンプラーデータのグローバル変数定義
//───────────────────────────────────────
Texture2D		g_texture : register(t0);	//テクスチャー
SamplerState	g_sampler : register(s0);	//サンプラー

Texture2D		g_toon_texture : register(t1); //レジスターにセットテクスチャ

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
	float4		ambientColor;	// 環境光
	float4		specularColor;
	float	    shininess;
	bool		isTexture;		// テクスチャ貼ってあるかどうか
};

cbuffer global:register(b1)
{
	float4		lightDirection;
	float4		eyePos;
}

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
float4 VS(float4 pos : POSITION, float4 uv : TEXCOORD, float4 normal : NORMAL):SV_POSITION
{
	//面の法線の方に座標を引っ張る
	//法線の隙間は勝手に補完してくれるから法線を少し大きくすればそれごと大きくなってく
	
	normal.w = 0;// wの情報は0。メモ帳にあるけどwの値は平行移動に使う値だから、なくなるッテ事？わからんけどたぶんそう？
	pos = pos + normal * 0.05;

	pos = mul(pos, matWVP);
	
	
	return pos;
}

//───────────────────────────────────────
// ピクセルシェーダ
//───────────────────────────────────────
float4 PS(VS_OUT inData) : SV_Target
{

	//ここは色とか明度を返すところだから、色を黒にして返してやる
	return float4(0,0,0,1);
}

//色がちらちらしていろんな色に変わるパチンコ仕様になるときはDirect3Dの中のクリエイトシェーダーの中のSimple3Dと2Dの表記が間違ってる可能性大！！