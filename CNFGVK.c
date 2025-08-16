//Copyright (c) 2010-2025 <>< Charles Lohr, and several others!
//	Licensed under the MIT/x11 or NewBSD License you choose.
//
// Vulkan Batched Renderer

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "vk/cnfgshader.h"

#ifndef CNFGVK
#define CNFGVK
#define CNFG_BATCH 8192
#define CNFG_SURFACE_EXTENSION ""
#define CNFGVK_VALIDATION_LAYERS

#include "CNFG.h"
#endif

#ifndef CNFGVK_FRAMES_IN_FLIGHT
#define CNFGVK_FRAMES_IN_FLIGHT 3
#endif

#ifndef CNFGVK_IMAGE_ALLOCATION
// Enough memory for a 4k framebuffer. As this is a huge upper limit, it can be redefined
#define CNFGVK_IMAGE_ALLOCATION 32*1024*1024
#endif

#define CNFGVK_BATCH_SIZE ( sizeof( CNFGVertDataC ) + sizeof( CNFGVertDataV ) + sizeof( float[4] ) )
#define CNFG_LOAD_FUNC( var, name ) PFN_##name var = (PFN_##name)vkGetInstanceProcAddr( CNFGVkInstance, #name )

static VkInstance CNFGVkInstance = NULL;
static VkPhysicalDevice CNFGPhysicalDevice = NULL;
static VkDevice CNFGLogicalDevice = NULL;
static VkQueue CNFGGraphicsQueue = NULL;
static VkQueue CNFGPresentQueue = NULL;
static VkSurfaceKHR CNFGVkSurface = NULL;

static VkSwapchainKHR CNFGSwapchain = NULL;
static VkExtent2D CNFGSwapchainExtent = { 0 };
static VkImage* CNFGSwapchainImages = NULL;
static VkImageView* CNFGSwapchainViews = NULL;
static uint32_t CNFGSwapchainImageCount = 0;
static VkFormat CNFGSwapchainFormat = 0;

static VkPipelineLayout CNFGPipelineLayout = NULL;
static VkPipeline CNFGBatchPipeline = NULL;
static VkPipeline CNFGImagePipeline = NULL;
static VkCommandPool CNFGGraphicsPool = NULL;
static VkCommandBuffer CNFGGraphicsCommands[CNFGVK_FRAMES_IN_FLIGHT] = { NULL };

static VkSemaphore* CNFGPresentSemas = NULL;
static VkSemaphore* CNFGRenderSemas = NULL;
static VkFence CNFGRenderFences[CNFGVK_FRAMES_IN_FLIGHT] = { NULL };

static VkDescriptorSetLayout CNFGDescriptorSetLayout = NULL;
static VkDescriptorPool CNFGDescriptorPool = NULL;
static VkDescriptorSet CNFGDescriptorSet = NULL;

static VkBuffer CNFGBatchBuffer = NULL;
static VkCommandBuffer CNFGBatchCommand = NULL;
static VkFence CNFGBatchFence = NULL;
static VkImage CNFGImage = NULL;
static VkImageView CNFGImageView = NULL;
static VkDeviceMemory CNFGBatchMemory = NULL;
static void* CNFGBatchMap = NULL;

static VkDeviceMemory CNFGImageMemory = NULL;
static void* CNFGImageMap = NULL;
static VkSampler CNFGSampler = NULL;

static uint32_t CNFGCurSwapchainImage = -1;
static int CNFGCurFlight = 0;
static int CNFGCurSema = 0;
static int CNFGScissors[4] = { -1 };
static int CNFGVSyncOn = 1;

static const char * CNFGInstanceExtensions[] = {
#ifdef CNFGVK_VALIDATION_LAYERS
	"VK_EXT_debug_utils",
#endif
	CNFG_SURFACE_EXTENSION
};
#define CNFG_NUM_INSTANCE_EXT sizeof(CNFGInstanceExtensions)/sizeof(*CNFGInstanceExtensions)

static const char * CNFGDeviceExtensions[] = { "VK_KHR_swapchain" };
#define CNFG_NUM_DEVICE_EXT sizeof(CNFGDeviceExtensions)/sizeof(*CNFGDeviceExtensions)

#ifdef CNFGVK_VALIDATION_LAYERS
static const char * CNFGInstanceLayers[] = { "VK_LAYER_KHRONOS_validation" };
#define CNFG_NUM_INSTANCE_LAYER sizeof(CNFGInstanceLayers)/sizeof(*CNFGInstanceLayers)

