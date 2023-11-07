#include "Input.h"


namespace Input
{

	LPDIRECTINPUT8   pDInput = nullptr; //内部でポインタになってるからnullptr
	LPDIRECTINPUTDEVICE8 pKeyDevice = nullptr; //キーボードにアクセスするためのやつ
	BYTE keyState[256] = { 0 }; //unsgind charを書くのがめんどくさかったらBYTEキーの数は一般的に109なので十分
	BYTE prevKeyState[256];    //前フレームでの各キーの状態
	XMVECTOR mousePosition;

	void Initialize(HWND hWnd)
	{
		DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&pDInput, nullptr);
		pDInput->CreateDevice(GUID_SysKeyboard, &pKeyDevice, nullptr); //クリエイト
		pKeyDevice->SetDataFormat(&c_dfDIKeyboard); //ここでn￥デバイスの種類を指定
		pKeyDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND); //他の実行中のアプリに対する優先度。ウィンドウが重なってても反応するかどうかみたいな
	}

	void Update()
	{
		//まとまったデータを丸っとコピーできる関数。割と使うかも
		memcpy(prevKeyState, keyState, sizeof(BYTE)*256);

		pKeyDevice->Acquire();
		pKeyDevice->GetDeviceState(sizeof(keyState), &keyState); //その瞬間のキーボードの状態を入手する
	}

	bool IsKey(int keyCode)
	{
		if (keyState[keyCode] & 0x80) //二進数で128。キーボードの数より多く余裕を持った7bit
		{
			return true;
		}
		return false;
	}

	bool IsKeyDown(int keyCode)
	{
		if (prevKeyState[keyCode] ^ keyState[keyCode] && keyState[keyCode] & 0x80)//(Iskey(keyCode) && !(prevKeyState[keyCode] & 0x80))ってのもあり
		{
			return true;
		}
		return false;
	}

	bool IsKeyUp(int keyCode)
	{
		if (prevKeyState[keyCode] ^ keyState[keyCode] && prevKeyState[keyCode] & 0x80)
		{
			return true;
		}
		return false;
	}

	XMVECTOR GetMousePosition()
	{
		return mousePosition;
	}


	void SetMousePosition(int x, int y)
	{
		mousePosition = XMVectorSet((float)x, (float)y, 0, 0);
	}

	void Release()
	{
		SAFE_RELEASE(pDInput);
		SAFE_RELEASE(pKeyDevice);
	}
}