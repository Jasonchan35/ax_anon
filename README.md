# ax_anon – Game Engine Framework Prototype

### Objective
More for personal learning or experiment purpose I did during free time
- Cross-platform Render API: supported backend 
	- Windows - DirectX 11/12, Vulkan, OpenGL
	- MacOSX – MoltenVK, Metal, OpenGL
	- Linux X11 – Vulkan, OpenGL
- Shaders – HLSL for all platforms by compile to SPIRV for Vulkan, and SPIRV-cross to output Metal and GLSL
	- Contributed to SPIRV-cross, added `--rename-interface-variable` for Mac GLSL issue
https://github.com/KhronosGroup/SPIRV-Cross/issues/196
	- Compute Shader and output as vertex buffer

- Material – support multi-pass shader, constant buffer thread-safe update
- UI Framework – build from scratch base on RenderAPI
- Editor Interface Scene View, Node based Graph, Inspector, Object Outliner, Move Object Handle
- Undo/Redo
- Load / Save Scene file
- Node based Graph for Procedural Geometry
- Subdivision Surface with The Dual half-edge mesh data structure (source code: axEditableMesh)

-----
### Screenshots
[Click here for More screenshots](screenshots/screenshots.md)

**On Windows**
![](screenshots/2024-03-12%20Screenshot.png)


**On Linux X11**
![](screenshots/2024-03-12%20Linux%20X11.png)