static VkDebugUtilsMessengerEXT CNFGDebugMessenger = NULL;
static VkBool32 VKAPI_CALL CNFGDebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageTypes,
	const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData,
	void* pUserData
)
{
	// Change the color of the terminal to reflect the severity of the message
	if ( messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT ) printf( "\x1B[2;44m" );
	if ( messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT ) printf( "\x1B[40m" );
	if ( messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT ) printf( "\x1B[43m" );
	if ( messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT ) printf( "\x1B[41;30m" );
	
	if ( messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT ) printf( "GENERAL: " );
	if ( messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT ) printf( "VALIDATION: " );
	if ( messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT ) printf( "PERFORMANCE: " );

	printf( "%s\x1B[0m\n", pCallbackData->pMessage );
	
	return VK_FALSE;
}

#endif

static int CNFGScorePhysicalDevice( VkPhysicalDevice dev, uint32_t * didx, uint32_t * pidx )
{
	VkPhysicalDeviceProperties prop;
	vkGetPhysicalDeviceProperties( dev, &prop );

	uint32_t score = 0, api = prop.apiVersion;
	printf( "Device Name: %s, API Version: %i.%i.%i\n", prop.deviceName, VK_VERSION_MAJOR( api ), VK_VERSION_MINOR( api ), VK_VERSION_PATCH( api ) );
	if ( api < VK_VERSION_1_3 ) return -100000;
	if ( prop.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ) score += 5000;
	
	VkQueueFamilyProperties* qfp = NULL;
	uint32_t qfc = 0;
	vkGetPhysicalDeviceQueueFamilyProperties( dev, &qfc, NULL );
	qfp = alloca( qfc * sizeof(VkQueueFamilyProperties) );
	vkGetPhysicalDeviceQueueFamilyProperties( dev, &qfc, qfp );
	for ( int i = 0; i < qfc; i++ )
	{
		VkBool32 support = VK_FALSE;
		if ( qfp[i].queueFlags & VK_QUEUE_GRAPHICS_BIT )
		{
			*didx = i;
		}
		vkGetPhysicalDeviceSurfaceSupportKHR( dev, i, CNFGVkSurface, &support );
		if ( support ) *pidx = i;
		if ( *didx != -1 && *pidx != -1 ) break;
	}
	
	if ( *didx == *pidx ) score += 1000;
	if ( *pidx == -1 ) return -100001;
	
	return score;
}

#define CREATE_MODULE(var, name) \
	VkShaderModuleCreateInfo CI_##var = { \
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, .pNext = NULL, \
		.codeSize = sizeof( name ), .pCode = name \
	}; \
	vkCreateShaderModule( CNFGLogicalDevice, &CI_##var, NULL, &var );

static void CNFGCreatePipelines()
{
	VkShaderModule vert, batch, img;
	CREATE_MODULE( vert, CNFGVertShader );
	CREATE_MODULE( batch, CNFGBatchShader );
	CREATE_MODULE( img, CNFGImageShader );
	
	VkPipelineShaderStageCreateInfo psci[] = {
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, .pNext = NULL,
			.stage = VK_SHADER_STAGE_VERTEX_BIT, .module = vert, .pName = "main"
		},
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, .pNext = NULL,
			.stage = VK_SHADER_STAGE_FRAGMENT_BIT, .module = batch, .pName = "main"
		}
	};
	
	static const VkDynamicState dynstates[] = {
		VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR
	};
	VkPipelineDynamicStateCreateInfo dynamic = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO, .pNext = NULL,
		.pDynamicStates = dynstates, .dynamicStateCount = sizeof( dynstates ) / sizeof( *dynstates )
	};
	VkPipelineVertexInputStateCreateInfo vinp = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO, .pNext = NULL
	};
	VkPipelineInputAssemblyStateCreateInfo inpasm = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO, .pNext = NULL,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
	};
	VkPipelineViewportStateCreateInfo vp = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO, .pNext = NULL,
		.pViewports = NULL, .pScissors = NULL, .scissorCount = 1, .viewportCount = 1
	};
	VkPipelineRasterizationStateCreateInfo rast = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO, .pNext = NULL,
		.polygonMode = VK_POLYGON_MODE_FILL, .lineWidth = 1
	};
	VkPipelineMultisampleStateCreateInfo msaa = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, .pNext = NULL,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT, .sampleShadingEnable = VK_FALSE
	};
	VkPipelineColorBlendAttachmentState batt = {
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
		.blendEnable = VK_TRUE, .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA, .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA
	};
	VkPipelineColorBlendStateCreateInfo blend = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO, .pNext = NULL,
		.logicOpEnable = VK_FALSE, .logicOp = VK_LOGIC_OP_COPY, .attachmentCount = 1, .pAttachments = &batt
	};
	
	VkDescriptorSetLayoutBinding desc[] = {
		{ .binding = 0, .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .descriptorCount = 1, .stageFlags = VK_SHADER_STAGE_VERTEX_BIT },
		{ .binding = 1, .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .descriptorCount = 1, .stageFlags = VK_SHADER_STAGE_VERTEX_BIT },
		{ .binding = 2, .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = 1, .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT }
	};
	
	VkDescriptorSetLayoutCreateInfo dci = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.bindingCount = sizeof(desc)/sizeof(*desc), .pBindings = desc
	};
	
	vkCreateDescriptorSetLayout( CNFGLogicalDevice, &dci, NULL, &CNFGDescriptorSetLayout );
	
	VkPipelineLayoutCreateInfo lci = { .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO, .setLayoutCount = 1, .pSetLayouts = &CNFGDescriptorSetLayout };
	vkCreatePipelineLayout( CNFGLogicalDevice, &lci, NULL, &CNFGPipelineLayout );

	VkPipelineRenderingCreateInfo rend = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO, .pNext = NULL,
		.colorAttachmentCount = 1, .pColorAttachmentFormats = &CNFGSwapchainFormat
	};
	VkGraphicsPipelineCreateInfo batci = {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO, .pNext = &rend,
		.stageCount = 2, .pStages = psci, .pVertexInputState = &vinp, .pInputAssemblyState = &inpasm,
		.pViewportState = &vp, .pRasterizationState = &rast, .pMultisampleState = &msaa,
		.pColorBlendState = &blend, .pDynamicState = &dynamic, .layout = CNFGPipelineLayout,
		.basePipelineHandle = VK_NULL_HANDLE, .basePipelineIndex = -1,
	};
	vkCreateGraphicsPipelines( CNFGLogicalDevice, NULL, 1, &batci, NULL, &CNFGBatchPipeline );
	
	psci[1].module = img;
	vkCreateGraphicsPipelines( CNFGLogicalDevice, NULL, 1, &batci, NULL, &CNFGImagePipeline );
}

