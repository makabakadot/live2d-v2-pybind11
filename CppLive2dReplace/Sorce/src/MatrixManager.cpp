#include "MatrixManager.hpp"
#include "L2DModelMatrix.hpp"

#include <Eigen/Core>
#include <Eigen/Geometry>

namespace live2d {

MatrixManager::MatrixManager() = default;

void MatrixManager::onResize(int w, int h) { mWidth = w; mHeight = h; }
void MatrixManager::setScale(float s) { mScale = s; }
void MatrixManager::setOffset(float dx, float dy) { mOffsetX = dx; mOffsetY = dy; }
void MatrixManager::rotate(float deg) { mRotation = deg; }

std::array<float, 16> MatrixManager::getMvp(L2DModelMatrix* modelMatrix) const {
    const float canvasW = modelMatrix->mWidth > 0 ? modelMatrix->mWidth : 1.0f;
    const float canvasH = modelMatrix->mHeight > 0 ? modelMatrix->mHeight : 1.0f;

    // 1) 模型矩阵：setWidth(2.0) + setCenterPosition(0,0)
    const float sxM = 2.0f / canvasW;
    const float syM = -sxM;

    Eigen::Matrix4f mm = Eigen::Matrix4f::Identity();
    mm(0, 0) = sxM * modelMatrix->getScale();
    mm(1, 1) = syM * modelMatrix->getScale();
    mm(0, 3) = -(canvasW * mm(0, 0)) * 0.5f;
    mm(1, 3) = -(canvasH * mm(1, 1)) * 0.5f;

    // 2) 投影
    Eigen::Matrix4f proj = Eigen::Matrix4f::Identity();
    if (mHeight > mWidth) {
        proj(0, 0) = 1.0f;
        proj(1, 1) = static_cast<float>(mWidth) / static_cast<float>(mHeight);
    } else {
        proj(0, 0) = static_cast<float>(mHeight) / static_cast<float>(mWidth);
        proj(1, 1) = 1.0f;
    }
    proj(0, 0) *= mScale;
    proj(1, 1) *= mScale;
    proj(0, 3) = mOffsetX;
    proj(1, 3) = mOffsetY;

    // 3) proj = proj * mm
    proj = proj * mm;

    // 4) 用户旋转：proj = rot * proj
    if (mRotation != 0.0f) {
        Eigen::Matrix4f rot = Eigen::Matrix4f::Identity();
        const float r = mRotation * 0.017453292519943295f;
        rot(0, 0) = std::cos(r);
        rot(0, 1) = -std::sin(r);
        rot(1, 0) = std::sin(r);
        rot(1, 1) = std::cos(r);
        proj = rot * proj;
    }

    std::array<float, 16> result;
    Eigen::Map<Eigen::Matrix4f>(result.data()) = proj;
    return result;
}

} // namespace live2d
