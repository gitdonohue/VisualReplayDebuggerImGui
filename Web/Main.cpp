﻿//#include <stdio.h>
//#include <emscripten.h>
//
//int main() {
//	printf("Hello, world!");
//	return 0;
//}

// Dear ImGui: standalone example application for Emscripten, using GLFW + WebGPU
// (Emscripten is a C++-to-javascript compiler, used to publish executables for the web. See https://emscripten.org/)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_wgpu.h"
#include <stdio.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/html5_webgpu.h>
#include <GLFW/glfw3.h>
#include <webgpu/webgpu.h>
#include <webgpu/webgpu_cpp.h>
#include <fstream>

#include <src/ReplayData.hpp>
#include <src/ReplayContext.hpp>
#include <src/ReplayLogsWindow.hpp>
#include <src/ReplayEntitiesWindow.hpp>
#include <src/ReplayTimelineSlider.hpp>
#include <src/ReplayViewportWindow.hpp>

// Global WebGPU required states
static WGPUDevice    wgpu_device = nullptr;
static WGPUSurface   wgpu_surface = nullptr;
static WGPUSwapChain wgpu_swap_chain = nullptr;
static int           wgpu_swap_chain_width = 0;
static int           wgpu_swap_chain_height = 0;

// Forward declarations
static void MainLoopStep(void* window);
static bool InitWGPU();
static void print_glfw_error(int error, const char* description);
static void print_wgpu_error(WGPUErrorType error_type, const char* message, void*);

EM_JS(int, canvas_get_width, (), { return Module.canvas.width; });
EM_JS(int, canvas_get_height, (), { return Module.canvas.height; });
EM_JS(void, resizeCanvas, (), { js_resizeCanvas(); });

static VisualReplayDebugger::ReplayData s_replayData;
static VisualReplayDebugger::ReplayContext* s_pReplayContext = nullptr;
static VisualReplayDebugger::ReplayLogsWindow* s_pLogsWindow = nullptr;
static VisualReplayDebugger::ReplayEntitiesWindow* s_entitiesWindow = nullptr;
static VisualReplayDebugger::ReplayTimelineWindow* s_timelineWindow = nullptr;
static VisualReplayDebugger::ReplayViewportWindow* s_viewport = nullptr;

// Main code
int main(int, char**)
{
    std::ifstream ifs("sample.vrd", std::ifstream::in | std::ifstream::app | std::ifstream::binary);
    s_replayData.Read(ifs);
    s_pReplayContext = new VisualReplayDebugger::ReplayContext(s_replayData);
    s_pLogsWindow = new VisualReplayDebugger::ReplayLogsWindow(*s_pReplayContext);
    s_entitiesWindow = new VisualReplayDebugger::ReplayEntitiesWindow(*s_pReplayContext);
    s_timelineWindow = new VisualReplayDebugger::ReplayTimelineWindow(*s_pReplayContext);
    s_viewport = new VisualReplayDebugger::ReplayViewportWindow(*s_pReplayContext);

    glfwSetErrorCallback(print_glfw_error);
    if (!glfwInit())
        return 1;

    // Make sure GLFW does not initialize any graphics context.
    // This needs to be done explicitly later.
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+WebGPU example", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return 1;
    }

    // Initialize the WebGPU environment
    if (!InitWGPU())
    {
        if (window)
            glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    glfwShowWindow(window);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    //ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOther(window, true);
    ImGui_ImplWGPU_Init(wgpu_device, 3, WGPUTextureFormat_RGBA8Unorm, WGPUTextureFormat_Undefined);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Emscripten allows preloading a file or folder to be accessible at runtime. See Makefile for details.
    //io.Fonts->AddFontDefault();
#ifndef IMGUI_DISABLE_FILE_FUNCTIONS
    io.Fonts->AddFontFromFileTTF("fonts/segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("fonts/ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);
#endif

    // This function will directly return and exit the main function.
    // Make sure that no required objects get cleaned up.
    // This way we can use the browsers 'requestAnimationFrame' to control the rendering.
    emscripten_set_main_loop_arg(MainLoopStep, window, 0, false);

    return 0;
}

static bool InitWGPU()
{
    wgpu_device = emscripten_webgpu_get_device();
    if (!wgpu_device)
        return false;

    wgpuDeviceSetUncapturedErrorCallback(wgpu_device, print_wgpu_error, nullptr);

    // Use C++ wrapper due to misbehavior in Emscripten.
    // Some offset computation for wgpuInstanceCreateSurface in JavaScript
    // seem to be inline with struct alignments in the C++ structure
    wgpu::SurfaceDescriptorFromCanvasHTMLSelector html_surface_desc = {};
    html_surface_desc.selector = "#canvas";

    wgpu::SurfaceDescriptor surface_desc = {};
    surface_desc.nextInChain = &html_surface_desc;

    // Use 'null' instance
    wgpu::Instance instance = wgpu::CreateInstance();
    wgpu_surface = instance.CreateSurface(&surface_desc).Release();

    return true;
}