static void CNFGDestroySwapchain()
{
	vkDeviceWaitIdle( CNFGLogicalDevice );
	for (int i = 0; i < CNFGSwapchainImageCount; i++ )
	{
		vkDestroyImageView( CNFGLogicalDevice, CNFGSwapchainViews[i], NULL );
		vkDestroySemaphore( CNFGLogicalDevice, CNFGRenderSemas[i], NULL );
		vkDestroySemaphore( CNFGLogicalDevice, CNFGPresentSemas[i], NULL );
	}
	vkDestroySwapchainKHR( CNFGLogicalDevice, CNFGSwapchain, NULL );
	free( CNFGSwapchainImages );
	free( CNFGSwapchainViews );
}

static void CNFGCreateSwapchain()
{
	VkSurfaceCapabilitiesKHR sc = { 0 };
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR( CNFGPhysicalDevice, CNFGVkSurface, &sc );
	
	VkSurfaceFormatKHR formats[256] = {0};
	uint32_t numFormat = 256;
	VkPresentModeKHR presentModes[256] = {0};
	uint32_t numPresent = 256;
	int fmtIdx = 0;
	VkPresentModeKHR present = -1;
	vkGetPhysicalDeviceSurfaceFormatsKHR( CNFGPhysicalDevice, CNFGVkSurface, &numFormat, formats );
	vkGetPhysicalDeviceSurfacePresentModesKHR( CNFGPhysicalDevice, CNFGVkSurface, &numPresent, presentModes );
	for (int i = 0; i < numFormat; i++)
	{
		if ( formats[i].format == VK_FORMAT_B8G8R8A8_UNORM && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR ) fmtIdx = i;
	}
	for (int i = 0; i < numPresent; i++)
	{
		if ( presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR ) present = presentModes[i];
		else if ( presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR && present == -1 ) present = presentModes[i];
	}
	if ( CNFGVSyncOn || (present == -1) ) present = VK_PRESENT_MODE_FIFO_KHR;
	
	uint32_t minImg = sc.minImageCount > 3 ? sc.minImageCount : 3;
	uint32_t imgCount = sc.minImageCount + 1;
	if ( sc.maxImageCount > 0 && imgCount > sc.maxImageCount ) imgCount = sc.maxImageCount;
	if ( sc.maxImageCount > 0 && minImg > sc.maxImageCount ) minImg = sc.maxImageCount;
	
	CNFGSwapchainFormat = formats[fmtIdx].format;
	CNFGSwapchainExtent = sc.currentExtent;
	if ( CNFGScissors[2] > CNFGSwapchainExtent.width ) CNFGScissors[2] = CNFGSwapchainExtent.width;
	if ( CNFGScissors[3] > CNFGSwapchainExtent.height ) CNFGScissors[3] = CNFGSwapchainExtent.height;
	
	VkSwapchainCreateInfoKHR scci = {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR, .pNext = NULL,
		.flags = 0, .surface = CNFGVkSurface, .minImageCount = minImg,
		.imageFormat = CNFGSwapchainFormat, .imageColorSpace = formats[fmtIdx].colorSpace,
		.imageExtent = sc.currentExtent, .imageArrayLayers = 1, .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		.preTransform = sc.currentTransform, .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = present, .clipped = VK_TRUE, .oldSwapchain = NULL
	};
	
	if ( CNFGGraphicsQueue != CNFGPresentQueue )
	{
		uint32_t devdqf, devpqf;
		CNFGScorePhysicalDevice( CNFGPhysicalDevice, &devdqf, &devpqf );
		uint32_t qfis[] = { devdqf, devpqf };
		
		scci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		scci.queueFamilyIndexCount = 2;
		scci.pQueueFamilyIndices = qfis;
	}
	
	vkCreateSwapchainKHR( CNFGLogicalDevice, &scci, NULL, &CNFGSwapchain );
	vkGetSwapchainImagesKHR( CNFGLogicalDevice, CNFGSwapchain, &CNFGSwapchainImageCount, NULL);
	CNFGSwapchainImages = malloc( sizeof( VkImage ) * CNFGSwapchainImageCount );
	CNFGSwapchainViews = malloc( sizeof( VkImageView ) * CNFGSwapchainImageCount );
	CNFGPresentSemas = malloc( sizeof(VkSemaphore) * CNFGSwapchainImageCount );
	CNFGRenderSemas = malloc( sizeof(VkSemaphore) * CNFGSwapchainImageCount );
	vkGetSwapchainImagesKHR( CNFGLogicalDevice, CNFGSwapchain, &CNFGSwapchainImageCount, CNFGSwapchainImages);
	
	VkImageViewCreateInfo ivci = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, .pNext = NULL,
		.viewType = VK_IMAGE_VIEW_TYPE_2D, .format = CNFGSwapchainFormat,
		.subresourceRange.levelCount = 1, .subresourceRange.layerCount = 1,
		.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT
	};
	
	for ( int i = 0; i < CNFGSwapchainImageCount; i++ )
	{
		VkSemaphoreCreateInfo sema = { .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
		vkCreateSemaphore( CNFGLogicalDevice, &sema, NULL, &CNFGPresentSemas[i] );
		vkCreateSemaphore( CNFGLogicalDevice, &sema, NULL, &CNFGRenderSemas[i] );
		
		ivci.image = CNFGSwapchainImages[i];
		vkCreateImageView( CNFGLogicalDevice, &ivci, NULL, &CNFGSwapchainViews[i] );
	}
}

