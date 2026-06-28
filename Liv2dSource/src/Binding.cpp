#include <LAppModel.hpp>
#include <pybind11/pybind11.h>
#include <GL/glew.h>
//#include <DrawParamOpenGL.hpp>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "Id.hpp"
class PyRender :public live2d::Render
{
public:
	void setupDraw() override
	{
		PYBIND11_OVERRIDE_PURE(
			void, // 返回值类型
			Render,      // 父类
			setupDraw         // C++函数名（需与Python方法名匹配）
		);
	}
	void  endDraw() override
	{
		PYBIND11_OVERRIDE_PURE(
			void, // 返回值类型
			Render,      // 父类
			endDraw         // C++函数名（需与Python方法名匹配）
		);
	}
	void drawTexture(int texNo,
		const std::array<float, 4>& screenColor,
		const std::vector<int16_t>& indices,
		const std::vector<float>& vertices,
		const std::vector<float>& uvs,
		float opacity, int compositionType,
		const std::array<float, 4>& multiplyColor) override
	{
		PYBIND11_OVERRIDE_PURE(
			void, // 返回值类型
			Render,      // 父类
			drawTexture,
			texNo,
			screenColor,
			indices,
			vertices,
			uvs,
			opacity,
			compositionType,
			multiplyColor
		);
	}
	void clearBuffer(float r, float g, float b, float a) override
	{

		PYBIND11_OVERRIDE_PURE(
			void, // 返回值类型
			Render,      // 父类
			clearBuffer,
			r, g, b, a

		);
	}
	void setTexture(int no, std::vector<uint8_t> data) override
	{
		PYBIND11_OVERRIDE_PURE(
			void, // 返回值类型
			Render,      // 父类
			setTexture,
			no, data

		);
	}

	//void setupClip(live2d::ModelContext* mc) override{
	//	PYBIND11_OVERRIDE(
	//		void, // 返回值类型
	//		Render,      // 基类
	//		setupClip,       // 函数名
	//		mc
	//		);
	//};
	void enterMaskRendering() override
	{
		PYBIND11_OVERRIDE_PURE(
			void, // 返回值类型
			Render,      // 父类
			enterMaskRendering,
			);
	}
	void exitMaskRendering() override
	{
		PYBIND11_OVERRIDE_PURE(
			void, // 返回值类型
			Render,      // 父类
			exitMaskRendering,
			);
	}
};
PYBIND11_MODULE(v2,m)
{


	m.def("ClearIds", &live2d::Id::releaseStored);
	pybind11::class_<live2d::Render, PyRender, std::shared_ptr<live2d::Render>>(m, "BaseRender")
		.def(pybind11::init<>())
		.def("setupDraw", &live2d::Render::setupDraw)
		.def("endDraw", &live2d::Render::endDraw)
		.def("drawTexture", &live2d::Render::drawTexture)
		.def("clearBuffer", &live2d::Render::clearBuffer)
		.def("setTexture", &live2d::Render::setTexture)
		.def("enterMaskRendering", &live2d::Render::enterMaskRendering)
		.def("exitMaskRendering", &live2d::Render::exitMaskRendering)
		.def_readonly_static("COLOR_COMPOSITION_NORMAL", &live2d::Render::COLOR_COMPOSITION_NORMAL)
		.def_readonly_static("COLOR_COMPOSITION_SCREEN", &live2d::Render::COLOR_COMPOSITION_SCREEN)
		.def_readonly_static("COLOR_COMPOSITION_MULTIPLY", &live2d::Render::COLOR_COMPOSITION_MULTIPLY)
		.def_property_readonly("mCulling", [](const live2d::Render& self) { return self.mCulling; })
		.def_property_readonly("mBaseRed", [](const live2d::Render& self) { return self.mBaseRed; })
		.def_property_readonly("mBaseGreen", [](const live2d::Render& self) { return self.mBaseGreen; })
		.def_property_readonly("mBaseBlue", [](const live2d::Render& self) { return self.mBaseBlue; })
		.def_property_readonly("mBaseAlpha", [](const live2d::Render& self) { return self.mBaseAlpha; })
		.def_property_readonly("mClipMatrix", [](const live2d::Render& self) { return self.mClipMatrix; })
		.def_property_readonly("mMatrix4x4", [](const live2d::Render& self) { return self.mMatrix4x4; })
		.def_property_readonly("mClipChannel", [](const live2d::Render& self) { 
				return self.mClipChannel;
			})
		.def_property_readonly("clipMaskContext", [](const live2d::Render& self) { return self.mClipMaskCtx?true:false; })
		.def_property_readonly("clipDrawContext", [](const live2d::Render& self) { return self.mClipDrawCtx ? true : false; });
	pybind11::class_<live2d::LAppModel>(m, "LAppModel")
		.def(pybind11::init<>())
		.def(pybind11::init<std::shared_ptr<live2d::Render>>())
		.def("Resize", &live2d::LAppModel::resize)
		.def("Update",&live2d::LAppModel::update)
		.def("Draw", &live2d::LAppModel::draw)
		.def("LoadModelJson",&live2d::LAppModel::loadModelJson)
		.def("StartMotion", &live2d::LAppModel::startMotion)
		.def("MotionNames", [](const live2d::LAppModel& self)
			{
				std::vector<std::string> keys;
				keys.reserve(self.mMotions.size());
				for (const auto& pair : self.mMotions)
				{
					keys.push_back(pair.first);
				}
				return keys;
			})
		.def("ExpressionNames", [](const live2d::LAppModel& self)
			{
				std::vector<std::string> keys;
				keys.reserve(self.mExpressions.size());
				for (const auto& pair : self.mExpressions)
				{
					keys.push_back(pair.first);
				}
				return keys;
			});
	m.def("ClearBuffer", [](float a, float b, float c, float d)
		{
			live2d::ClearBuffer(a,b,c,d);

		});
}





