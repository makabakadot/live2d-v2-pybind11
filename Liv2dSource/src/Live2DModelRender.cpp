#include "Live2DModelRender.hpp"
#include "ModelContext.hpp"

namespace live2d {


Live2DModelRender::Live2DModelRender()
{
    render = std::make_shared<OpenGlRender>();
}
Live2DModelRender::~Live2DModelRender()
{
    //delete mDrawParam;
}
void Live2DModelRender::draw()
{
    mModelContext->draw(render);
    }

Live2DModelRender::Live2DModelRender(std::shared_ptr<Render> render)
{
    if (render)
    {
        this->render = render;
    }
    else
    {
        this->render = std::make_shared < OpenGlRender>();
    }
        
}

void Live2DModelRender::resize(int w, int h) { (void)w; (void)h; }
Live2DModelRender* Live2DModelRender::loadModel(const std::vector<uint8_t>& data) {
    (void)data; return new Live2DModelRender();
}
} // namespace live2d
