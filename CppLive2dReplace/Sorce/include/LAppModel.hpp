#pragma once
#include "L2DBaseModel.hpp"
#include "L2DTargetPoint.hpp"
#include "MatrixManager.hpp"
#include "Live2DModelRender.hpp"
#include <nlohmann/json.hpp>
#include <functional>
#include <string>
#include <Render.hpp>
#include <iostream>
namespace live2d {
class LAppModel : public L2DBaseModel {
public:
    using StartCallback = std::function<void(const std::string&, int)>;
    using FinishCallback = std::function<void(const std::string&, int)>;

    LAppModel();
    LAppModel(std::shared_ptr<Render> render);
    ~LAppModel() override;
    bool loadModelJson(const std::string& path);
    bool loadModelFromString(const std::string& json,const std::string& baseDir);
    void resize(int w, int h);
    void drag(float x, float y);
    void touch(float x, float y);
    bool isMotionFinished() const;
    void setOffset(float dx, float dy);
    void setScale(float s);
    void setParameterValue(const std::string& id, float val, float weight = 1.0f);
    void addParameterValue(const std::string& id, float val, float weight = 1.0f);
    void setAutoBreathEnable(bool v) { mAutoBreath = v; }
    void setAutoBlinkEnable(bool v) { mAutoBlink = v; }
    int getParameterCount() const;
    int getPartCount() const;
    std::string getPartId(int index) const;
    void setPartOpacity(int index, float val);
    void update();
    void draw();
    bool hitTest(const std::string& area, float x, float y);
    void setExpression(const std::string& name);
    void setRandomExpression();
    void startMotion(const std::string& group, int no, int priority,
                     StartCallback onStart = nullptr, FinishCallback onFinish = nullptr);
    void startRandomMotion(const std::string& group, int priority,
                           StartCallback onStart = nullptr, FinishCallback onFinish = nullptr);
    std::unordered_map<std::string, float> GetMotionState();
    void clearMotions();
    void stopAllMotions() { clearMotions(); }
    void ClearMotionImmediately();
    void resetExpression();
    void resetPose();
    float getCanvasWidth() const { return (float)mLive2DModel->getCanvasWidth(); }
    float getCanvasHeight() const { return (float)mLive2DModel->getCanvasHeight(); }
    float getPixelsPerUnit() const {
        float w = static_cast<float>(mMatrixManager.getWidth()) / static_cast<float>(this->getCanvasWidth());
        float h = static_cast<float>(mMatrixManager.getHeight()) / static_cast<float>(this->getCanvasHeight());
        return w < h ? w : h;
    }
    void rotate(float deg);
    float getParameterValue(int index) const;
    float getParameterMin(int index) const;
    float getParameterMax(int index) const;
    float getParameterDefault(int index) const;
    std::string getParameterId(int index) const;
    void setPartScreenColor(int index, float r, float g, float b, float a);
    void setPartMultiplyColor(int index, float r, float g, float b, float a);
    std::vector<float> getPartScreenColor(int index) const;
    std::vector<float> getPartMultiplyColor(int index) const;
    std::vector<std::string> hitPart(float x, float y, bool topOnly);
    void setTexture(int no, int texId);
    L2DTargetPoint mDragMgr;
    MatrixManager mMatrixManager;
    bool mAutoBreath = true, mAutoBlink = true;
    bool mClearFlag = false;
    std::string mModelHomeDir;
private:
    StartCallback mOnStartMotion;
    FinishCallback mOnFinishMotion;
    bool mCallbacksPending = false;
    std::string mCurrentGroup; int mCurrentNo = 0;
};
} // namespace live2d