static void MainLoopStep(void* window)
{
    ImGuiIO& io = ImGui::GetIO();

    glfwPollEvents();

    int canvas_width = canvas_get_width();
    int canvas_height = canvas_get_height();
    glfwSetWindowSize((GLFWwindow*)window, canvas_width, canvas_height);
    //printf("Canvas Size: %d, %d\n", canvas_width, canvas_height);
    ImGui::SetCurrentContext(ImGui::GetCurrentContext());

    int width, height;
    glfwGetFramebufferSize((GLFWwindow*)window, &width, &height);

    //printf("glfwGetFramebufferSize: %d, %d\n", width, height);

    // React to changes in screen size
    if (width != wgpu_swap_chain_width && height != wgpu_swap_chain_height)
    {
        ImGui_ImplWGPU_InvalidateDeviceObjects();
        if (wgpu_swap_chain)
            wgpuSwapChainRelease(wgpu_swap_chain);
        wgpu_swap_chain_width = width;
        wgpu_swap_chain_height = height;
        WGPUSwapChainDescriptor swap_chain_desc = {};
        swap_chain_desc.usage = WGPUTextureUsage_RenderAttachment;
        swap_chain_desc.format = WGPUTextureFormat_RGBA8Unorm;
        swap_chain_desc.width = width;
        swap_chain_desc.height = height;
        swap_chain_desc.presentMode = WGPUPresentMode_Fifo;
        wgpu_swap_chain = wgpuDeviceCreateSwapChain(wgpu_device, wgpu_surface, &swap_chain_desc);
        ImGui_ImplWGPU_CreateDeviceObjects();
    }

    // Start the Dear ImGui frame
    ImGui_ImplWGPU_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Our state
    // (we use static, which essentially makes the variable globals, as a convenience to keep the example code easy to follow)
    static bool show_demo_window = true;
    //static bool show_another_window = false;
    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);
    /*
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                                // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");                     // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);            // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);                  // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color);       // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                                  // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }
    */
        //// 3. Show another simple window.
        //if (s_pLogsWindow != nullptr)
        //{
        //    ImGui::Begin("Another Window");
        //    ImGui::Text("Hello from another window!");
        //    s_pLogsWindow->Draw();
        //    ImGui::End();
        //}

    s_timelineWindow->Draw();
    s_entitiesWindow->Draw();
    s_pLogsWindow->Draw();
    s_viewport->Draw();

    // Rendering
    ImGui::Render();

    WGPURenderPassColorAttachment color_attachments = {};
    color_attachments.loadOp = WGPULoadOp_Clear;
    color_attachments.storeOp = WGPUStoreOp_Store;
    color_attachments.clearValue = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
    color_attachments.view = wgpuSwapChainGetCurrentTextureView(wgpu_swap_chain);
    WGPURenderPassDescriptor render_pass_desc = {};
    render_pass_desc.colorAttachmentCount = 1;
    render_pass_desc.colorAttachments = &color_attachments;
    render_pass_desc.depthStencilAttachment = nullptr;

    WGPUCommandEncoderDescriptor enc_desc = {};
    WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(wgpu_device, &enc_desc);

    WGPURenderPassEncoder pass = wgpuCommandEncoderBeginRenderPass(encoder, &render_pass_desc);
    ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), pass);
    wgpuRenderPassEncoderEnd(pass);

    WGPUCommandBufferDescriptor cmd_buffer_desc = {};
    WGPUCommandBuffer cmd_buffer = wgpuCommandEncoderFinish(encoder, &cmd_buffer_desc);
    WGPUQueue queue = wgpuDeviceGetQueue(wgpu_device);
    wgpuQueueSubmit(queue, 1, &cmd_buffer);
}

static void print_glfw_error(int error, const char* description)
{
    printf("GLFW Error %d: %s\n", error, description);
}

static void print_wgpu_error(WGPUErrorType error_type, const char* message, void*)
{
    const char* error_type_lbl = "";
    switch (error_type)
    {
    case WGPUErrorType_Validation:  error_type_lbl = "Validation"; break;
    case WGPUErrorType_OutOfMemory: error_type_lbl = "Out of memory"; break;
    case WGPUErrorType_Unknown:     error_type_lbl = "Unknown"; break;
    case WGPUErrorType_DeviceLost:  error_type_lbl = "Device lost"; break;
    default:                        error_type_lbl = "Unknown";
    }
    printf("%s error: %s\n", error_type_lbl, message);
}
