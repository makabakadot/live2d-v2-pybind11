#pragma once
#include <ClipContext.hpp>
#include <stb_image.h>
#include <GL/glew.h>
#include <memory>
#ifdef LOGGING
#include<iostream>
#endif
namespace live2d
{
	class ModelContext;
	class DrawParamOpenGL;
	class IDrawData;
	class MeshContext;
	class ClipContext;
	class ALive2DModel;
	class Render
	{
	public:
		static constexpr int CHANNEL_COUNT = 4;
		static constexpr int CLIP_MASK_SIZE = 256;
		static constexpr int NORMAL_SHADER = 0;
		static constexpr int MASK_SHADER = 1;
		static constexpr int COLOR_COMPOSITION_NORMAL = 0;
		static constexpr int COLOR_COMPOSITION_SCREEN = 1;
		static constexpr int COLOR_COMPOSITION_MULTIPLY = 2;

		Render();
		virtual ~Render();
		//DrawParamOpenGL
		virtual void setupDraw()=0;//初始化设定(会调用lazyInit)，如创建Shader，调整渲染设定，获取(创建Fbo)等
		virtual void endDraw()=0;//结束绘制
		virtual void drawTexture(int texNo,
			const std::array<float, 4>& screenColor,
			const std::vector<int16_t>& indices,
			const std::vector<float>& vertices,
			const std::vector<float>& uvs,
			float opacity, int compositionType,
			const std::array<float, 4>& multiplyColor)=0;
		virtual void clearBuffer(float r, float g, float b, float a)=0;
		virtual void setTexture(int no,std::vector<uint8_t> data)=0;

		//Property
		void setMatrix(const float m[16]);
		void setClipMatrix(const float m[16]);
		void setClipBufPre_clipContextForDraw(void* ctx) { mClipDrawCtx = ctx; }
		void setClipBufPre_clipContextForMask(void* ctx) { mClipMaskCtx = ctx; }
		void setCulling(bool b);

		//ClippingManagerOpenGL
		static live2d::ClipContext* findSameClip(
			const std::vector<ClipContext*>& list,
			const std::vector<std::string>& ids
		);
		void init(ModelContext* mc,
			const std::vector<IDrawData*>& drawDataList,
			const std::vector<MeshContext*>& drawContextList);
		void  calcClippedDrawTotalBounds(ModelContext* mc, ClipContext* clip);
		void  setupLayoutBounds(int count);
		static void buildClipMatrix(std::array<float, 16>& out, ClipContext* clip, bool forMask);

		//setupClip在渲染时切换FBO,要么重写enterMaskRendering和 exitMaskRendering实现改变FBO的逻辑，要么重写setupClip,并顺便覆盖上面两个函数
		//如果要重写setupClip则要传入ModelContext,懒了，以后再写
		virtual void setupClip(ModelContext* mc);
		virtual void enterMaskRendering()=0;
		virtual void exitMaskRendering() = 0;
		


		std::array<float, 16> mMatrix4x4{};
		std::array<float, 16> mClipMatrix{};
		void* mClipMaskCtx = nullptr;   
		void* mClipDrawCtx = nullptr; 
		int mClipChannel = 0;
		std::vector<ClipContext*> mClipContextList;
		std::vector<std::array<float, 4>> mChannelColors;
		bool mCulling = true;
		bool mInited = false;
		float mBaseRed = 1.0f, mBaseGreen = 1.0f, mBaseBlue = 1.0f, mBaseAlpha = 1.0f;
	};
}
