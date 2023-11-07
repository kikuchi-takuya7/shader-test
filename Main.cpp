//インクルード
#include <Windows.h>
#include <stdlib.h>
#include "Engine/Direct3D.h"
#include "Engine/Input.h"
#include "Engine/Camera.h"
#include "Engine/RootJob.h"
#include "Engine/Model.h"

#pragma comment(lib, "winmm.lib")

const char* WIN_CLASS_NAME = "SanpleGame";
const int WINDOW_WIDTH = 800;  //ウィンドウの幅
const int WINDOW_HEIGHT = 600; //ウィンドウの高さ

RootJob* pRootJob = new RootJob;


//プロトタイプ宣言
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//エントリーポイント
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	
	//ウィンドウクラス（設計図）を作成
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);             //この構造体のサイズ
	wc.hInstance = hInstance;                   //インスタンスハンドル
	wc.lpszClassName = WIN_CLASS_NAME;            //ウィンドウクラス名
	wc.lpfnWndProc = WndProc;                   //ウィンドウプロシージャ
	wc.style = CS_VREDRAW | CS_HREDRAW;         //スタイル（デフォルト）
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); //アイコン　　　　　　　　　　　自由に
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);   //小さいアイコン　　　　　　　　　変更
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);   //マウスカーソル　　　　　　　　　　可能
	wc.lpszMenuName = NULL;                     //メニュー（なし）
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //背景（白）

	RegisterClassEx(&wc);  //クラスを登録


	//ウィンドウサイズの計算
	RECT winRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, FALSE);
	int winW = winRect.right - winRect.left;     //ウィンドウ幅
	int winH = winRect.bottom - winRect.top;     //ウィンドウ高さ


	//ウィンドウを作成
	HWND hWnd = CreateWindow(
		WIN_CLASS_NAME,         //ウィンドウクラス名
		"サンプルゲーム",     //タイトルバーに表示する内容
		WS_OVERLAPPEDWINDOW, //スタイル（普通のウィンドウ） スタイルを変えるとウィンドウの大きさ変更だったりをできなくできる
		CW_USEDEFAULT,       //表示位置左（おまかせ）
		CW_USEDEFAULT,       //表示位置上（おまかせ）
		winW,                 //ウィンドウ幅
		winH,                 //ウィンドウ高さ
		NULL,                //親ウインドウ（なし）
		NULL,                //メニュー（なし）
		hInstance,           //インスタンス
		NULL                 //パラメータ（なし）
	);

	//ウィンドウを表示
	ShowWindow(hWnd, nCmdShow);
	
	//Direct3Dの初期化
	HRESULT hr;
	hr = Direct3D::Initialize(winW, winH, hWnd);
	if (FAILED(hr)) {
		PostQuitMessage(0);
	}

	Camera::Initialize();
	Camera::SetTarget(XMFLOAT3(0, 0, 0));
	Camera::SetPosition(XMFLOAT3(0, 3, -10));

	//DirectInputの初期化
	Input::Initialize(hWnd);
	/*if (FAILED(hr)) {
		PostQuitMessage(0);
	}*/
	
	pRootJob = new RootJob(nullptr);//これ以上上の親には何もないってこと
	pRootJob->Initialize();

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		//メッセージあり
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//メッセージなし
		else
		{
			timeBeginPeriod(1); //ここの間はmm秒単位で計算するようになる

			static DWORD countFps = 0;

			static DWORD startTime = timeGetTime();//このプログラムが起動したタイミングのwindows起動時間
			DWORD nowTime = timeGetTime();//Windowsが起動してからの時間だから、このパソコン自体の起動時間が出てくる
			static DWORD lastUpdateTime = nowTime;

			if (nowTime - startTime >= 1000)
			{
				char str[16];
				wsprintf(str, "%u", countFps);
				SetWindowText(hWnd, str);
				countFps = 0;
				startTime = nowTime;
			}

			if ((nowTime - lastUpdateTime) * 60 <= 1000.0f)//前回の更新から1/60秒立ってないなら休み
			{
				continue;
			}
			lastUpdateTime = nowTime;

			countFps++;
			timeEndPeriod(1);

			//ゲームの処理
			//カメラの更新
			Camera::Update();

			//入力の処理
			Input::Update();
			pRootJob->UpdateSub();

			//描画
			Direct3D::BeginDraw();

			//ルートジョブからすべてのオブジェクトのドローを呼ぶ
			pRootJob->DrawSub();

			Direct3D::EndDraw();
		}
	}

	Model::Release();
	pRootJob->ReleaseSub();
	Input::Release();
	Direct3D::Release();

	return 0;
}


//ウィンドウプロシージャ（何かあった時によばれる関数）
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)//メッセージ
	{
	case WM_CLOSE:
		PostQuitMessage(0);  //プログラム終了
		return 0;

	case WM_MOUSEMOVE://マウスが動いたら
		Input::SetMousePosition(LOWORD(lParam), HIWORD(lParam));//この時、ウィンドウプロシージャの引数lParamにマウスカーソルの位置が入る。
		return 0;												//（下位ビットにX座標、上位ビットにY座標）

	case WM_DESTROY://ウィンドウが閉じられたら
		PostQuitMessage(0);  //プログラム終了
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