static void CNFGCreateLogicalDevice( uint32_t * o_dqf, uint32_t * o_pqf )
{
	uint32_t numDev = 0;
	VkPhysicalDevice * dev;
	vkEnumeratePhysicalDevices( CNFGVkInstance, &numDev, NULL );
	dev = alloca( sizeof(VkPhysicalDevice) * numDev );
	vkEnumeratePhysicalDevices( CNFGVkInstance, &numDev, dev );
	
	int devIdx = -1, devScore = -1;
	uint32_t devdqf = -1, devpqf = -1;
	
	for ( int i = 0; i < numDev; i++ )
	{
		uint32_t newdqf = -1, newpqf = -1;
		int newScore = CNFGScorePhysicalDevice( dev[i], &newdqf, &newpqf );
		if ( newScore > devScore )
		{
			devIdx = i;
			devScore = newScore;
			devdqf = newdqf;
			devpqf = newpqf;
		}
	}
	
	CNFGPhysicalDevice = dev[devIdx];
	float prio = 0;
	VkDeviceQueueCreateInfo dqci = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.pNext = NULL,
		.queueFamilyIndex = devdqf,
		.queueCount = 1,
		.pQueuePriorities = &prio
	};

	VkPhysicalDeviceVulkan13Features v13 = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES, .pNext = NULL,
		.dynamicRendering = VK_TRUE,
		.synchronization2 = VK_TRUE
	};
	VkPhysicalDeviceFeatures2 pdf = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .features = { 0 }, .pNext = &v13 };
	VkDeviceCreateInfo dci = { .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, .pNext = &pdf,
		.pQueueCreateInfos = &dqci, .queueCreateInfoCount = 1, .ppEnabledExtensionNames = CNFGDeviceExtensions, .enabledExtensionCount = CNFG_NUM_DEVICE_EXT
	};
	vkCreateDevice( CNFGPhysicalDevice, &dci, NULL, &CNFGLogicalDevice );
	vkGetDeviceQueue( CNFGLogicalDevice, devdqf, 0, &CNFGGraphicsQueue );
	vkGetDeviceQueue( CNFGLogicalDevice, devpqf, 0, &CNFGPresentQueue );
	
	*o_dqf = devdqf;
	*o_pqf = devpqf;
}

