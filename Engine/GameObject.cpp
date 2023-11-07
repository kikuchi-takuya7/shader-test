#include "GameObject.h"
#include "SphereCollider.h"

GameObject::GameObject() :IsDead_(false),pParent_(nullptr)
{
	
}

GameObject::GameObject(GameObject* parent, const std::string& name) :IsDead_(false),pParent_(parent),objectName_(name)
{

	//pParent_ = parent;
	//objectName_ = name;
	if (pParent_ != nullptr) {
		this->transform_.pParent_ = &(parent->transform_);
	}
}

GameObject::~GameObject()
{
}

void GameObject::DrawSub()
{
	Draw();
	for (auto itr = childList_.begin(); itr != childList_.end(); itr++) {
		(*itr)->DrawSub();
	}
}

void GameObject::UpdateSub()
{

	Update();

	RoundRobin(GetRootJob());
	for (auto itr = childList_.begin(); itr != childList_.end(); itr++) {
		(*itr)->UpdateSub();
	}

	for (auto itr = childList_.begin(); itr != childList_.end();) {


		if ((*itr)->IsDead_) {
			(*itr)->ReleaseSub();
			SAFE_DELETE(*itr);
			itr = childList_.erase(itr);
		}
		else {
			itr++;
		}
	}
}

void GameObject::ReleaseSub()
{
	for (auto itr = childList_.begin(); itr != childList_.end(); itr++) {
		(*itr)->ReleaseSub();
	}
	Release();
}

void GameObject::KillMe()
{
	IsDead_ = true;
}

GameObject* GameObject::FindChildObject(string _objName)
{
	if (_objName == this->objectName_) {
		return(this); //自分が_objNameのオブジェクトだった
	}
	else {
		//for (auto itr = childList_.begin(); itr != childList_.end(); itr++) { //こっちはポインタアスタリスクが必要
		for(auto itr:childList_){ //こっちはアドレスそのまま行くからおっけ
			GameObject* obj = itr->FindChildObject(_objName);
			if (obj != nullptr)
				return obj;
		}
	}

	return nullptr;
}

/// <summary>
/// 再起呼び出しでRootJobを探してそのアドレスを返す関数
/// </summary>
/// <returns>RootJobのアドレス</returns>
GameObject* GameObject::GetRootJob()
{
	if(pParent_ == nullptr)
		return this;
	
	return pParent_->GetRootJob();
}

GameObject* GameObject::FindObject(string _objName)
{
	return GetRootJob()->FindChildObject(_objName);
}

const std::string& GameObject::GetObjectName(void) const
{
	return objectName_;
}

void GameObject::AddCollider(SphereCollider* pCollider)
{
	pCollider_ = pCollider;
	//colliderList_.push_back(pCollider_);
}

void GameObject::Collision(GameObject* pTarget)
{

	if (pTarget == this || pTarget->pCollider_ == nullptr) {
		return;//ターゲットにコライダーがアタッチされていない
	}

	/*XMVECTOR v = XMLoadFloat3(&transform_.position_);
	XMVECTOR tv = XMLoadFloat3(&pTarget->transform_.position_);
	v = v - tv;
	XMVECTOR dist = XMVector3Dot(v, v);*/
	float dist = (transform_.position_.x - pTarget->transform_.position_.x) * (transform_.position_.x - pTarget->transform_.position_.x) +
		(transform_.position_.y - pTarget->transform_.position_.y) * (transform_.position_.y - pTarget->transform_.position_.y) +
		(transform_.position_.z - pTarget->transform_.position_.z) * (transform_.position_.z - pTarget->transform_.position_.z);
	float rDist = (this->pCollider_->GetRadius() + pTarget->pCollider_->GetRadius()) * (this->pCollider_->GetRadius() + pTarget->pCollider_->GetRadius());
	//自分とターゲットの距離　<= R1+R2なら
	//もし、自分のコライダーとターゲットがぶつかっていたら
	//onCollision(pTarget)を呼び出す
	if (dist <= rDist) {
		OnCollision(pTarget);
	}

}

void GameObject::RoundRobin(GameObject* pTarget)
{
	/*for (auto itr : colliderList_) {
		if (itr == this->pCollider_) {
			continue;
		}
	}*/

	if (pCollider_ == nullptr) {
		return;
	}
	if (pTarget->pCollider_ != nullptr) { //自分とターゲット
		Collision(pTarget);
	}
	//自分の子供全部とターゲット
	for (auto itr = pTarget->childList_.begin(); itr != pTarget->childList_.end();itr++) {
		RoundRobin(*itr);
	}
}
