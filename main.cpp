
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <vulkan/vulkan_core.h>

#include "glm_config.h"

#define GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_NONE
#include <vulkan/vulkan.h>

#include <GLFW/glfw3.h>

#include "context.h"
#include "imgui_integration.h"
#include "pipeline_simple.h"
#include "swapchain.h"
#include "wrappers.h"

#include <imgui.h>

class Application {
public:
    Application(const std::string& name, bool useValidation)
        : m_name(name)
        , m_context(name, useValidation)
    {
    }

    bool CheckGLFW()
    {
        if (glfwVulkanSupported()) {
            printf("Failed to look up minimal Vulkan loader/ICD\n!");
            return false;
        }

        if (!glfwInit()) {
            printf("Failed to init GLFW!\n");
            return false;
        }

        return true;
    }

    void CreateWindow(uint32_t width, uint32_t height)
    {
        // Some glfw requirements
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_width  = width;
        m_height = height;
        m_window = glfwCreateWindow(m_width, m_height, m_name.c_str(), NULL, NULL);

        glfwSetWindowUserPointer(m_window, this);
        glfwSetKeyCallback(m_window, Application::KeyCallback);
        m_imgui.Init(m_window);
    }

    void InitVulkan()
    {
        // Create Vulkan context
        uint32_t     count          = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&count);

        printf("Minimal set of required extension by GLFW:\n");
        for (uint32_t idx = 0; idx < count; idx++) {
            printf("-> %s\n", glfwExtensions[idx]);
        }

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + count);
        m_context.CreateInstance({}, extensions);

        // Create Render Surface

        // We have the window, the instance, create a surface from the window to draw onto.
        // Create a Vulkan Surface using GLFW.
        // By using GLFW the current windowing system's surface is created (xcb, win32, etc..)
        VkSurfaceKHR surface             = VK_NULL_HANDLE;
        VkResult     surfaceCreateResult = glfwCreateWindowSurface(m_context.instance(), m_window, NULL, &surface);
        if (surfaceCreateResult != VK_SUCCESS) {
            // TODO: not the best, but will fail the application surely
            throw std::runtime_error("Failed to create window surface!");
        }

        // Create Device
        std::vector<const char*> deviceExtensions{};

        VkPhysicalDevice phyDevice = m_context.SelectPhysicalDevice(surface);
        VkDevice         device    = m_context.CreateDevice(deviceExtensions);

        // Create Swapchain
        m_swapchain = std::unique_ptr<Swapchain>(
            new Swapchain(m_context.instance(), phyDevice, device, surface, {m_width, m_height}));
        VkResult swapchainCreated = m_swapchain->Create();
        assert(swapchainCreated == VK_SUCCESS);

        m_cmdPool    = CreateCommandPool(device, m_context.queueFamilyIdx(), "baseCmdPool");
        m_cmdBuffers = AllocateCommandBuffers(device, m_cmdPool, m_swapchain->images().size(), "baseCmdBuffer");

        // Some sync object
        m_imageFence       = CreateFence(device, "imageFence");
        m_presentSemaphore = CreateSemaphore(device, "presentSemaphore");

        // Init imgui
        m_imgui.CreateContext(m_context, m_swapchain->format());

        // Set dynamic viewport/scissor infos
        m_viewport = {
            .x        = 0,
            .y        = 0,
            .width    = (float)m_swapchain->surfaceExtent().width,
            .height   = (float)m_swapchain->surfaceExtent().height,
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };

        m_scissor = {
            .offset = {0, 0},
            .extent = m_swapchain->surfaceExtent(),
        };
    }

    void Destroy()
    {
        // TODO: destroy all resources
        VkDevice device = m_context.device();
        vkDestroyCommandPool(device, m_cmdPool, nullptr);
        vkDestroyFence(device, m_imageFence, nullptr);
        vkDestroySemaphore(device, m_presentSemaphore, nullptr);

        m_imgui.Destroy(m_context);

        m_swapchain->Destroy();
        m_context.Destroy();

        glfwDestroyWindow(m_window);
    }

    void ShowWindow() { glfwShowWindow(m_window); }

    void RunLoop();

private:
    static void KeyCallback(GLFWwindow* window, int key, int /*scancode*/, int /*action*/, int /*mods*/)
    {
        Application* userPtr = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
        if (userPtr) {
            userPtr->HandleKey(key);
        }
    }

    void HandleKey(int key)
    {
        bool imguiIgnore = !m_imgui.WantCaptureKeyboard();

        if (imguiIgnore) {
            switch (key) {
            case GLFW_KEY_ESCAPE: {
                glfwSetWindowShouldClose(m_window, GLFW_TRUE);
                break;
            }
            default:
                break;
            }
        }
    }

    std::string m_name;
    Context     m_context;
    // Window info
    uint32_t    m_width  = 0;
    uint32_t    m_height = 0;
    GLFWwindow* m_window = nullptr;
    // IMGUI
    IMGUIIntegration m_imgui;

    // Vulkan resources
    std::unique_ptr<Swapchain>   m_swapchain        = nullptr;
    VkCommandPool                m_cmdPool          = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> m_cmdBuffers       = {};
    VkFence                      m_imageFence       = VK_NULL_HANDLE;
    VkSemaphore                  m_presentSemaphore = VK_NULL_HANDLE;
    VkViewport                   m_viewport         = {};
    VkRect2D                     m_scissor          = {};
};