static void CNFGAllocateMemory( uint64_t size, uint32_t bits, VkMemoryPropertyFlags props, VkDeviceMemory* outMem )
{
	VkPhysicalDeviceMemoryProperties mem = { 0 };
	vkGetPhysicalDeviceMemoryProperties( CNFGPhysicalDevice, &mem );
	int memType = -1;
	for ( int i = 0; i < mem.memoryTypeCount; i++ )
	{
		if (( bits & ( 1 << i )) && ( mem.memoryTypes[i].propertyFlags & props ) == props )
		{
			memType = i;
			break;
		}
	}
	
	VkMemoryAllocateInfo alloc = { .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, .allocationSize = size, .memoryTypeIndex = memType };
	vkAllocateMemory( CNFGLogicalDevice, &alloc, NULL, outMem );
}

static void CNFGCreateResources()
{
	VkBufferCreateInfo bbinfo = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO, .pNext = NULL, .size = CNFGVK_BATCH_SIZE,
		.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, .sharingMode = VK_SHARING_MODE_EXCLUSIVE
	};
	vkCreateBuffer( CNFGLogicalDevice, &bbinfo, NULL, &CNFGBatchBuffer );
	VkMemoryRequirements memReq = { 0 };
	vkGetBufferMemoryRequirements( CNFGLogicalDevice, CNFGBatchBuffer, &memReq );
	CNFGAllocateMemory( memReq.size, memReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &CNFGBatchMemory );
	vkBindBufferMemory( CNFGLogicalDevice, CNFGBatchBuffer, CNFGBatchMemory, 0 );

	CNFGAllocateMemory( CNFGVK_IMAGE_ALLOCATION, 0xFFFFFFFF, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &CNFGImageMemory );
	
	vkMapMemory( CNFGLogicalDevice, CNFGBatchMemory, 0, CNFGVK_BATCH_SIZE, 0, &CNFGBatchMap);
	vkMapMemory( CNFGLogicalDevice, CNFGImageMemory, 0, CNFGVK_IMAGE_ALLOCATION, 0, &CNFGImageMap );
	
	VkDescriptorPoolSize poolsize[] = {
		{ .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .descriptorCount = 2 },
		{ .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = 1 }
	};
	VkDescriptorPoolCreateInfo poolci = { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO, .maxSets = 1, .poolSizeCount = 2, .pPoolSizes = poolsize };
	vkCreateDescriptorPool( CNFGLogicalDevice, &poolci, NULL, &CNFGDescriptorPool );
	
	VkDescriptorSetAllocateInfo setalloc = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, .descriptorPool = CNFGDescriptorPool,
		.descriptorSetCount = 1, .pSetLayouts = &CNFGDescriptorSetLayout
	};
	
	VkSamplerCreateInfo sampci = {
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO, .minFilter = VK_FILTER_NEAREST, .magFilter = VK_FILTER_NEAREST,
		.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
	};
	vkCreateSampler( CNFGLogicalDevice, &sampci, NULL, &CNFGSampler );
	
	VkFenceCreateInfo fci = { .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, .flags = VK_FENCE_CREATE_SIGNALED_BIT };
	vkAllocateDescriptorSets( CNFGLogicalDevice, &setalloc, &CNFGDescriptorSet );
	VkDescriptorBufferInfo infv = { .buffer = CNFGBatchBuffer, .offset = 0, .range = sizeof( CNFGVertDataV )+sizeof( float[4] ) };
	VkDescriptorBufferInfo infc = { .buffer = CNFGBatchBuffer, .offset = sizeof( CNFGVertDataV )+sizeof( float[4] ), .range = sizeof( CNFGVertDataC ) };
	
	VkWriteDescriptorSet write[] = {{
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, .dstSet = CNFGDescriptorSet, .dstBinding = 0,
		.descriptorCount = 1, .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .pBufferInfo = &infv
	}, {
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, .dstSet = CNFGDescriptorSet, .dstBinding = 1,
		.descriptorCount = 1, .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .pBufferInfo = &infc
	}};
	vkUpdateDescriptorSets( CNFGLogicalDevice, 2, write, 0, NULL);
	
	vkCreateFence( CNFGLogicalDevice, &fci, NULL, &CNFGBatchFence );
	
	for ( int i = 0; i < CNFGVK_FRAMES_IN_FLIGHT; i++ )
	{
		VkFenceCreateInfo fence = { .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, .flags = VK_FENCE_CREATE_SIGNALED_BIT };
		vkCreateFence( CNFGLogicalDevice, &fence, NULL, &CNFGRenderFences[i] );
	}
}

void CNFGSetupBatchInternal()
{
	VkApplicationInfo app = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext = NULL,
		.pApplicationName = "CNFG",
		.pEngineName = "CNFG",
		.applicationVersion = VK_MAKE_VERSION( 0, 9, 5 ),
		.engineVersion = VK_MAKE_VERSION( 0, 9, 5 ),
		.apiVersion = VK_API_VERSION_1_3
	};
	
	VkInstanceCreateInfo instInf = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = NULL,
		.pApplicationInfo = &app,

