#include "Live2DObjectFactory.hpp"
#include "ISerializable.hpp"
#include "ModelImpl.hpp"
#include "Avatar.hpp"
#include "PartsData.hpp"
#include "ParamDefFloat.hpp"
#include "ParamDefSet.hpp"
#include "ParamPivots.hpp"
#include "PivotManager.hpp"
#include "Deformer.hpp"
#include "RotationDeformer.hpp"
#include "WarpDeformer.hpp"
#include "AffineEnt.hpp"
#include "Mesh.hpp"
#include <stdexcept>
#include <string>

namespace live2d {

std::unique_ptr<ISerializable> Live2DObjectFactory::create(int clsNo) {
    if (clsNo < 100) {
        switch (clsNo) {
            case 65: return std::make_unique<WarpDeformer>();
            case 66: return std::make_unique<PivotManager>();
            case 67: return std::make_unique<ParamPivots>();
            case 68: return std::make_unique<RotationDeformer>();
            case 69: return std::make_unique<AffineEnt>();
            case 70: return std::make_unique<Mesh>();
            default: break;
        }
    } else if (clsNo < 150) {
        switch (clsNo) {
            case 131: return std::make_unique<ParamDefFloat>();
            case 133: return std::make_unique<PartsData>();
            case 136: return std::make_unique<ModelImpl>();
            case 137: return std::make_unique<ParamDefSet>();
            case 142: return std::make_unique<Avatar>();
            default: break;
        }
    }

    throw std::runtime_error("Unknown class ID: " + std::to_string(clsNo));
}

void live2d::ISerializable::ISerializableRelease(void* ptr)
{
    if (!ptr) return;  // 释放空指针安全

    int clsNo = static_cast<ISerializable*>(ptr)->type;

    if (clsNo < 100)
    {
        switch (clsNo)
        {
        case 65: delete static_cast<WarpDeformer*>(ptr); return;
        case 66: delete static_cast<PivotManager*>(ptr); return;
        case 67: delete static_cast<ParamPivots*>(ptr); return;
        case 68: delete static_cast<RotationDeformer*>(ptr); return;
        case 69: delete static_cast<AffineEnt*>(ptr); return;
        case 70: delete static_cast<Mesh*>(ptr); return;
        default: break;
        }
    }
    else if (clsNo < 150)
    {
        switch (clsNo)
        {
        case 131: delete static_cast<ParamDefFloat*>(ptr); return;
        case 133: delete static_cast<PartsData*>(ptr); return;
        case 136: delete static_cast<ModelImpl*>(ptr); return;
        case 137: delete static_cast<ParamDefSet*>(ptr); return;
        case 142: delete static_cast<Avatar*>(ptr); return;
        default: break;
        }
    }

    // 未知类型：无法安全删除，抛出异常或记录错误
    throw std::runtime_error("ISerializableRelease: unknown class ID " + std::to_string(clsNo));
}
} // namespace live2d
