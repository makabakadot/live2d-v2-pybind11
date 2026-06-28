
import moderngl
import numpy as np
import v2
from dataclasses import dataclass 
from PIL import Image
from io import BytesIO
@dataclass 
class FrameBufferObject:
    #创建Mask渲染的fbo
    ctx:"moderngl.Context"
    Rbo:"moderngl.Renderbuffer"=None
    Framebuffer:"moderngl.Framebuffer"=None
    Texture:"moderngl.Sampler"=None
    def __post_init__(self):
        self.Create()
    def Create(self):
        
        size = 256
        rbo = self.ctx.renderbuffer(
            size=(size, size),
            components=4,
            dtype='f4'
        )
        texture = self.ctx.texture(
            size=(size, size),
            components=4,
            dtype="f1"
        )
        sampler = self.ctx.sampler(
            repeat_x=False,  # gl.texParameteri(aL.TEXTURE_2D, gl.TEXTURE_WRAP_S, aL.CLAMP_TO_EDGE)
            repeat_y=False,  # gl.texParameteri(aL.TEXTURE_2D, gl.TEXTURE_WRAP_T, aL.CLAMP_TO_EDGE)
            texture=texture,
            filter=(moderngl.LINEAR, moderngl.LINEAR)
            # aL.texParameteri(aL.TEXTURE_2D, aL.TEXTURE_MIN_FILTER, aL.LINEAR)
            # aL.texParameteri(aL.TEXTURE_2D, aL.TEXTURE_MAG_FILTER, aL.LINEAR)
        )
        fbo = self.ctx.framebuffer(
            color_attachments=[texture],
            # depth_attachment=rbo
        )
        self.Rbo = rbo
        self.Framebuffer = fbo
        self.Texture = sampler

