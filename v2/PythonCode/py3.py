
import math
import os.path
import time

import glfw
import live2d.v3 as live2d
# import live2d.v2 as live2d
# import live2d.v2cpp as live2d

if live2d.LIVE2D_VERSION == 3:
    from live2d.v3 import StandardParams
else:
    from live2d.v2 import StandardParams
from live2d.utils import log
from live2d.utils.lipsync import WavHandler


def main():
    if not glfw.init():
        raise RuntimeError("glfw.init failed")

    display = (500, 600)
    window = glfw.create_window(*display, "glfw", None, None)
    if not window:
        glfw.terminate()
        return
    glfw.make_context_current(window)

    live2d.init()
    live2d.glInit()

    model = live2d.LAppModel()
    if live2d.LIVE2D_VERSION == 3:
        model.LoadModelJson(r"D:\Code\python_code\Live2d\Resources\v3\Mao\Mao.model3.json")
    

    model.Resize(*display)

    

    glfw.swap_interval(1)
    model.StartMotion("TapBody_0", 0, live2d.MotionPriority.FORCE,
                              None, None)
    while not glfw.window_should_close(window):
       

        model.Update()

        
        


        live2d.clearBuffer(1.0, 0.0, 0.0, 0.0)
        model.Draw()
        glfw.swap_buffers(window)

    live2d.dispose()

    # if crashes when exiting, try to explicitly destroy the renderer before terminating glfw
    # this is a workaround for a known issue in some environments where the OpenGL context is not properly released
    model.DestroyRenderer()
    glfw.terminate()


if __name__ == "__main__":
    main()