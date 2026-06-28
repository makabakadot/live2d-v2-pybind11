#pragma once
#include "Render.hpp"
#include <vector>
#include <array>
#include <GL/glew.h>
#include <cstdint>

namespace live2d
{	
	class OpenGlRender:public Render
	{
		
	public:
		OpenGlRender();
		~OpenGlRender();

		//DrawParamOpenGL
		void setupDraw() override;//初始化设定(会调用lazyInit)，如创建Shader，调整渲染设定，获取(创建Fbo)等
		void endDraw() override;//结束绘制
		void drawTexture(int texNo,
			const std::array<float, 4>& screenColor,
			const std::vector<int16_t>& indices,
			const std::vector<float>& vertices,
			const std::vector<float>& uvs,
			float opacity, int compositionType,
			const std::array<float, 4>& multiplyColor) override;
		void clearBuffer(float r, float g, float b, float a) override;
		void bindFramebuffer(int fb);

		void setTexture(int no, GLuint texData);
		void setTexture(int no, std::vector<uint8_t> texData) override;


		GLuint getTexture(int no);
		int createFramebuffer();
		GLuint createVBO(const std::vector<float>& data, int loc, int size);
		GLuint createEBO(const std::vector<int16_t>& data);
		void lazyInit();//初始化Shader
		
		//void enterMaskRendering() override;
		//void exitMaskRendering() override;

		//void setupClip(ModelContext* mc) override;
		void enterMaskRendering() override;
		void exitMaskRendering() override;

		GLuint mFramebuffer = 0;
		GLuint mFramebufferTexture = 0;
		GLuint mShaderNormal = 0, mShaderMask = 0;
		GLuint mPosVBO = 0, mUVVBO = 0, mEBO = 0;
		std::vector<GLuint> mTextures;
		GLuint mCurrentFBO = 0;
		GLint savedViewport[4];
	};
	void ClearBuffer(float r, float g, float b, float a);
}
