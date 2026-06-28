# 介绍
本项目来自Arkueid大佬[live2d-py](https://github.com/EasyLive2D/live2d-py/commits?author=Arkueid)项目的v2Cpp。与之不同的是采用Pybind11连接Python与C++而不是Cpython Api。并在源代码基础上修复了现版本可能存在的内存泄漏问题，以及将渲染独立出来方便重写。

Dependence中是默认C++ openGl的依赖，没有添加Pybind11的include，编译前记得加上
Live2dSource是v2Cpp自定义改造后的源码，Header和Cpp分离，方便导入，Pybind11绑定的代码是Binding.cpp
PythonCode是Python端测试的代码，自定义使用moderngl渲染
V2_independent_Render是vs项目，可参考其中依赖
# 1. 修改
## 1.1 GC
除部分类中创建的未回收外，原v2Cpp中似乎未对 ```BinaryRender.mObject``` 的项目进行回收，怀疑会导致内存泄漏。本项目在```L2DBaseModel```中添加了
```C++
std::shared_ptr<std::vector<void*>> mObjects;
std::shared_ptr< std::vector<std::function<void(void*)>>> mDeleters;
```
上面两者将会与```BinaryRender``` 共享，添加相应的Object以及对应的删除器，进而方便回收
原添加```void*``` 的主要时机在返回```void*```后，现改为之前
所有由```BinaryReader```创建的实例(```Id```的实例除外，其由```live2d::Id::releaseStored```函数调用释放)都统一由随模型释放，释放相关在```L2DBaseModel```析构函数中
```C++
//改动相关
class L2DBaseModel {
public:
	L2DBaseMode();
	virtual ~L2DBaseModel();
	loadModelData(const std::vector<uint8_t>& data, int version);
    std::shared_ptr<std::vector<void*>> mObjects;
    std::shared_ptr< std::vector<std::function<void(void*)>>> mDeleters;
};
L2DBaseModel::L2DBaseModel()
{
	//正常实例化相关
	...
    mObjects = std::make_shared<std::vector<void*>>();
    mDeleters = std::make_shared<std::vector<std::function<void(void*)>>>();
}
L2DBaseModel::~L2DBaseModel() {
    //正常释放相关代码
    ...
    if (mDeleters)//统一在模型结束后释放，除了Id*
    {
        for (size_t i = 0; i < mDeleters->size(); ++i)
        {
            if ((*mDeleters)[i])
            {
                (*mDeleters)[i]((*mObjects)[i]);
            }
        }
    }
}
void L2DBaseModel::loadModelData(const std::vector<uint8_t>& data, int version) {
    BinaryReader br(data);
    br.mObjects = mObjects;
    br.mDeleters = mDeleters;
    //正常加载相关代码
    ...
}
class BinaryReader {
public:
    std::shared_ptr<std::vector<void*>> mObjects;
    std::shared_ptr<std::vector<std::function<void(void*)>>> mDeleters;
};
class ISerializable {
public:
    int type;//为了RTTI
    static void ISerializableRelease(void* ptr);//释放函数,详见代码
};
//其他加入mObjects的也相应进行改动(包括头文件里的模版定义与实现)，每次添加相应的指针以及如：
void* BinaryReader::readKnownTypeObject(int type) {
    if (type == 0) {
        mObjects->push_back(nullptr);
        mDeleters->push_back(nullptr);
        return nullptr;
    }
    // String-to-Id types: 50, 51, 134, 60
    if (type == 50 || type == 51 || type == 134 || type == 60) {
        std::string idStr = readUTF8String();
        Id* idPtr = const_cast<Id*>(&Id::getID(idStr));
        mObjects->push_back(idPtr);
        mDeleters->push_back(nullptr);//由live2d::Id::releaseStored释放
        return idPtr;
    }
    if (type >= 48) {
        auto obj = Live2DObjectFactory::create(type);
        ISerializable* rawPtr = obj.get();
        rawPtr->type = type;
        obj.release(); // Transfer ownership — will be tracked in mObjects
        rawPtr->read(*this);
        mObjects->push_back(rawPtr);
        mDeleters->push_back(&live2d::ISerializable::ISerializableRelease);
        return rawPtr;
    }
    // Primitive types
    if (type == 1) {
        // String — return heap-allocated string
        std::string* s = new std::string(readUTF8String());
        mObjects->push_back(s);
        mDeleters->push_back([](void* ptr) { delete static_cast<std::string*>(ptr); });
        return s;
    }
    if (type == 15) {
        // Array of objects — read count then each element
        int count = readType();
        auto* arr = new std::vector<void*>();
        arr->reserve(count);
        for (int i = 0; i < count; i++) {
            arr->push_back(readObjectUntyped());
        }
        mObjects->push_back(arr);
        mDeleters->push_back([](void* ptr) { delete static_cast<std::vector<void*>*>(ptr); });
        return arr;
    }
    //其他修改过的释放代码
    ...
}
```
## 1.2 渲染
本项目将渲染与计算分离，使用```Render```类作为所有渲染类的父类，并在其中提供了```OpenGlRender```类作为默认渲染类,当不传入自定义的Render时默认使用。
其中```Render```类有部分中有部分需要重写的函数
```C++
class Render
	{
	public:
		//大部分需要重写的代码，来自DrawParamOpenGL
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
		
		//传入两个参数，一个是纹理的编号，一个是纹理的Raw图片数据(直接读取的图片数据，如PNG图片开头是PNG魔术头的那种)，读取可见Python示例
		virtual void setTexture(int no,std::vector<uint8_t> data)=0;
		
		//来自ClipManagerOpenGL的代码，大部分不需要重新，除了下面三个
		...
		//setupClip在渲染时需要切换FBO(OpenGL中),要么重写enterMaskRendering和 exitMaskRendering实现改变FBO的逻辑，要么重写setupClip,这样做会不再自动调用nterMaskRendering和exitMaskRendering
		//如果要重写setupClip则要传入ModelContext,懒得写绑定ModelContext的代码，以后再写
		//注意！setupClip没有开放重写给Python
		virtual void setupClip(ModelContext* mc);
		virtual void enterMaskRendering()=0;
		virtual void exitMaskRendering() = 0;
};

class OpenGlRender:public Render
{
	
public:
	//注意！在OpenGlRender的构造函数中调用了gladLoadGL而不是原先的glInit()
	OpenGlRender()
	//重写Render的虚函数
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
	void enterMaskRendering() override;
	void exitMaskRendering() override;
	//OpenGL的相关实现等
	...
};
//注意！为了防止在Python中调用C++导致的所有权问题。故在使用默认渲染器时无法获得实例，进而无法调用clearBuffer。因此单独写ClearBuffer全局函数出用于调用，若在Python端实现Render，请不要使用它(防止未定义的行为如程序直接崩溃)，使用自定义的clearBuffer
void ClearBuffer(float r, float g, float b, float a);
```
本代码中对大部分使用```DrawParamOpenGL```与```ClipManagerOpenGL```进行改名统一使用 render 命名，使用时直接对```LAppModel```类传参,传入```Render```子类。Pybind11会自动转化为```std::shared_ptr<Render>```进行传入，若无传参则使用默认```OpenGL```渲染器
```C++
//L2DBaseModel
L2DBaseModel::L2DBaseModel() {
    mLive2DModel = new Live2DModelRender();
    //其他初始化
    ...
    mObjects = std::make_shared<std::vector<void*>>();
    mDeleters = std::make_shared<std::vector<std::function<void(void*)>>>();
}
L2DBaseModel::L2DBaseModel(std::shared_ptr<Render> render)
{
    mLive2DModel = new Live2DModelRender(render);
    //其他初始化
    ...
    mObjects = std::make_shared<std::vector<void*>>();
    mDeleters = std::make_shared<std::vector<std::function<void(void*)>>>();
}
//Live2DModelRender
Live2DModelRender::Live2DModelRender()
{
    render = std::make_shared<OpenGlRender>();
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
```

# 2.在Visual studio中使用Pybind11绑定的live2dv2

改为生成动态库，Pybind11相关语法可见B站,请使用C++17(因为使用u8path处理中文字符)

需要设定包含目录有：

	1.Dependence\Common
	
	2.Dependence\pybind11\include
	
	3.Liv2dSource\include
	
	4.你编译目标版本下python的include(与python.exe在同一个文件夹位置)
	
需要设定的库目录有

	1.你编译目标版本下python的lib(与python.exe在同一个文件夹位置)
	
在连接器中的附加依赖项需要添加

	python3.lib
	
	python3XX.lib（如果是python3.11为python311.lib,python3.13为python313.lib）
	
需要将以下文件拖入项目

	1.Dependence\Common\glad.c
	
	2.Liv2dSource\src下的所有文件
	
然后生成就可以了
