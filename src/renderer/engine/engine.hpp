//
// Created by otgaard on 2018/10/13.
//

#pragma once

// The renderer engine code is really the data side of the API dependent code hidden behind the renderer.  Thus buffers
// are created with these classes and bound by an API-specific renderer to the relevant API (e.g. OpenGL, ES, Direct3D)
// or for use in the software rasteriser or ray tracer.  The goal is to create an API-independent representation of all
// resources related to the rendering process, such as pixel buffers, framebuffers, vertex/index buffers, meshes,
// textures, samplers, uniforms, etc.  The implementation will be ported to (in order):
//
// 1) OpenGL 3.3 Core or later
// 2) OpenGL 2.0 ES for Raspberry Pi
// 3) DirectX 12
// 4) Vulkan
//
// In addition, the data classes will be used in the software rasteriser and the ray tracer, as well as the scene graph
// in the base renderer.

namespace zap { namespace renderer { namespace engine {

}}}
