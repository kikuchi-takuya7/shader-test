#include "Input.h"


namespace Input
{

	LPDIRECTINPUT8   pDInput = nullptr; //�����Ń|�C���^�ɂȂ��Ă邩��nullptr
	LPDIRECTINPUTDEVICE8 pKeyDevice = nullptr; //�L�[�{�[�h�ɃA�N�Z�X���邽�߂̂��
	BYTE keyState[256] = { 0 }; //unsgind char�������̂��߂�ǂ�����������BYTE�L�[�̐��͈�ʓI��109�Ȃ̂ŏ\��
	BYTE prevKeyState[256];    //�O�t���[���ł̊e�L�[�̏��
	XMVECTOR mousePosition;

	void Initialize(HWND hWnd)
	{
		DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&pDInput, nullptr);
		pDInput->CreateDevice(GUID_SysKeyboard, &pKeyDevice, nullptr); //�N���G�C�g
		pKeyDevice->SetDataFormat(&c_dfDIKeyboard); //������n���f�o�C�X�̎�ނ��w��
		pKeyDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND); //���̎��s���̃A�v���ɑ΂���D��x�B�E�B���h�E���d�Ȃ��ĂĂ��������邩�ǂ����݂�����
	}

	void Update()
	{
		//�܂Ƃ܂����f�[�^���ۂ��ƃR�s�[�ł���֐��B���Ǝg������
		memcpy(prevKeyState, keyState, sizeof(BYTE)*256);

		pKeyDevice->Acquire();
		pKeyDevice->GetDeviceState(sizeof(keyState), &keyState); //���̏u�Ԃ̃L�[�{�[�h�̏�Ԃ���肷��
	}

	bool IsKey(int keyCode)
	{
		if (keyState[keyCode] & 0x80) //��i����128�B�L�[�{�[�h�̐���葽���]�T��������7bit
		{
			return true;
		}
		return false;
	}

	bool IsKeyDown(int keyCode)
	{
		if (prevKeyState[keyCode] ^ keyState[keyCode] && keyState[keyCode] & 0x80)//(Iskey(keyCode) && !(prevKeyState[keyCode] & 0x80))���Ă̂�����
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