#ifdef CNFGVK_VALIDATION_LAYERS
		.ppEnabledLayerNames = CNFGInstanceLayers,
		.enabledLayerCount = CNFG_NUM_INSTANCE_LAYER,
#endif
		
		.ppEnabledExtensionNames = CNFGInstanceExtensions,
		.enabledExtensionCount = CNFG_NUM_INSTANCE_EXT
	};
	
	VkResult res = vkCreateInstance( &instInf, NULL, (VkInstance*)&CNFGVkInstance );
	if (res != VK_SUCCESS) {
		fprintf( stderr, "Unable to create Vulkan instance with code %i\n", res );
		exit( -1 );
	}
	
#ifdef CNFGVK_VALIDATION_LAYERS
	VkDebugUtilsMessengerCreateInfoEXT msgInfo = {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.pNext = NULL,
		.messageSeverity = (VkDebugUtilsMessageSeverityFlagsEXT){VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT},
		.messageType = (VkDebugUtilsMessageTypeFlagsEXT){VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT},
		.pfnUserCallback = CNFGDebugCallback
	};
	CNFG_LOAD_FUNC( dbgmsg, vkCreateDebugUtilsMessengerEXT );
	dbgmsg( CNFGVkInstance, &msgInfo, NULL, &CNFGDebugMessenger );
#endif

	CNFGCreateVkSurface( CNFGVkInstance, NULL, &CNFGVkSurface );

	uint32_t devdqf, devpqf;
	CNFGCreateLogicalDevice( &devdqf, &devpqf );
	CNFGCreateSwapchain();
	CNFGCreatePipelines();
	
	VkCommandPoolCreateInfo cpci = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, .pNext = NULL, .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, .queueFamilyIndex = devdqf
	};
	vkCreateCommandPool( CNFGLogicalDevice, &cpci, NULL, &CNFGGraphicsPool );
	
	VkCommandBufferAllocateInfo cbai = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, .pNext = NULL,
		.commandPool = CNFGGraphicsPool, .commandBufferCount = CNFGVK_FRAMES_IN_FLIGHT, .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY
	};
	vkAllocateCommandBuffers( CNFGLogicalDevice, &cbai, CNFGGraphicsCommands );
	cbai.commandBufferCount = 1;
	vkAllocateCommandBuffers( CNFGLogicalDevice, &cbai, &CNFGBatchCommand );

	CNFGCreateResources();
}

static void CNFGTransImageLayout( VkCommandBuffer cmd, VkImage img, VkImageLayout old, VkImageLayout new, VkAccessFlags2 srcf, VkAccessFlags2 dstf, VkPipelineStageFlags2 srcs, VkPipelineStageFlags2 dsts )
{
	VkImageMemoryBarrier2 barrier = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2, .pNext = NULL,
		.srcStageMask = srcs, .dstStageMask = dsts, .srcAccessMask = srcf, .dstAccessMask = dstf,
		.oldLayout = old, .newLayout = new, .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED, .image = img,
		.subresourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .baseArrayLayer = 0, .layerCount = 1, .levelCount = 1
		}
	};
	VkDependencyInfo dep = {
		.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO, .pNext = NULL,
		.imageMemoryBarrierCount = 1, .pImageMemoryBarriers = &barrier
	};
	vkCmdPipelineBarrier2( cmd, &dep );
}

static void CNFGBeginCmd( VkCommandBuffer cmd )
{
	VkCommandBufferBeginInfo begin = { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, .pNext = NULL, .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT };
	vkResetCommandBuffer( cmd, 0 );
	vkBeginCommandBuffer( cmd, &begin );
}

static void CNFGEndCmd( VkCommandBuffer cmd, VkFence fence )
{
	vkEndCommandBuffer( cmd );
	VkSubmitInfo submit = { .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO, .commandBufferCount = 1, .pCommandBuffers = &cmd };
	vkQueueSubmit( CNFGGraphicsQueue, 1, &submit, fence );
}

