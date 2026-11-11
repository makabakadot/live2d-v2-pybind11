# live2d-v2-pybind11 — 当前版本快照（backup_2026_11_11）

本分支是 **Live2D Cubism 2.1（`.moc`）pybind11 绑定** 的完整快照，来自工作区
`G:\VB\Tools\离线渲染器\Lib\Live2d\v2`（2026-11-11）。

> 旧布局（`Dependence/` `Liv2dSource/` `PythonCode/` `V2_independent_Render/`）保留在 `main` 分支。

## 目录

```text
.
├─ CppLive2dReplace/          ★ 生成 v2.pyd 的绑定工程
│  ├─ CppLive2dReplace/       Binding.cpp / OpenGlRender.cpp / Render.cpp + Header/
│  └─ Sorce/                  Cubism 2.1 渲染核心改写副本（src / include）
├─ sdk/                       Cubism 2.1 SDK 源码
│  ├─ RelativeCode/           Json(nlohmann 3.12.0) / Glad / Common(stb_image)
│  ├─ OriginalCode/ Wrapped/ New/
├─ v2/                        另一份独立实现（V2_independent_Render / Liv2dSource / Dependence）
├─ v2.sln / v2.slnx           默认只编译 CppLive2dReplace
└─ LICENSE
```

## 构建

```bat
call "E:\progamme\VS\Base\VC\Auxiliary\Build\vcvars64.bat"
msbuild v2.sln /p:Configuration=Release /p:Platform=x64 /m
```

产物：`x64\Release\v2.pyd`（本机实测 **630,784 B**，Python 3.13 x64）。

依赖：`ThirdParty\pybind11`、`$(PythonRoot)\include|libs\python313.lib`。
**不需要** Live2D 官方 Core 库（2.1 的渲染全在源码里）。

## 当前依赖情况（2026-11-11 核查）

| 库 | 位置 | 版本 | 是否使用 |
| --- | --- | --- | --- |
| nlohmann/json | `sdk/RelativeCode/Json/nlohmann/json.hpp` | **3.12.0** | ✅ `LAppModel::loadModelFromString`（`LAppModel.cpp:666`）解析 model.json |
| Eigen | 工程外 `Lib/ThirdParty/eigen` | 3.5.0 | ❌ 未使用（源码无 `Eigen::`，IncludePath 也未包含） |

## 已知的优化点（尚未实施）

**解析**

- `LAppModel.cpp:42-58` 的 `parseTexturePaths()` 与 `:80-130` 的 `#else`（`OldLoader`）分支是**死代码**（宏未定义）
- `LAppModel.cpp:676` 用 `json::parse` + `try/catch`；可改 `parse(..., nullptr, false)` + `is_discarded()`
- `Render::setTexture(int, std::vector<uint8_t>)` **按值传**（`Binding.cpp:60`、`OpenGlRender.cpp:410`），每张纹理多一次整块拷贝
- `root["xxx"]` 在非 const 对象上是插入语义，宜改 `at()/find()`

**计算**

- 全手写 float 仿射（`AffineEnt`/`MatrixManager`），无 SIMD；Release 目前只有 `WholeProgramOptimization`，**没有 `/arch:AVX2`**
- 热点：`WarpDeformer.cpp` 网格插值、`ModelContext.cpp` 顶点变换、`RotationDeformer.cpp:80` 固定 10 次迭代求解

## 备注

- 本快照只含源码与工程文件，不含 `.pyd` / `.obj` 等构建产物
- 与 `main` 分支内容**结构不同**（main 是早期布局），合并时按目录对照搬运
