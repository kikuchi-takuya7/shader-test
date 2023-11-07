#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>


//リンカ
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#define SAFE_RELEASE(p) if(p != nullptr){ p->Release(); p = nullptr;}
#define SAFE_DELETE(p) if(p != nullptr){ delete p; p = nullptr;}

//定数宣言
//const char* WIN_CLASS_NAME = "SanpleGame";
//extern const char* WIN_CLASS_NAME;

enum SHADER_TYPE
{
	SHADER_2D,
	SHADER_3D,
	SHADER_MAX,
};


namespace Direct3D

{
	extern ID3D11Device* pDevice_;		 //デバイス
	extern ID3D11DeviceContext* pContext_;//デバイスコンテキスト

	//初期化
	HRESULT Initialize(int winW, int winH, HWND hWnd);

	//シェーダー準備
	HRESULT InitShader();
	HRESULT InitShader2D();
	HRESULT InitShader3D();

	void SetShader(SHADER_TYPE type);

	//描画開始
	void BeginDraw();

	//描画終了
	void EndDraw();

	//解放
	void Release();

};