static void CNFGEndFrame()
{
	CNFGFlushRender();
	
	CNFGBeginCmd( CNFGGraphicsCommands[CNFGCurFlight] );
	CNFGTransImageLayout( CNFGGraphicsCommands[CNFGCurFlight], CNFGSwapchainImages[CNFGCurSwapchainImage],
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, 0,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT
	);
	vkEndCommandBuffer( CNFGGraphicsCommands[CNFGCurFlight] );
	
	vkResetFences( CNFGLogicalDevice, 1, &CNFGRenderFences[CNFGCurFlight] );
	
	VkPipelineStageFlags wait = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo sub = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO, .pNext = NULL,
		.waitSemaphoreCount = 1, .pWaitSemaphores = &CNFGPresentSemas[CNFGCurSema], .pWaitDstStageMask = &wait,
		.commandBufferCount = 1, .pCommandBuffers = &CNFGGraphicsCommands[CNFGCurFlight], .signalSemaphoreCount = 1,
		.pSignalSemaphores = &CNFGRenderSemas[CNFGCurSema]
	};
	vkQueueSubmit( CNFGGraphicsQueue, 1, &sub, CNFGRenderFences[CNFGCurFlight] );
	
	VkPresentInfoKHR present = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR, .pNext = NULL, 
		.waitSemaphoreCount = 1, .pWaitSemaphores = &CNFGRenderSemas[CNFGCurSema],
		.swapchainCount = 1, .pSwapchains = &CNFGSwapchain, .pImageIndices = &CNFGCurSwapchainImage, .pResults = NULL
	};
	vkQueuePresentKHR( CNFGPresentQueue, &present );
	
	CNFGCurSwapchainImage = -1;
	CNFGCurFlight = (CNFGCurFlight + 1) % CNFGVK_FRAMES_IN_FLIGHT;
	CNFGCurSema = (CNFGCurSema + 1) % CNFGSwapchainImageCount;
}

static void CNFGBeginRender( VkCommandBuffer cmd )
{
	if ( CNFGCurSwapchainImage == -1 ) CNFGSwapBuffers();
	
	VkRenderingAttachmentInfo attach = {
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
		.imageView = CNFGSwapchainViews[CNFGCurSwapchainImage],
		.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE
	};
	VkRenderingInfo rendInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
		.renderArea = {.offset={0}, .extent=CNFGSwapchainExtent},
		.colorAttachmentCount = 1, .layerCount = 1, .pColorAttachments = &attach	
	};
	
	vkCmdBeginRendering( cmd, &rendInfo );
	vkCmdSetViewport( cmd, 0, 1, (VkViewport[]){{ .x=0, .y=0, .width=CNFGSwapchainExtent.width, .height=CNFGSwapchainExtent.height, 0, 1 }} );
	if ( CNFGScissors[0] >= 0 ) vkCmdSetScissor( cmd, 0, 1, (VkRect2D[]){{ {CNFGScissors[0], CNFGScissors[1]}, {CNFGScissors[2], CNFGScissors[3]} }} );
	else vkCmdSetScissor( cmd, 0, 1, (VkRect2D[]){{ {0}, CNFGSwapchainExtent }} );
	vkCmdBindDescriptorSets( cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, CNFGPipelineLayout, 0, 1, &CNFGDescriptorSet, 0, NULL );
}

void CNFGInternalResize( short x, short y )
{
	if ( CNFGCurSwapchainImage != (uint32_t)-1 )
	{
		CNFGCurSwapchainImage = -1;
		CNFGCurFlight = (CNFGCurFlight + 1) % CNFGVK_FRAMES_IN_FLIGHT;
		CNFGCurSema = (CNFGCurSema + 1) % CNFGSwapchainImageCount;
	}
	vkDeviceWaitIdle( CNFGLogicalDevice );
	
	CNFGDestroySwapchain();
	CNFGCreateSwapchain();
	
	memcpy( CNFGBatchMap, (float[]){ 2.f/x, 2.f/y, -1.f, -1.f }, sizeof( float[4] ) );
}

static void CNFGWaitForBatchBuffer()
{
	vkWaitForFences( CNFGLogicalDevice, 1, &CNFGBatchFence, VK_FALSE, UINT64_MAX );
	vkResetFences( CNFGLogicalDevice, 1, &CNFGBatchFence );
}

void CNFGEmitBackendTriangles( const float * vertices, const uint32_t * colors, int num_vertices )
{
	CNFGWaitForBatchBuffer();

	CNFGBeginCmd( CNFGBatchCommand );
	CNFGBeginRender( CNFGBatchCommand );
	
	memcpy( CNFGBatchMap + sizeof( float[4] ), vertices, num_vertices * sizeof( float[3] ) );
	memcpy( CNFGBatchMap + sizeof( CNFGVertDataV ) + sizeof( float[4] ), colors, sizeof( uint32_t ) * num_vertices );
	
	vkCmdBindPipeline( CNFGBatchCommand, VK_PIPELINE_BIND_POINT_GRAPHICS, CNFGBatchPipeline );
	vkCmdDraw( CNFGBatchCommand, num_vertices, 1, 0, 0 );
	
	vkCmdEndRendering( CNFGBatchCommand );
	CNFGEndCmd( CNFGBatchCommand, CNFGBatchFence );
}

