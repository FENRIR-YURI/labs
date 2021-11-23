// minimalistic code to draw a single triangle, this is not part of the API.
#include "shaderc/shaderc.h" // needed for compiling shaders at runtime
#ifdef _WIN32 // must use MT platform DLL libraries on windows
#pragma comment(lib, "shaderc_combined.lib") 
#endif
// TODO: Part 2b
// Simple Vertex Shader
const char* vertexShaderSource = R"(
// an ultra simple hlsl vertex shader
float4 main(float2 inputVertex : POSITION) : SV_POSITION
{
	return float4(inputVertex, 0, 1);
}
)";
// Simple Pixel Shader
const char* pixelShaderSource = R"(
// an ultra simple hlsl pixel shader
float4 main() : SV_TARGET 
{	
	return float4(0.75f ,0.75f, 0.75f, 0); // TODO: Part 1a 
}
)";
// TODO: Part 4b
const char* vertexShaderSourceJR = R"(
// an ultra simple hlsl vertex shader
// TODO: Part 4c (in new shader)
struct VERTEX
{
float2 position : POSITION;
float4 color : COLOR;
};
struct VERTEX_OUT
{
float4 position : SV_POSITION;
float4 color : COLOR;
};
VERTEX_OUT mainJR(VERTEX inputVertex : POSITION) : SV_POSITION
{
    VERTEX_OUT vertex;
    vertex.color = inputVertex.color;
    vertex.position = float4( inputVertex.position, 0, 1);
	return vertex;
}
)";
// Simple Pixel Shader
const char* pixelShaderSourceJR = R"(
// an ultra simple hlsl pixel shader
// TODO: Part 4d (in new shader)
struct VERTEX_OUT
{
float4 position : SV_POSITION;
float4 color : COLOR;
};
float4 mainJR(VERTEX_OUT vertexColor) : SV_TARGET 
{	
	return vertexColor.color;
}
)";
// TODO: Part 4e
// Creation, Rendering & Cleanup
class Renderer
{
	// proxy handles
	GW::SYSTEM::GWindow win;
	GW::GRAPHICS::GVulkanSurface vlk;
	GW::CORE::GEventReceiver shutdown;
	// what we need at a minimum to draw a triangle
	VkDevice device = nullptr;
	VkBuffer vertexHandle = nullptr;
	VkDeviceMemory vertexData = nullptr;
	// TODO: Part 3a
	VkBuffer vertexHandleJR = nullptr;
	VkBuffer vertexHandle3rd = nullptr;
	VkBuffer vertexHandle4th = nullptr;
	VkBuffer vertexHandle5th = nullptr;
	VkShaderModule vertexShader = nullptr;
	VkShaderModule pixelShader = nullptr;
	VkPipeline pipelineJR;
	// TODO: Part 4b
	VkShaderModule vertexShaderJR;
	VkShaderModule pixelShaderJR;
	// pipeline settings for drawing (also required)
	VkPipeline pipeline = nullptr;
	VkPipelineLayout pipelineLayout = nullptr;
public:
	// TODO: Part 4a
	struct VERTEX
	{
		float X, Y;
		float R, G, B, A;
	};
	struct container
	{
		float filler[20000];
	};
	struct VERTEXholder
	{
		VERTEX ver[11];
	};
	Renderer(GW::SYSTEM::GWindow _win, GW::GRAPHICS::GVulkanSurface _vlk)
	{
		win = _win;
		vlk = _vlk;
		unsigned int width, height;
		win.GetClientWidth(width);
		win.GetClientHeight(height);
		/***************** GEOMETRY INTIALIZATION ******************/
		// Grab the device & physical device so we can allocate some stuff
		VkPhysicalDevice physicalDevice = nullptr;
		vlk.GetDevice((void**)&device);
		vlk.GetPhysicalDevice((void**)&physicalDevice);
		// TODO: Part 2b
		// srand(time(0));
		container BackgroundStars = StarGen();

		// Create Vertex Buffer
		/*float verts[] = {
			   0,   0.5f,
			 0.5f, -0.5f,
			-0.5f, -0.5f
		};*/
		float x[] = {
			   0,
			 0.2f,

			0.93f,

			0.35f,

			0.58f,

			0.0f ,


			-0.58f,

			-0.35f,

			-0.93f,

			-0.2f,
			0.0f

		};
		float y[] = {
			 1.0f,
			0.4f,

			0.39f,

			-0.1f,

			-0.82f,

			-0.4f,


			 -0.82f,

			 -0.1f,

			 0.39f,

			 0.4f,
			 1.0f
		};
		VERTEXholder BigStar;
		BigStar = BigStarGen(x, y, 1);
		VERTEXholder BigStarJR;
		BigStarJR = BigStarGen(x, y, 2);
		VERTEXholder BigStarJR2;
		BigStarJR2 = BigStarGen(x, y, 3);
		VERTEXholder BigStarJR3;
		BigStarJR3 = BigStarGen(x, y, 4);
		// Transfer triangle data to the vertex buffer. (staging would be prefered here)
		GvkHelper::create_buffer(physicalDevice, device, sizeof(BackgroundStars.filler),
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vertexHandle, &vertexData);
		GvkHelper::write_to_buffer(device, vertexData, BackgroundStars.filler, sizeof(BackgroundStars.filler));
		// TODO: Part 3a
		// TODO: Part 4a
		GvkHelper::create_buffer(physicalDevice, device, sizeof(BigStar.ver),
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vertexHandleJR, &vertexData);
		GvkHelper::write_to_buffer(device, vertexData, BigStar.ver, sizeof(BigStar.ver));

		GvkHelper::create_buffer(physicalDevice, device, sizeof(BigStarJR.ver),
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vertexHandle3rd, &vertexData);
		GvkHelper::write_to_buffer(device, vertexData, BigStarJR.ver, sizeof(BigStarJR.ver));

		GvkHelper::create_buffer(physicalDevice, device, sizeof(BigStarJR2.ver),
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vertexHandle4th, &vertexData);
		GvkHelper::write_to_buffer(device, vertexData, BigStarJR2.ver, sizeof(BigStarJR2.ver));

		GvkHelper::create_buffer(physicalDevice, device, sizeof(BigStarJR3.ver),
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vertexHandle5th, &vertexData);
		GvkHelper::write_to_buffer(device, vertexData, BigStarJR3.ver, sizeof(BigStarJR3.ver));
		/***************** SHADER INTIALIZATION ******************/
		// Intialize runtime shader compiler HLSL -> SPIRV
		shaderc_compiler_t compiler = shaderc_compiler_initialize();
		shaderc_compile_options_t options = shaderc_compile_options_initialize();
		shaderc_compile_options_set_source_language(options, shaderc_source_language_hlsl);
		shaderc_compile_options_set_invert_y(options, true);
#ifndef NDEBUG
		shaderc_compile_options_set_generate_debug_info(options);
#endif
		// Create Vertex Shader
		shaderc_compilation_result_t result = shaderc_compile_into_spv( // compile
			compiler, vertexShaderSource, strlen(vertexShaderSource),
			shaderc_vertex_shader, "main.vert", "main", options);
		if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) // errors?
			std::cout << "Vertex Shader Errors: " << shaderc_result_get_error_message(result) << std::endl;
		GvkHelper::create_shader_module(device, shaderc_result_get_length(result), // load into Vulkan
			(char*)shaderc_result_get_bytes(result), &vertexShader);
		shaderc_result_release(result); // done
		// Create Pixel Shader
		result = shaderc_compile_into_spv( // compile
			compiler, pixelShaderSource, strlen(pixelShaderSource),
			shaderc_fragment_shader, "main.frag", "main", options);
		if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) // errors?
			std::cout << "Pixel Shader Errors: " << shaderc_result_get_error_message(result) << std::endl;
		GvkHelper::create_shader_module(device, shaderc_result_get_length(result), // load into Vulkan
			(char*)shaderc_result_get_bytes(result), &pixelShader);
		shaderc_result_release(result); // done


		// TODO: Part 4b
		// Create Vertex ShaderJR
		shaderc_compilation_result_t resultJR = shaderc_compile_into_spv( // compile
			compiler, vertexShaderSourceJR, strlen(vertexShaderSourceJR),
			shaderc_vertex_shader, "mainJR.vert", "mainJR", options);
		if (shaderc_result_get_compilation_status(resultJR) != shaderc_compilation_status_success) // errors?
			std::cout << "Vertex Shader Errors: " << shaderc_result_get_error_message(resultJR) << std::endl;
		GvkHelper::create_shader_module(device, shaderc_result_get_length(resultJR), // load into Vulkan
			(char*)shaderc_result_get_bytes(resultJR), &vertexShaderJR);
		shaderc_result_release(resultJR); // done
		// Create Pixel ShaderJR
		resultJR = shaderc_compile_into_spv( // compile
			compiler, pixelShaderSourceJR, strlen(pixelShaderSourceJR),
			shaderc_fragment_shader, "mainJR.frag", "mainJR", options);
		if (shaderc_result_get_compilation_status(resultJR) != shaderc_compilation_status_success) // errors?
			std::cout << "Pixel Shader Errors: " << shaderc_result_get_error_message(resultJR) << std::endl;
		GvkHelper::create_shader_module(device, shaderc_result_get_length(resultJR), // load into Vulkan
			(char*)shaderc_result_get_bytes(resultJR), &pixelShaderJR);
		shaderc_result_release(resultJR); // done
		// Free runtime shader compiler resources
		shaderc_compile_options_release(options);
		shaderc_compiler_release(compiler);

		/***************** PIPELINE INTIALIZATION ******************/
		// Create Pipeline & Layout (Thanks Tiny!)
		VkRenderPass renderPass;
		vlk.GetRenderPass((void**)&renderPass);
		VkPipelineShaderStageCreateInfo stage_create_info[2] = {};
		// Create Stage Info for Vertex Shader
		stage_create_info[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stage_create_info[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		stage_create_info[0].module = vertexShader;
		stage_create_info[0].pName = "main";
		// Create Stage Info for Fragment Shader
		stage_create_info[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stage_create_info[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		stage_create_info[1].module = pixelShader;
		stage_create_info[1].pName = "main";
		// Assembly State
		VkPipelineInputAssemblyStateCreateInfo assembly_create_info = {};
		assembly_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		assembly_create_info.primitiveRestartEnable = false;
		// Vertex Input State
		VkVertexInputBindingDescription vertex_binding_description = {};
		vertex_binding_description.binding = 0;
		vertex_binding_description.stride = sizeof(float) * 2;
		vertex_binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		VkVertexInputAttributeDescription vertex_attribute_description[1] = {
			{ 0, 0, VK_FORMAT_R32G32_SFLOAT, 0 } //uv, normal, etc....
		};
		VkPipelineVertexInputStateCreateInfo input_vertex_info = {};
		input_vertex_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		input_vertex_info.vertexBindingDescriptionCount = 1;
		input_vertex_info.pVertexBindingDescriptions = &vertex_binding_description;
		input_vertex_info.vertexAttributeDescriptionCount = 1;
		input_vertex_info.pVertexAttributeDescriptions = vertex_attribute_description;
		// Viewport State (we still need to set this up even though we will overwrite the values)
		VkViewport viewport = {
			0, 0, static_cast<float>(width), static_cast<float>(height), 0, 1
		};
		VkRect2D scissor = { {0, 0}, {width, height} };
		VkPipelineViewportStateCreateInfo viewport_create_info = {};
		viewport_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewport_create_info.viewportCount = 1;
		viewport_create_info.pViewports = &viewport;
		viewport_create_info.scissorCount = 1;
		viewport_create_info.pScissors = &scissor;
		// Rasterizer State
		VkPipelineRasterizationStateCreateInfo rasterization_create_info = {};
		rasterization_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterization_create_info.rasterizerDiscardEnable = VK_FALSE;
		rasterization_create_info.polygonMode = VK_POLYGON_MODE_POINT; // TODO: Part 2a
		rasterization_create_info.lineWidth = 1.0f;
		rasterization_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterization_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterization_create_info.depthClampEnable = VK_FALSE;
		rasterization_create_info.depthBiasEnable = VK_FALSE;
		rasterization_create_info.depthBiasClamp = 0.0f;
		rasterization_create_info.depthBiasConstantFactor = 0.0f;
		rasterization_create_info.depthBiasSlopeFactor = 0.0f;
		// Multisampling State
		VkPipelineMultisampleStateCreateInfo multisample_create_info = {};
		multisample_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisample_create_info.sampleShadingEnable = VK_FALSE;
		multisample_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisample_create_info.minSampleShading = 1.0f;
		multisample_create_info.pSampleMask = VK_NULL_HANDLE;
		multisample_create_info.alphaToCoverageEnable = VK_FALSE;
		multisample_create_info.alphaToOneEnable = VK_FALSE;
		// Depth-Stencil State
		VkPipelineDepthStencilStateCreateInfo depth_stencil_create_info = {};
		depth_stencil_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depth_stencil_create_info.depthTestEnable = VK_FALSE;
		depth_stencil_create_info.depthWriteEnable = VK_FALSE;
		depth_stencil_create_info.depthCompareOp = VK_COMPARE_OP_LESS;
		depth_stencil_create_info.depthBoundsTestEnable = VK_FALSE;
		depth_stencil_create_info.minDepthBounds = 0.0f;
		depth_stencil_create_info.maxDepthBounds = 1.0f;
		depth_stencil_create_info.stencilTestEnable = VK_FALSE;
		// Color Blending Attachment & State
		VkPipelineColorBlendAttachmentState color_blend_attachment_state = {};
		color_blend_attachment_state.colorWriteMask = 0xF;
		color_blend_attachment_state.blendEnable = VK_FALSE;
		color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
		color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_DST_COLOR;
		color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
		color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
		color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;
		VkPipelineColorBlendStateCreateInfo color_blend_create_info = {};
		color_blend_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		color_blend_create_info.logicOpEnable = VK_FALSE;
		color_blend_create_info.logicOp = VK_LOGIC_OP_COPY;
		color_blend_create_info.attachmentCount = 1;
		color_blend_create_info.pAttachments = &color_blend_attachment_state;
		color_blend_create_info.blendConstants[0] = 0.0f;
		color_blend_create_info.blendConstants[1] = 0.0f;
		color_blend_create_info.blendConstants[2] = 0.0f;
		color_blend_create_info.blendConstants[3] = 0.0f;
		// Dynamic State 
		VkDynamicState dynamic_state[2] = {
			// By setting these we do not need to re-create the pipeline on Resize
			VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo dynamic_create_info = {};
		dynamic_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamic_create_info.dynamicStateCount = 2;
		dynamic_create_info.pDynamicStates = dynamic_state;
		// Descriptor pipeline layout
		VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
		pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_create_info.setLayoutCount = 0;
		pipeline_layout_create_info.pSetLayouts = VK_NULL_HANDLE;
		pipeline_layout_create_info.pushConstantRangeCount = 0;
		pipeline_layout_create_info.pPushConstantRanges = nullptr;
		vkCreatePipelineLayout(device, &pipeline_layout_create_info,
			nullptr, &pipelineLayout);
		// Pipeline State... (FINALLY) 
		VkGraphicsPipelineCreateInfo pipeline_create_info = {};
		pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_create_info.stageCount = 2;
		pipeline_create_info.pStages = stage_create_info;
		pipeline_create_info.pInputAssemblyState = &assembly_create_info;
		pipeline_create_info.pVertexInputState = &input_vertex_info;
		pipeline_create_info.pViewportState = &viewport_create_info;
		pipeline_create_info.pRasterizationState = &rasterization_create_info;
		pipeline_create_info.pMultisampleState = &multisample_create_info;
		pipeline_create_info.pDepthStencilState = &depth_stencil_create_info;
		pipeline_create_info.pColorBlendState = &color_blend_create_info;
		pipeline_create_info.pDynamicState = &dynamic_create_info;
		pipeline_create_info.layout = pipelineLayout;
		pipeline_create_info.renderPass = renderPass;
		pipeline_create_info.subpass = 0;
		pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
		vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1,
			&pipeline_create_info, nullptr, &pipeline);

	//pipeline jr.
/***************** PIPELINE INTIALIZATION ******************/
		// Create Pipeline & Layout (Thanks Tiny!)
		VkRenderPass renderPassJR;
		vlk.GetRenderPass((void**)&renderPassJR);
		VkPipelineShaderStageCreateInfo stage_create_infoJR[2] = {};
		// Create Stage Info for Vertex Shader
		stage_create_infoJR[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stage_create_infoJR[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		stage_create_infoJR[0].module = vertexShaderJR;
		stage_create_infoJR[0].pName = "mainJR";
		// Create Stage Info for Fragment Shader
		stage_create_infoJR[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stage_create_infoJR[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		stage_create_infoJR[1].module = pixelShaderJR;
		stage_create_infoJR[1].pName = "mainJR";
		// Assembly State
		VkPipelineInputAssemblyStateCreateInfo assembly_create_infoJR = {};
		assembly_create_infoJR.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		assembly_create_infoJR.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		assembly_create_infoJR.primitiveRestartEnable = false;
		// Vertex Input State
		VkVertexInputBindingDescription vertex_binding_descriptionJR = {};
		vertex_binding_descriptionJR.binding = 0;
		vertex_binding_descriptionJR.stride = sizeof(float) * 6;
		vertex_binding_descriptionJR.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		VkVertexInputAttributeDescription vertex_attribute_descriptionJR[2] = {
			{ 0, 0, VK_FORMAT_R32G32_SFLOAT, 0 }, //uv, normal, etc....
			{ 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 8 }
		};
		VkPipelineVertexInputStateCreateInfo input_vertex_infoJR = {};
		input_vertex_infoJR.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		input_vertex_infoJR.vertexBindingDescriptionCount = 1;
		input_vertex_infoJR.pVertexBindingDescriptions = &vertex_binding_descriptionJR;
		input_vertex_infoJR.vertexAttributeDescriptionCount = 2;
		input_vertex_infoJR.pVertexAttributeDescriptions = vertex_attribute_descriptionJR;
		// Viewport State (we still need to set this up even though we will overwrite the values)
		VkViewport viewportJR = {
			0, 0, static_cast<float>(width), static_cast<float>(height), 0, 1
		};
		VkRect2D scissorJR = { {0, 0}, {width, height} };
		VkPipelineViewportStateCreateInfo viewport_create_infoJR = {};
		viewport_create_infoJR.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewport_create_infoJR.viewportCount = 1;
		viewport_create_infoJR.pViewports = &viewportJR;
		viewport_create_infoJR.scissorCount = 1;
		viewport_create_infoJR.pScissors = &scissorJR;
		// Rasterizer State
		VkPipelineRasterizationStateCreateInfo rasterization_create_infoJR = {};
		rasterization_create_infoJR.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterization_create_infoJR.rasterizerDiscardEnable = VK_FALSE;
		rasterization_create_infoJR.polygonMode = VK_POLYGON_MODE_POINT; // TODO: Part 2a
		rasterization_create_infoJR.lineWidth = 1.0f;
		rasterization_create_infoJR.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterization_create_infoJR.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterization_create_infoJR.depthClampEnable = VK_FALSE;
		rasterization_create_infoJR.depthBiasEnable = VK_FALSE;
		rasterization_create_infoJR.depthBiasClamp = 0.0f;
		rasterization_create_infoJR.depthBiasConstantFactor = 0.0f;
		rasterization_create_infoJR.depthBiasSlopeFactor = 0.0f;
		// Multisampling State
		VkPipelineMultisampleStateCreateInfo multisample_create_infoJR = {};
		multisample_create_infoJR.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisample_create_infoJR.sampleShadingEnable = VK_FALSE;
		multisample_create_infoJR.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisample_create_infoJR.minSampleShading = 1.0f;
		multisample_create_infoJR.pSampleMask = VK_NULL_HANDLE;
		multisample_create_infoJR.alphaToCoverageEnable = VK_FALSE;
		multisample_create_infoJR.alphaToOneEnable = VK_FALSE;
		// Depth-Stencil State
		VkPipelineDepthStencilStateCreateInfo depth_stencil_create_infoJR = {};
		depth_stencil_create_infoJR.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depth_stencil_create_infoJR.depthTestEnable = VK_FALSE;
		depth_stencil_create_infoJR.depthWriteEnable = VK_FALSE;
		depth_stencil_create_infoJR.depthCompareOp = VK_COMPARE_OP_LESS;
		depth_stencil_create_infoJR.depthBoundsTestEnable = VK_FALSE;
		depth_stencil_create_infoJR.minDepthBounds = 0.0f;
		depth_stencil_create_infoJR.maxDepthBounds = 1.0f;
		depth_stencil_create_infoJR.stencilTestEnable = VK_FALSE;
		// Color Blending Attachment & State
		VkPipelineColorBlendAttachmentState color_blend_attachment_stateJR = {};
		color_blend_attachment_stateJR.colorWriteMask = 0xF;
		color_blend_attachment_stateJR.blendEnable = VK_FALSE;
		color_blend_attachment_stateJR.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
		color_blend_attachment_stateJR.dstColorBlendFactor = VK_BLEND_FACTOR_DST_COLOR;
		color_blend_attachment_stateJR.colorBlendOp = VK_BLEND_OP_ADD;
		color_blend_attachment_stateJR.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		color_blend_attachment_stateJR.dstAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
		color_blend_attachment_stateJR.alphaBlendOp = VK_BLEND_OP_ADD;
		VkPipelineColorBlendStateCreateInfo color_blend_create_infoJR = {};
		color_blend_create_infoJR.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		color_blend_create_infoJR.logicOpEnable = VK_FALSE;
		color_blend_create_infoJR.logicOp = VK_LOGIC_OP_COPY;
		color_blend_create_infoJR.attachmentCount = 1;
		color_blend_create_infoJR.pAttachments = &color_blend_attachment_stateJR;
		color_blend_create_infoJR.blendConstants[0] = 0.0f;
		color_blend_create_infoJR.blendConstants[1] = 0.0f;
		color_blend_create_infoJR.blendConstants[2] = 0.0f;
		color_blend_create_infoJR.blendConstants[3] = 0.0f;
		// Dynamic State 
		VkDynamicState dynamic_stateJR[2] = {
			// By setting these we do not need to re-create the pipeline on Resize
			VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo dynamic_create_infoJR = {};
		dynamic_create_infoJR.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamic_create_infoJR.dynamicStateCount = 2;
		dynamic_create_infoJR.pDynamicStates = dynamic_stateJR;
		// Descriptor pipeline layout
		VkPipelineLayoutCreateInfo pipeline_layout_create_infoJR = {};
		pipeline_layout_create_infoJR.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_create_infoJR.setLayoutCount = 0;
		pipeline_layout_create_infoJR.pSetLayouts = VK_NULL_HANDLE;
		pipeline_layout_create_infoJR.pushConstantRangeCount = 0;
		pipeline_layout_create_infoJR.pPushConstantRanges = nullptr;
		vkCreatePipelineLayout(device, &pipeline_layout_create_infoJR,
			nullptr, &pipelineLayout);
		// Pipeline State... (FINALLY) 
		VkGraphicsPipelineCreateInfo pipeline_create_infoJR = {};
		pipeline_create_infoJR.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_create_infoJR.stageCount = 2;
		pipeline_create_infoJR.pStages = stage_create_infoJR;
		pipeline_create_infoJR.pInputAssemblyState = &assembly_create_infoJR;
		pipeline_create_infoJR.pVertexInputState = &input_vertex_infoJR;
		pipeline_create_infoJR.pViewportState = &viewport_create_info;
		pipeline_create_infoJR.pRasterizationState = &rasterization_create_infoJR;
		pipeline_create_infoJR.pMultisampleState = &multisample_create_infoJR;
		pipeline_create_infoJR.pDepthStencilState = &depth_stencil_create_infoJR;
		pipeline_create_infoJR.pColorBlendState = &color_blend_create_infoJR;
		pipeline_create_infoJR.pDynamicState = &dynamic_create_infoJR;
		pipeline_create_infoJR.layout = pipelineLayout;
		pipeline_create_infoJR.renderPass = renderPassJR;
		pipeline_create_infoJR.subpass = 0;
		pipeline_create_infoJR.basePipelineHandle = VK_NULL_HANDLE;
		vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1,
			&pipeline_create_infoJR, nullptr, &pipelineJR);

		// TODO: Part 3b
		// TODO: Part 4f
		/***************** CLEANUP / SHUTDOWN ******************/
		// GVulkanSurface will inform us when to release any allocated resources
		shutdown.Create(vlk, [&]() {
			if (+shutdown.Find(GW::GRAPHICS::GVulkanSurface::Events::RELEASE_RESOURCES, true)) {
				CleanUp(); // unlike D3D we must be careful about destroy timing
			}
			});
	}
	void Render()
	{
		// grab the current Vulkan commandBuffer
		unsigned int currentBuffer;
		vlk.GetSwapchainCurrentImage(currentBuffer);
		VkCommandBuffer commandBuffer;
		vlk.GetCommandBuffer(currentBuffer, (void**)&commandBuffer);
		// what is the current client area dimensions?
		unsigned int width, height;
		win.GetClientWidth(width);
		win.GetClientHeight(height);
		// setup the pipeline's dynamic settings
		VkViewport viewport = {
			0, 0, static_cast<float>(width), static_cast<float>(height), 0, 1
		};
		VkRect2D scissor = { {0, 0}, {width, height} };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		// now we can draw
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexHandle, offsets);
		vkCmdDraw(commandBuffer, 10000, 1, 0, 0); // TODO: Part 2b
		// TODO: Part 3b
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineJR);
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexHandleJR, offsets);
		vkCmdDraw(commandBuffer, 11, 1, 0, 0);
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexHandle3rd, offsets);
		vkCmdDraw(commandBuffer, 11, 1, 0, 0);
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexHandle4th, offsets);
		vkCmdDraw(commandBuffer, 11, 1, 0, 0);
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexHandle5th, offsets);
		vkCmdDraw(commandBuffer, 11, 1, 0, 0);
	}
	container StarGen()
	{
		container StarsForged;
		for (int i = 0; i < 20000; i++)
			StarsForged.filler[i] = -1.0f + ((float)rand() / RAND_MAX) * (1.0f - -1.0f);
		return StarsForged;
	}
	VERTEXholder BigStarGen(float x[], float y[], int size)
	{
		VERTEXholder star;
		for (size_t i = 0; i <= 10; i++)
		{
			star.ver[i].X = x[i] / size;
			star.ver[i].Y = y[i] / size;
			star.ver[i].R = rand() / static_cast<float>(RAND_MAX);
			star.ver[i].G = rand() / static_cast<float>(RAND_MAX);
			star.ver[i].B = rand() / static_cast<float>(RAND_MAX);
			star.ver[i].A = 1;
		}
		return star;
	}
private:
	void CleanUp()
	{
		// wait till everything has completed
		vkDeviceWaitIdle(device);
		// Release allocated buffers, shaders & pipeline
		vkDestroyBuffer(device, vertexHandle, nullptr);
		vkFreeMemory(device, vertexData, nullptr);
		// TODO: Part 3a
		vkDestroyBuffer(device, vertexHandleJR, nullptr);
		vkDestroyBuffer(device, vertexHandle3rd, nullptr);
		vkDestroyBuffer(device, vertexHandle4th, nullptr);
		vkDestroyBuffer(device, vertexHandle5th, nullptr);
		vkDestroyShaderModule(device, vertexShader, nullptr);
		vkDestroyShaderModule(device, pixelShader, nullptr);
		// TODO: Part 4b
		vkDestroyShaderModule(device, vertexShaderJR, nullptr);
		vkDestroyShaderModule(device, pixelShaderJR, nullptr);
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		vkDestroyPipeline(device, pipeline, nullptr);
		vkDestroyPipeline(device, pipelineJR, nullptr);
	}
};
