#pragma once
#include <memory>
#include "ModelImpl.hpp"

#include "OpenGlRender.hpp"

namespace live2d {

class ModelContext;

class ALive2DModel {
public:
    ALive2DModel();
    virtual ~ALive2DModel() = default;

    void setModelImpl(ModelImpl* impl) { mModelImpl = impl; }
    ModelImpl* getModelImpl();
    ModelContext* getModelContext() const { return mModelContext; }

    virtual std::shared_ptr<Render> getDrawParam() = 0;
    
    virtual void draw() = 0;

    int getCanvasWidth() const;
    int getCanvasHeight() const;
    float getParamFloat(int index) const;
    void setParamFloat(int index, float value, float weight = 1.0f);

protected:
    ModelImpl* mModelImpl = nullptr;
    ModelContext* mModelContext = nullptr;
};

} // namespace live2d