void CNFGBlitImage( uint32_t * data, int x, int y, int w, int h )
{
	CNFGFlushRender();
	CNFGWaitForBatchBuffer();

	vkDestroyImageView( CNFGLogicalDevice, CNFGImageView, NULL );
	vkDestroyImage( CNFGLogicalDevice, CNFGImage, NULL );
	
	VkImageCreateInfo imgci = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO, .imageType = VK_IMAGE_TYPE_2D, .format = VK_FORMAT_R8G8B8A8_UNORM,
		.extent = { w, h, 1 }, .mipLevels = 1, .arrayLayers = 1, .samples = VK_SAMPLE_COUNT_1_BIT, .tiling = VK_IMAGE_TILING_LINEAR,
		.usage = VK_IMAGE_USAGE_SAMPLED_BIT, .sharingMode = VK_SHARING_MODE_EXCLUSIVE, .initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED
	};
	vkCreateImage( CNFGLogicalDevice, &imgci, NULL, &CNFGImage);
	vkBindImageMemory( CNFGLogicalDevice, CNFGImage, CNFGImageMemory, 0 );
	memcpy( CNFGImageMap, data, w*h*sizeof( uint32_t ) );
	
	CNFGBeginCmd( CNFGBatchCommand );
	CNFGTransImageLayout(
		CNFGBatchCommand, CNFGImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		VK_ACCESS_2_TRANSFER_WRITE_BIT, VK_ACCESS_2_SHADER_READ_BIT, VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT
	);
	
	VkImageViewCreateInfo viewci = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, .image = CNFGImage, .viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = VK_FORMAT_R8G8B8A8_UNORM, .subresourceRange = { .aspectMask=VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
	};
	vkCreateImageView( CNFGLogicalDevice, &viewci, NULL, &CNFGImageView );
	
	VkDescriptorImageInfo  infi = {.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, .sampler = CNFGSampler, .imageView = CNFGImageView };
	VkWriteDescriptorSet write = {
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, .dstSet = CNFGDescriptorSet, .dstBinding = 2,
		.descriptorCount = 1, .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .pImageInfo = &infi
	};
	vkUpdateDescriptorSets( CNFGLogicalDevice, 1, &write, 0, NULL );
	
	CNFGBeginRender( CNFGBatchCommand );
	
	const float verts[] = { x, y, 0, x+w, y, 0, x+w, y+h, 0, x, y, 0, x+w, y+h, 0, x, y+h, 0 };
	const uint32_t uvs[] = { 0x00000000, 0xFF000000, 0xFFFF0000, 0x00000000, 0xFFFF0000, 0x00FF0000 };
	
	memcpy( CNFGBatchMap + sizeof( float[4] ), verts, sizeof( verts ) );
	memcpy( CNFGBatchMap + sizeof( CNFGVertDataV ) + sizeof( float[4] ), uvs, sizeof( uvs ) );
	vkCmdBindPipeline( CNFGBatchCommand, VK_PIPELINE_BIND_POINT_GRAPHICS, CNFGImagePipeline );
	vkCmdDraw( CNFGBatchCommand, 6, 1, 0, 0 );
	
	vkCmdEndRendering( CNFGBatchCommand );
	CNFGEndCmd( CNFGBatchCommand, CNFGBatchFence );
}

// Could use vkCmdClearColorImage, but it doesn't respect scissors
void CNFGClearFrame()
{
	uint32_t temp = CNFGLastColor;
	CNFGLastColor = CNFGBGColor;
	CNFGEmitQuad( 0, 0, 0, CNFGSwapchainExtent.height, CNFGSwapchainExtent.width, 0, CNFGSwapchainExtent.width, CNFGSwapchainExtent.height );
	CNFGLastColor = temp;
}

void CNFGSwapBuffers()
{
	if ( CNFGCurSwapchainImage != (uint32_t)-1 ) CNFGEndFrame();
	
	vkWaitForFences( CNFGLogicalDevice, 1, &CNFGRenderFences[CNFGCurFlight], VK_TRUE, UINT64_MAX );
	vkAcquireNextImageKHR( CNFGLogicalDevice, CNFGSwapchain, UINT64_MAX, CNFGPresentSemas[CNFGCurSema], NULL, &CNFGCurSwapchainImage);
	
	CNFGBeginCmd( CNFGGraphicsCommands[CNFGCurFlight] );
	CNFGTransImageLayout( CNFGGraphicsCommands[CNFGCurFlight], CNFGSwapchainImages[CNFGCurSwapchainImage],
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		0, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT
	);
	CNFGEndCmd( CNFGGraphicsCommands[CNFGCurFlight], NULL );
}

void CNFGSetVSync( int vson )
{
	CNFGVSyncOn = vson;
	CNFGInternalResize( CNFGSwapchainExtent.width, CNFGSwapchainExtent.height );
}

void CNFGSetScissors( int * xywh )
{
	CNFGFlushRender();
	memcpy( CNFGScissors, xywh, sizeof(int)*4 );
}

void CNFGGetScissors( int * xywh )
{
	memcpy( xywh, CNFGScissors, sizeof(int)*4 );
}