class Render(v2.BaseRender):
    ShaderProgram_texture0=1
    ShaderProgramOff_texture0=1
    ShaderProgramOff_texture1=2
    def __init__(self):
        super().__init__()
        self.ctx=moderngl.create_context()
        self.shaderProgram = None
        self.shaderProgramOff =  None

        self.loadShadersInteral()
        self.MaskFramework = FrameBufferObject(self.ctx)

        self.fbo=None
        self.initialized = False
        self.backgroundBlending = None 
        self.textures:list[moderngl.Texture]=[]

        self.tempVeiwport=None
        self.vbo = None
        self.uv_vbo = None
        self.ibo = None
    def setupDraw(self):
        if not self.initialized:
            self.ctx.front_face = 'cw'  # 设置顺时针方向为正面
            self.ctx.scissor = None  # gl.disable(gl.SCISSOR_TEST)
            self.ctx.disable(self.ctx.DEPTH_TEST)  # gl.disable(gl.DEPTH_TEST)
            # 3. 启用混合并保存原有混合状态
            self.ctx.enable(moderngl.BLEND)

            # 4. 设置颜色掩码 (ModernGL 中默认就是 (True, True, True, True))
            self.fbo = self.ctx.detect_framebuffer()
            self.fbo.color_mask = (True, True, True, True)
            # self.backgroundBlending= self.ctx.blend_func
            self.initialized=True
    def setTexture(self, no, data:list[int]):#传入no:int 和 vector<char>(list[int])
        data=bytearray(data)
        size = len(self.textures)
        if no >= size:
            for i in range(size, no + 1):
                self.textures.append(None)
        img=Image.open(BytesIO(data)).convert('RGBA')
        self.textures[no] = self.ctx.texture(
            img.size,
            4,
            img.tobytes()
        )
        self.textures[no].filter=(
            self.ctx.LINEAR_MIPMAP_NEAREST,self.ctx.LINEAR
        )
        self.textures[no].build_mipmaps()
    def endDraw(self):
        return
    def drawTexture(self, texNo, screenColor, indexArray, vertexArray, uvArray, opacity, comp, multiplyColor):
        if opacity < 0.01 and not self.clipMaskContext:
            return

        r =  self.mBaseRed*opacity
        g =  self.mBaseGreen*opacity
        b =  self.mBaseBlue*opacity
        a =  self.mBaseAlpha*opacity
        # print("baseColor", a_w, a2, a5, a7)
        if self.clipMaskContext:
            # 根据蒙版进行裁剪
            self.ctx.front_face = 'ccw'
            self.bindOrCreateIBO(indexArray)
            self.bindOrCreateVBO(vertexArray)
            self.bindOrCreateUV_VBO(uvArray)
            texture = self.textures[texNo]
            texture.use(Render.ShaderProgram_texture0)
            vao = self.ctx.vertex_array(
                self.shaderProgram, [
                    (self.vbo, '2f', "a_position"),
                    (self.uv_vbo, '2f', 'a_texCoord')
                ],
                index_buffer=self.ibo,
                index_element_size=4
            )
            self.shaderProgram["u_mvpMatrix"] = self.mClipMatrix
            self.shaderProgram["u_channelFlag"] = tuple(1.0 if i == self.mClipChannel else 0.0 for i in range(4))
            self.shaderProgram["u_baseColor"] = (-1.0, -1.0, 1.0, 1.0)
            self.shaderProgram["u_maskFlag"] = True
            self.shaderProgram["u_screenColor"] = tuple(screenColor)
            self.shaderProgram["u_multiplyColor"] = tuple(multiplyColor)
        elif self.clipDrawContext:
            # 绘制蒙版
            self.bindOrCreateIBO(indexArray)
            self.bindOrCreateVBO(vertexArray)
            self.bindOrCreateUV_VBO(uvArray)
            texture = self.textures[texNo]
            texture.use(Render.ShaderProgramOff_texture0)

            vao = self.ctx.vertex_array(
                self.shaderProgramOff, [
                    (self.vbo, '2f', "a_position"),
                    (self.uv_vbo, '2f', 'a_texCoord')
                ],
                index_buffer=self.ibo,
                index_element_size=4
            )
            self.shaderProgramOff["u_clipMatrix"] = tuple(self.mClipMatrix)
            self.shaderProgramOff["u_mvpMatrix"] = tuple(self.mMatrix4x4)

            self.MaskFramework.Texture.use(Render.ShaderProgramOff_texture1)
            self.shaderProgramOff["u_channelFlag"] = tuple(1.0 if i == self.mClipChannel else 0.0 for i in range(4))
            self.shaderProgramOff["u_baseColor"] = (
                r, g, b, a
            )
            self.shaderProgramOff["u_screenColor"] = tuple(screenColor)
            self.shaderProgramOff["u_multiplyColor"] = tuple(multiplyColor)
        else:
            self.bindOrCreateIBO(indexArray)
            self.bindOrCreateVBO(vertexArray)
            self.bindOrCreateUV_VBO(uvArray)
            texture = self.textures[texNo]
            texture.use(Render.ShaderProgram_texture0)

            vao = self.ctx.vertex_array(
                self.shaderProgram, [
                    (self.vbo, '2f', "a_position"),
                    (self.uv_vbo, '2f', 'a_texCoord')
                ],
                index_buffer=self.ibo,
                index_element_size=4
            )
            self.shaderProgram["u_mvpMatrix"] = tuple(self.mMatrix4x4)
            self.shaderProgram["u_baseColor"] = r, g, b, a
            self.shaderProgram["u_maskFlag"] = False
            self.shaderProgram["u_screenColor"] = screenColor
            self.shaderProgram["u_multiplyColor"] = multiplyColor
        if self.mCulling:
            self.ctx.enable(self.ctx.CULL_FACE)
        else:
            self.ctx.disable(self.ctx.CULL_FACE)

        self.ctx.enable(self.ctx.BLEND)

        self.ctx.blend_equation = self.ctx.FUNC_ADD
        if self.clipMaskContext or (comp == 0):
            self.ctx.blend_func = (
                self.ctx.ONE, self.ctx.ONE_MINUS_SRC_ALPHA,
                self.ctx.ONE, self.ctx.ONE_MINUS_SRC_ALPHA
            )
        else:
            if comp == 1:
                self.ctx.blend_func = (
                    self.ctx.ONE, self.ctx.ONE,
                    self.ctx.ZERO, self.ctx.ONE
                )
            elif comp == 2:
                self.ctx.blend_func = (
                    self.ctx.DST_COLOR, self.ctx.ONE_MINUS_SRC_ALPHA,
                    self.ctx.ZERO, self.ctx.ONE
                )
            else:
                raise RuntimeError("unknown comp")

        vao.render(moderngl.TRIANGLES)
    def loadShadersInteral(self):
        vert = """#version 330 core
                 in vec2 a_position;
                 in vec2 a_texCoord;
                 out vec2 v_texCoord;
                 out vec4 v_clipPos;
                 uniform mat4 u_mvpMatrix;
                 void main(){
                     gl_Position = u_mvpMatrix * vec4(a_position, 0.0, 1.0);
                     v_clipPos = gl_Position;
                     v_texCoord = a_texCoord;
                     v_texCoord.y = 1.0 - v_texCoord.y;
                }
              """
        frag = """#version 330 core
                in vec2       v_texCoord;
                in vec4       v_clipPos;
                uniform sampler2D  s_texture0;
                uniform vec4       u_channelFlag;
                uniform vec4       u_baseColor;
                uniform bool       u_maskFlag;
                uniform vec4       u_screenColor;
                uniform vec4       u_multiplyColor;
                out vec4 FragColor;
                void main(){
                    vec4 smpColor;
                    if(u_maskFlag){
                    float isInside = 
                    step(u_baseColor.x, v_clipPos.x/v_clipPos.w)
                    * step(u_baseColor.y, v_clipPos.y/v_clipPos.w)
                    * step(v_clipPos.x/v_clipPos.w, u_baseColor.z)
                    * step(v_clipPos.y/v_clipPos.w, u_baseColor.w);
                    smpColor = u_channelFlag * texture(s_texture0, v_texCoord).a * isInside;
                    }else{
                    smpColor = texture(s_texture0 , v_texCoord);
                    smpColor.rgb = smpColor.rgb * smpColor.a;
                    smpColor.rgb = smpColor.rgb * u_multiplyColor.rgb;
                    smpColor.rgb = smpColor.rgb + u_screenColor.rgb - (smpColor.rgb * u_screenColor.rgb);
                    smpColor = smpColor * u_baseColor;
                    }
                FragColor = smpColor;}"""
        vertOff = """#version 330 core
              in vec2     a_position;
              in vec2     a_texCoord;
              out vec2       v_texCoord;
              out vec4       v_clipPos;
              uniform mat4       u_mvpMatrix;
              uniform mat4       u_clipMatrix;
              void main(){
                vec4 pos = vec4(a_position, 0, 1.0);
                gl_Position = u_mvpMatrix * pos;
                v_clipPos = u_clipMatrix * pos;
                v_texCoord = a_texCoord;
                v_texCoord.y = 1.0 - v_texCoord.y;
              }"""
        fragOff = """#version 330 core
                    in   vec2   v_texCoord;
                    in   vec4   v_clipPos;
                    uniform sampler2D  s_texture0;
                    uniform sampler2D  s_texture1;
                    uniform vec4       u_channelFlag;
                    uniform vec4       u_baseColor;
                    uniform vec4       u_screenColor;
                    uniform vec4       u_multiplyColor;
                    out vec4 FragColor;
                    void main(){
                        vec4 col_formask = texture(s_texture0, v_texCoord);
                        col_formask.rgb = col_formask.rgb * col_formask.a;                
                        col_formask.rgb = col_formask.rgb * u_multiplyColor.rgb; 
                        col_formask.rgb = col_formask.rgb + u_screenColor.rgb - (col_formask.rgb * u_screenColor.rgb);
                        col_formask = col_formask * u_baseColor;  
                        vec4 clipMask = texture(s_texture1, v_clipPos.xy / v_clipPos.w) * u_channelFlag;
                        float maskVal = clipMask.r + clipMask.g + clipMask.b + clipMask.a;
                        col_formask = col_formask * maskVal;
                        FragColor = col_formask;}"""
        self.shaderProgram = self.ctx.program(vert, frag)
        self.shaderProgramOff = self.ctx.program(vertOff, fragOff)

        self.shaderProgram["s_texture0"] = Render.ShaderProgram_texture0
        self.shaderProgramOff["s_texture0"] = Render.ShaderProgramOff_texture0
        self.shaderProgramOff["s_texture1"] = Render.ShaderProgramOff_texture1
        return True
    def enterMaskRendering(self):
        self.tempVeiwport=self.ctx.viewport
        self.ctx.viewport=(0,0,256,256)
        self.MaskFramework.Framebuffer.use()
        self.ctx.clear(0,0,0,0)
    def exitMaskRendering(self):
        self.fbo.use()#切换回屏幕fbo
        self.ctx.viewport=self.tempVeiwport
    def clearBuffer(self):
        self.ctx.clear(1.0,1.0,1.0,1.0)
    #创建buffer的函数
    def bindOrCreateVBO(self, data: list):
        b = np.array(data, dtype='f4').tobytes()
        if self.vbo is None:
            self.vbo = self.ctx.buffer(
                data=b,
                dynamic=True
            )
            return
        else:
            offset = self.vbo.size - len(b)  # 计算差值
            if offset >= 0:  # 说明有空位，可以直接填充
                self.vbo.write(
                    data=b,
                    offset=0
                )
                if offset != 0:
                    self.vbo.write(
                        data=bytes(offset),#填充为0的byte
                        offset=len(b)
                    )
            else:  # 释放内存,重新分配
                self.vbo.release()
                self.vbo = self.ctx.buffer(
                    data=b,
                    dynamic=True
                )
                return
    def bindOrCreateUV_VBO(self, data: list):
        b = np.array(data, dtype='f4').tobytes()
        if self.uv_vbo is None:
            self.uv_vbo = self.ctx.buffer(
                data=b,
                dynamic=True
            )
            return
        else:
            offset = self.uv_vbo.size - len(b)  # 计算差值
            if offset >= 0:  # 说明有空位，可以直接填充
                self.uv_vbo.write(
                    data=b,
                    offset=0
                )
                if offset != 0:
                    self.uv_vbo.write(
                        data=bytes(offset),
                        offset=len(b)
                    )
            else:  # 释放内存,重新分配
                self.uv_vbo.release()
                self.uv_vbo = self.ctx.buffer(
                    data=b,
                    dynamic=True
                )
                return
    def bindOrCreateIBO(self, data: list):
        b = np.array(data, dtype='i4').tobytes()
        if self.ibo is None:
            self.ibo = self.ctx.buffer(
                data=b,
                dynamic=True
            )
            return
        else:
            offset = self.ibo.size - len(b)  # 计算差值
            if offset >= 0:  # 说明有空位，可以直接填充
                self.ibo.write(
                    data=b,
                    offset=0
                )
                if offset != 0:  # 用0填充
                    self.ibo.write(
                        data=bytes(offset),
                        offset=len(b)
                    )
            else:  # 释放内存,重新分配
                self.ibo.release()
                self.ibo = self.ctx.buffer(
                    data=b,
                    dynamic=True
                )
                return