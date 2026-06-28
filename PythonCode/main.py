import os.path
import OpenGL.GL as gl
import glfw

from v2 import LAppModel,ClearBuffer,BaseRender
from RenderClass import Render
# from v2 import glInit,LAppModel,clearBuffer
def init_window(width, height, title):
    if not glfw.init():
        return None

    window = glfw.create_window(width, height, title, None, None)
    if not window:
        glfw.terminate()
        return None

    glfw.make_context_current(window)

    return window

# 渲染循环
def main():
    
    window = init_window(400, 500, "glfw")
    if not window:
        print("Failed to create GLFW window")
        return

    render=Render()
    model = LAppModel(render)
    # model = LAppModel()#默认用

    #测试文件路径
    model.LoadModelJson(r"D:\Code\python_code\Live2d\Resources\v2\kasumi2\kasumi2.model.json")

    model.StartMotion("right01", 3, 3,None,None)

    glfw.swap_interval(0)
    print(model.MotionNames())
    model.Resize(400, 500)
    while not glfw.window_should_close(window):
        glfw.poll_events()

        # ClearBuffer(1.0,1.0,1.0,1.0)#默认用
        render.clearBuffer()
        model.Update()
        model.Draw()

        glfw.swap_buffers(window)


    glfw.terminate()
    del model
    print("看到此行话表示回收中没有发生崩溃")


if __name__ == "__main__":
    main()
