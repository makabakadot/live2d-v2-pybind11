#pragma once

#include <array>

#include <Eigen/Core>

namespace live2d {

// 4x4 变换矩阵：内部用 Eigen 计算，存储保持 OpenGL 兼容的列主序 float[16]。
class L2DMatrix44 {
public:
    L2DMatrix44();

    void identity();
    void setMatrix(const float m[16]);
    void multTranslate(float x, float y, float z);
    void multScale(float sx, float sy, float sz);
    std::array<float, 16>& getArray() { return mMatrix; }
    const std::array<float, 16>& getArray() const { return mMatrix; }

    static L2DMatrix44 mul(const L2DMatrix44& a, const L2DMatrix44& b);

private:
    Eigen::Map<Eigen::Matrix4f> mat() { return Eigen::Map<Eigen::Matrix4f>(mMatrix.data()); }
    Eigen::Map<const Eigen::Matrix4f> mat() const {
        return Eigen::Map<const Eigen::Matrix4f>(mMatrix.data());
    }

    std::array<float, 16> mMatrix;
};

} // namespace live2d