void Application::RunLoop()
{
    uint32_t queueFamilyIdx = m_context.queueFamilyIdx();
    VkQueue  queue          = m_context.queue();
    VkDevice device         = m_context.device();

    PipelineSimple pipelineSimple("simple");
    pipelineSimple.Create(device, m_swapchain->format());

    int32_t color = 0;
    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();
        {
            m_imgui.NewFrame();

            // ImGui::ShowDemoWindow();
            ImGui::Begin("Info");

            static bool colorAutoInc = true;
            ImGui::Checkbox("Use auto increment", &colorAutoInc);

            if (colorAutoInc) {
                color = (color + 1) % 255;
            }

            ImGuiIO& io = ImGui::GetIO();

            ImGui::SliderInt("Red value", &color, 0, 255);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
            ImGui::Render();
        }

        // Get new image to render to
        vkResetFences(m_context.device(), 1, &m_imageFence);
        const Swapchain::Image& swapchainImage = m_swapchain->AquireNextImage(m_imageFence);
        vkWaitForFences(m_context.device(), 1, &m_imageFence, VK_TRUE, UINT64_MAX);

        // Get command buffer based on swapchain image index
        VkCommandBuffer cmdBuffer = m_cmdBuffers[swapchainImage.idx];
        {
            // Begin command buffer record
            const VkCommandBufferBeginInfo beginInfo = {
                .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .pNext            = nullptr,
                .flags            = 0,
                .pInheritanceInfo = nullptr,
            };
            vkBeginCommandBuffer(cmdBuffer, &beginInfo);

            m_swapchain->CmdTransitionToRender(cmdBuffer, swapchainImage, queueFamilyIdx);

            // Begin render commands
            const VkClearValue                 clearColor      = {{{color / 255.0f, 0.0f, 0.0f, 1.0f}}};
            const VkRenderingAttachmentInfoKHR colorAttachment = {
                .sType              = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
                .pNext              = nullptr,
                .imageView          = swapchainImage.view,
                .imageLayout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                .resolveMode        = VK_RESOLVE_MODE_NONE,
                .resolveImageView   = VK_NULL_HANDLE,
                .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .loadOp             = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp            = VK_ATTACHMENT_STORE_OP_STORE,
                .clearValue         = clearColor,
            };
            const VkRenderingInfoKHR renderInfo = {
                .sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
                .pNext = nullptr,
                .flags = 0,
                .renderArea =
                    {
                        .offset = {0, 0},
                        .extent = {m_width, m_height},
                    },
                .layerCount           = 1,
                .viewMask             = 0,
                .colorAttachmentCount = 1,
                .pColorAttachments    = &colorAttachment,
                .pDepthAttachment     = nullptr,
                .pStencilAttachment   = nullptr,
            };
            vkCmdBeginRendering(cmdBuffer, &renderInfo);

            {
                glm::mat4 mvp = glm::mat4(1.0f);

                vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineSimple.pipeline());
                vkCmdSetScissor(cmdBuffer, 0, 1, &m_scissor);
                vkCmdSetViewport(cmdBuffer, 0, 1, &m_viewport);
                vkCmdPushConstants(cmdBuffer, pipelineSimple.layout(), VK_SHADER_STAGE_ALL, 0, sizeof(mvp), &mvp);
                vkCmdDraw(cmdBuffer, 3, 1, 0, 0);
            }

            // Render things
            m_imgui.Draw(cmdBuffer);

            // End render commands
            vkCmdEndRendering(cmdBuffer);

            // Finish up recording
            m_swapchain->CmdTransitionToPresent(cmdBuffer, swapchainImage, queueFamilyIdx);
            vkEndCommandBuffer(cmdBuffer);
        }

        // Execute recorded commands
        const VkSubmitInfo submitInfo = {
            .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext                = nullptr,
            .waitSemaphoreCount   = 0,
            .pWaitSemaphores      = nullptr,
            .pWaitDstStageMask    = nullptr,
            .commandBufferCount   = 1,
            .pCommandBuffers      = &cmdBuffer,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores    = &m_presentSemaphore,
        };
        vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);

        // Present current image
        m_swapchain->QueuePresent(queue, m_presentSemaphore);

        vkDeviceWaitIdle(device);
    }

    pipelineSimple.Destroy(device);
}

int main()
{
    Application app("Demo", true);

    if (!app.CheckGLFW()) {
        return -1;
    }

    app.CreateWindow(800, 600);
    app.InitVulkan();

    app.ShowWindow();

    app.RunLoop();

    app.Destroy();

    return 0;
}
