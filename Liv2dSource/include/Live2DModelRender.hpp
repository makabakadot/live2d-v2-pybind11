#pragma once
#include <cstdint>
#include "ALive2DModel.hpp"


namespace live2d {
class Live2DModelRender : public ALive2DModel {
public:
    Live2DModelRender();
    Live2DModelRender(std::shared_ptr<Render> render);
    ~Live2DModelRender() override;

    std::shared_ptr<Render> getDrawParam() override { return render; }

    void draw() override;
    void resize(int w, int h);
    static Live2DModelRender* loadModel(const std::vector<uint8_t>& data);

    std::shared_ptr<Render> render = nullptr;

    
};
} // namespace live2d
