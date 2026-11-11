#include "L2DModelMatrix.hpp"

namespace live2d {

L2DModelMatrix::L2DModelMatrix(float w, float h) : mWidth(w), mHeight(h) { identity(); }

void L2DModelMatrix::identity() {
    Eigen::Map<Eigen::Matrix4f>(mMat.data()).setIdentity();
}

void L2DModelMatrix::setWidth(float w) {
    float sx = w / mWidth;
    scale(sx, -sx);
}

void L2DModelMatrix::setHeight(float h) {
    float sx = h / mHeight;
    scale(sx, -sx);
}

void L2DModelMatrix::setCenterPosition(float x, float y) {
    translate(x - mWidth * getScaleX() * 0.5f, y - mHeight * getScaleY() * 0.5f);
}

void L2DModelMatrix::translate(float x, float y) {
    // 与 Python 版一致：绝对赋值
    Eigen::Map<Eigen::Matrix4f> m(mMat.data());
    m(0, 3) = x;
    m(1, 3) = y;
    m(2, 3) = 0.0f;
    m(3, 3) = 1.0f;
}

void L2DModelMatrix::scale(float sx, float sy) {
    // 与 Python 版一致：赋值而非相乘
    Eigen::Map<Eigen::Matrix4f> m(mMat.data());
    m.setZero();
    m(0, 0) = sx;
    m(1, 1) = sy;
    m(2, 2) = 1.0f;
    m(3, 3) = 1.0f;
}

} // namespace live2d
