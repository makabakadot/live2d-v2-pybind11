#include "L2DMatrix44.hpp"
#include <cstring>

namespace live2d {

L2DMatrix44::L2DMatrix44() { identity(); }

void L2DMatrix44::identity() { mat().setIdentity(); }

void L2DMatrix44::setMatrix(const float m[16]) {
    std::memcpy(mMatrix.data(), m, 16 * sizeof(float));
}

void L2DMatrix44::multTranslate(float x, float y, float z) {
    auto m = mat();
    m(0, 3) += x;
    m(1, 3) += y;
    m(2, 3) += z;
}

void L2DMatrix44::multScale(float sx, float sy, float sz) {
    auto m = mat();
    m(0, 0) *= sx;
    m(1, 1) *= sy;
    m(2, 2) *= sz;
}

L2DMatrix44 L2DMatrix44::mul(const L2DMatrix44& a, const L2DMatrix44& b) {
    L2DMatrix44 r;
    Eigen::Map<Eigen::Matrix4f>(r.mMatrix.data()) = a.mat() * b.mat();
    return r;
}

} // namespace live2d
