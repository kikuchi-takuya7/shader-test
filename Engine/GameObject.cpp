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
		return(this); //������_objName�̃I�u�W�F�N�g������
	}
	else {
		//for (auto itr = childList_.begin(); itr != childList_.end(); itr++) { //�������̓|�C���^�A�X�^���X�N���K�v
		for(auto itr:childList_){ //�������̓A�h���X���̂܂܍s�����炨����
			GameObject* obj = itr->FindChildObject(_objName);
			if (obj != nullptr)
				return obj;
		}
	}

	return nullptr;
}

/// <summary>
/// �ċN�Ăяo����RootJob��T���Ă��̃A�h���X��Ԃ��֐�
/// </summary>
/// <returns>RootJob�̃A�h���X</returns>
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
		return;//�^�[�Q�b�g�ɃR���C�_�[���A�^�b�`����Ă��Ȃ�
	}

	/*XMVECTOR v = XMLoadFloat3(&transform_.position_);
	XMVECTOR tv = XMLoadFloat3(&pTarget->transform_.position_);
	v = v - tv;
	XMVECTOR dist = XMVector3Dot(v, v);*/
	float dist = (transform_.position_.x - pTarget->transform_.position_.x) * (transform_.position_.x - pTarget->transform_.position_.x) +
		(transform_.position_.y - pTarget->transform_.position_.y) * (transform_.position_.y - pTarget->transform_.position_.y) +
		(transform_.position_.z - pTarget->transform_.position_.z) * (transform_.position_.z - pTarget->transform_.position_.z);
	float rDist = (this->pCollider_->GetRadius() + pTarget->pCollider_->GetRadius()) * (this->pCollider_->GetRadius() + pTarget->pCollider_->GetRadius());
	//�����ƃ^�[�Q�b�g�̋����@<= R1+R2�Ȃ�
	//�����A�����̃R���C�_�[�ƃ^�[�Q�b�g���Ԃ����Ă�����
	//onCollision(pTarget)���Ăяo��
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
	if (pTarget->pCollider_ != nullptr) { //�����ƃ^�[�Q�b�g
		Collision(pTarget);
	}
	//�����̎q���S���ƃ^�[�Q�b�g
	for (auto itr = pTarget->childList_.begin(); itr != pTarget->childList_.end();itr++) {
		RoundRobin(*itr);
	}
}
