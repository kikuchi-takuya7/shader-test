#pragma once
#include "GameObject.h"
#include "Fbx.h"
#include "Transform.h"
#include <string>
#include <vector>

using std::vector;
using std::string;

namespace Model{
    struct ModelData
    {
        Fbx* pFbx_;
        Transform transform_;
        string fileName_;
    };

    int Load(std::string _fileName);

    void SetTransform(int _hModel, Transform _t);

    void Draw(int _hModel);

    void Release();

    void AllRelease();

    void SetLightDirection(int hModel_, XMFLOAT4 pos);
}
