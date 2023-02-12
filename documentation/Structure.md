## Rocket Game Engine
1. main.cpp - initilizes TutorialApp and calls run() function on it
2. tutorial_app - WIDTH, HEIGHT, run() function and reference to RocketWindow
	1.  pointer to rocketWindow
	2. pointer to rocketDevice (rocketWidnow passed in contructor)
	3. pointer to rocketPipeline (device passed in contructor)
3. rocket_window - contrustor, destructor, height, width, windowName and reference to GLFWEwindow
4. rocket_pipeline 
	1. PipelineConfigInfo - used to configure each step of the graphics pipeline
	2. defaultPipelineConfiginfo - default config (without shaders, render pass and pipelineLayout)
	3. createGrahpicsPipeline - creates shaders and sets values in pipeline config for shaders
5. rocket_device - initializes all need Vulkan code including phisical and logical device, vaidaltion layers, surface, command pool...