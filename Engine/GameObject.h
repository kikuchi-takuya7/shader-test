#pragma once
#include "Transform.h"
#include <list>
#include <string>

class SphereCollider;

using std::string;
using std::list;

#define SAFE_RELEASE(p) if(p != nullptr){ p->Release(); p = nullptr;}
#define SAFE_DELETE(p) if(p != nullptr){ delete p; p = nullptr;}

class GameObject
{
private:
	bool				IsDead_;

protected:
	list<GameObject*>	childList_;
	Transform			transform_;
	GameObject*			pParent_;
	string				objectName_;
	SphereCollider*		pCollider_;
	//list<SphereCollider*> colliderList_;

public:
	GameObject();
	GameObject(GameObject* parent, const string& name);
	~GameObject();

	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Release() = 0;
	void DrawSub();
	void UpdateSub();
	void ReleaseSub();
	void KillMe();
	
	virtual void OnCollision(GameObject* pTarget) {};

	GameObject* FindChildObject(string _objName);
	GameObject* GetRootJob();
	GameObject* FindObject(string _objName);

	const std::string& GetObjectName(void) const;

	void AddCollider(SphereCollider* pCollider);
	void Collision(GameObject* pTarget);
	void RoundRobin(GameObject* pTarget);

	void SetTransform(Transform t) { transform_ = t; }
	Transform GetTransform() { return transform_; }

	void SetPosition(XMFLOAT3 p) { transform_.position_ = p; }
	void SetPosition(float x, float y, float z) { SetPosition(XMFLOAT3(x,y,z)); }
	void SetScale(XMFLOAT3 s) { transform_.scale_ = s; }
	void SetScale(float x, float y, float z) { SetScale(XMFLOAT3(x, y, z)); }
	void SetRotate(XMFLOAT3 r) { transform_.rotate_ = r; }
	void SetRotate(float x, float y, float z) { SetRotate(XMFLOAT3(x, y, z)); }
	void SetRotateX(float x) { SetRotate(x, transform_.rotate_.y, transform_.rotate_.z); }
	void SetRotateY(float y) { SetRotate(transform_.rotate_.x, y, transform_.rotate_.z); }
	void SetRotateZ(float z) { SetRotate(transform_.rotate_.x, transform_.rotate_.y, z); }
	XMFLOAT3 GetPosition() { return transform_.position_; }
	XMFLOAT3 GetRotate() { return transform_.rotate_; }
	XMFLOAT3 GetScale() { return transform_.scale_; }

	template<class T>
	GameObject* Instantiate(GameObject* parent)
	{
		T* P;
		P = new T(parent);
		P->Initialize();
		parent->childList_.push_back(P);
		return P;
	}
};