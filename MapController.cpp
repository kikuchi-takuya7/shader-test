#include "MapController.h"
#include "Engine/Input.h"
#include "Engine/Camera.h"
#include "Engine/Model.h"

//コンストラクタ
MapController::MapController(GameObject* parent)
    :GameObject(parent, "MapController")
{
}

//デストラクタ
MapController::~MapController()
{
}

//初期化
void MapController::Initialize()
{
    //transform_.rotate_.x = 45.0f;
}

//更新
void MapController::Update()
{

    /*if (Input::IsKey(DIK_LSHIFT) && transform_.position_.y <= 10.0f) {
        transform_.position_.y += 0.2f;
    }
    if (Input::IsKey(DIK_LCONTROL) && transform_.position_.y >= 0.0f) {
        transform_.position_.y -= 0.2f;
    }

    if (Input::IsKey(DIK_LEFTARROW)) {
        transform_.rotate_.y -= 1.0f;
    }
    if (Input::IsKey(DIK_RIGHTARROW)) {
        transform_.rotate_.y += 1.0f;
    }

    if (Input::IsKey(DIK_UPARROW) && transform_.rotate_.x <= 88) {
        transform_.rotate_.x += 1.0f;
    }
    if (Input::IsKey(DIK_DOWNARROW) && transform_.rotate_.x >= 0) {
        transform_.rotate_.x -= 1.0f;
    }*/

    //transform.rotate_.y度回転させる行列を作成
    XMMATRIX rotY = XMMatrixRotationY(XMConvertToRadians(transform_.rotate_.y));

    XMMATRIX rotX = XMMatrixRotationX(XMConvertToRadians(transform_.rotate_.x));

    //現在の位置をベクトル型に変換
    XMVECTOR pos = XMLoadFloat3(&transform_.position_);

    //1フレームの移動ベクトル
    XMVECTOR moveZ = { 0.0f, 0.0f, 0.1f, 0.0f }; //奥に0.1m

    //移動ベクトルを変形 (洗車と同じ向きに回転) させる
    moveZ = XMVector3TransformCoord(moveZ, rotY);


    //Wキーが押されたら移動
    if (Input::IsKey(DIK_W)) {
        //移動
        pos += moveZ;

    }

    if (Input::IsKey(DIK_S)) {

        pos -= moveZ;

    }

    //1フレームの移動ベクトル
    XMVECTOR moveX = { 0.1f, 0.0f, 0.0f, 0.0f }; //横に0.1m

    //移動ベクトルを変形 (洗車と同じ向きに回転) させる
    moveX = XMVector3TransformCoord(moveX, rotY);


    if (Input::IsKey(DIK_A)) {

        pos -= moveX;

    }

    if (Input::IsKey(DIK_D)) {

        pos += moveX;

    }

    //現在地をベクトルからいつものtransform.positionに戻す
    XMStoreFloat3(&transform_.position_, pos);

    //ポジションを常に見る。
    Camera::SetTarget(transform_.position_);

    //カメラの位置は常にポジションの後ろ
    XMVECTOR vCam = { 0,0,-10,0 };
    vCam = XMVector3TransformCoord(vCam, rotX * rotY);
    Camera::SetPosition(pos + vCam);
}

//描画
void MapController::Draw()
{
}

//開放
void MapController::Release()
{
}