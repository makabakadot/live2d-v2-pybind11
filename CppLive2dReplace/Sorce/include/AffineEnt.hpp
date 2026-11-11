#pragma once

#include "ISerializable.hpp"

#include <Eigen/Geometry>

namespace live2d {

// 仿射参数：直接继承 Eigen::Affine2f，矩阵由参数 rebuild() 生成。
// 参数来自 .moc（origin/scale/rotation/reflect），插值仍在参数域进行，
// 插值结束后调用 rebuild() 即可得到可用的 Eigen 矩阵。
class AffineEnt final : public ISerializable, public Eigen::Affine2f {
public:
    AffineEnt() { rebuild(); }

    void read(class BinaryReader& br) override;

    // 参数 -> 矩阵：T(origin) * R(rot) * S(reflectX*scaleX, reflectY*scaleY)
    void rebuild() {
        const float rx = mReflectX ? -1.0f : 1.0f;
        const float ry = mReflectY ? -1.0f : 1.0f;
        Eigen::Affine2f a = Eigen::Affine2f::Identity();
        a.translate(Eigen::Vector2f(mOriginX, mOriginY));
        a.rotate(mRotationDeg * 0.017453292519943295f);
        a.scale(Eigen::Vector2f(mScaleX * rx, mScaleY * ry));
        linear() = a.linear();
        translation() = a.translation();
    }

    float mOriginX = 0.0f;
    float mOriginY = 0.0f;
    float mScaleX = 1.0f;
    float mScaleY = 1.0f;
    float mRotationDeg = 0.0f;
    bool mReflectX = false;
    bool mReflectY = false;
};

} // namespace live2d
