// Compiled SPIR-V Shaders for the CNFG Batched Vulkan renderer
// To recreate, modify "cnfg.glsl" and run "make vk/cnfgshader.h" (requires glslang)

// These shaders are in the tree so contributers don't need
// glslang to properly recreate rawdraw_sf.h

static const uint32_t CNFGVertShader[] = {
#include "vk/verttemp"
};

static const uint32_t CNFGBatchShader[] = {
#include "vk/batchtemp"
};

static const uint32_t CNFGImageShader[] = {
#include "vk/imgtemp"
};
