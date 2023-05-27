#include "../FXE_FrameCreation.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <iostream>
#include <optional>
#include <chrono>

#include "../FXE_Window.h"
#include "../FXE_GraphicPipeline.h"
#include "../FXE_VertexBuffer.h"
#include "../FXE_TextureImage.h"
#include "../FXE_ExtraFunctions.h"

#define TIME(x) x = std::chrono::high_resolution_clock::now();
#define PRINT_TIME_NS(text, start, end) std::cout << text << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << " ns" << std::endl;
#define PRINT_TIME_MS(text, start, end) std::cout << text << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;

/*---------------------------------*/
/*----------Main Methods-----------*/
/*---------------------------------*/

//initializing the pointers pointing to the window and graphics class
void FXEFrameCreation::init_FrameCreation(VkDevice device, VkPhysicalDevice physicalDevice, FXEWindow* theWindow, FXEGraphicPipeline* theGraphicPipeline, FXEVertexBuffer* theVertexBuffer, FXETextureImage* theTextureImage)
{
    Device = device;
    PhysicalDevice = physicalDevice;

    TheWindowPtr = theWindow;
    TheGraphicPipelinePtr = theGraphicPipeline;
    TheVertexBufferPtr = theVertexBuffer;
    TheTextureImagePtr = theTextureImage;
}

//cleaning up the swap chain at the end of the program
void FXEFrameCreation::cleanup_SwapChain()
{
    for (VkFramebuffer FrameBuffer : SwapChainFramebuffers)
    {
        vkDestroyFramebuffer(Device, FrameBuffer, nullptr);
    }

    for (VkImageView ImageView : SwapChainImageViews)
    {
        vkDestroyImageView(Device, ImageView, nullptr);
    }

    vkDestroySwapchainKHR(Device, SwapChain, nullptr);
}

//cleaning up the semaphores at the end of the program
void FXEFrameCreation::cleanup_Semaphores()
{
    for (uint32_t i = 0; i < MaxFramesInFlight; i++)
    {
        vkDestroySemaphore(Device, ImageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(Device, RenderFinishedSemaphores[i], nullptr);
        vkDestroyFence(Device, InFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(Device, CommandPool, nullptr);
}

void FXEFrameCreation::cleanup_DepthImages()
{
    vkDestroyImageView(Device, DepthImageView, nullptr);
    vkDestroyImage(Device, DepthImage, nullptr);
    vkFreeMemory(Device, DepthImageMemory, nullptr);
}

//Method that draws the frame on the screen
void FXEFrameCreation::draw_Frame(VkQueue presentQueue, VkQueue graphicsQueue)
{
    vkWaitForFences(Device, 1, &InFlightFences[CurrentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(Device, SwapChain, UINT64_MAX, ImageAvailableSemaphores[CurrentFrame], VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        TIME(auto start)
            recreate_SwapChain(graphicsQueue);
        TIME(auto end)
            PRINT_TIME_MS("time to recreate: ", start, end)
            return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    update_UniformBuffer(CurrentFrame);
    vkResetFences(Device, 1, &InFlightFences[CurrentFrame]);
    vkResetCommandBuffer(CommandBuffers[CurrentFrame], 0);
    record_CommandBuffer(CommandBuffers[CurrentFrame], imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphore[] = { ImageAvailableSemaphores[CurrentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphore;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &CommandBuffers[CurrentFrame];

    VkSemaphore signalSemaphores[] = { RenderFinishedSemaphores[CurrentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, InFlightFences[CurrentFrame]) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[] = { SwapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    result = vkQueuePresentKHR(presentQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || FramebufferRezised)
    {
        FramebufferRezised = false;

        TIME(auto start);
        recreate_SwapChain(graphicsQueue);
        TIME(auto end);
        PRINT_TIME_MS("time to recreate: ", start, end)
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image");
    }

    CurrentFrame = (CurrentFrame + 1) % MaxFramesInFlight;
}

/*---------------------------------*/
/*---------Public Methods----------*/
/*---------------------------------*/

//creates a working swap chain with all preferred settings
void FXEFrameCreation::create_SwapChain()
{
    FXE::SwapChainSupportDetails swapChainSupport = FXE::query_SwapChainSupport(PhysicalDevice, TheWindowPtr->Surface);

    VkSurfaceFormatKHR surfaceFormat = choose_SwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = choose_SwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = choose_SwapExtent(swapChainSupport.capabilities, TheWindowPtr->Window);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = TheWindowPtr->Surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    FXE::QueueFamilyIndices indices = FXE::find_QueueFamilies(PhysicalDevice, TheWindowPtr->Surface);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(Device, &createInfo, nullptr, &SwapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(Device, SwapChain, &imageCount, nullptr);
    SwapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(Device, SwapChain, &imageCount, SwapChainImages.data());

    SwapChainImageFormat = surfaceFormat.format;
    SwapChainExtent = extent;
}

void FXEFrameCreation::create_ImageViews()
{
    SwapChainImageViews.resize(SwapChainImages.size());

    for (size_t i = 0; i < SwapChainImages.size(); i++)
    {
        SwapChainImageViews[i] = FXE::create_ImageView(Device, SwapChainImages[i], SwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    }
}

void FXEFrameCreation::create_FrameBuffer()
{
    SwapChainFramebuffers.resize(SwapChainImageViews.size());

    for (size_t i = 0; i < SwapChainImageViews.size(); i++)
    {
        std::array<VkImageView, 2> attachments = {
           SwapChainImageViews[i], DepthImageView
        };


        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = TheGraphicPipelinePtr->RenderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = SwapChainExtent.width;
        framebufferInfo.height = SwapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(Device, &framebufferInfo, nullptr, &SwapChainFramebuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void FXEFrameCreation::create_CommandPool()
{
    FXE::QueueFamilyIndices indices = FXE::find_QueueFamilies(PhysicalDevice, TheWindowPtr->Surface);

    VkCommandPoolCreateInfo commandPoolInfo{};
    commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolInfo.queueFamilyIndex = indices.graphicsFamily.value();

    if (vkCreateCommandPool(Device, &commandPoolInfo, nullptr, &CommandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }
}

void FXEFrameCreation::create_DepthResources(VkQueue graphicsQueue)
{
    VkFormat depthFormat = FXE::find_DepthFormat(PhysicalDevice);

    FXE::create_Image(Device, PhysicalDevice, SwapChainExtent.width, SwapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, DepthImage, DepthImageMemory);

    DepthImageView = FXE::create_ImageView(Device, DepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

    TheTextureImagePtr->transition_ImageLayout(graphicsQueue, DepthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

void FXEFrameCreation::create_CommandBuffer()
{
    CommandBuffers.resize(MaxFramesInFlight);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(CommandBuffers.size());

    if (vkAllocateCommandBuffers(Device, &allocInfo, CommandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffer!");
    }
}

void FXEFrameCreation::create_SyncObjects()
{
    ImageAvailableSemaphores.resize(MaxFramesInFlight);
    RenderFinishedSemaphores.resize(MaxFramesInFlight);
    InFlightFences.resize(MaxFramesInFlight);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (uint32_t i = 0; i < MaxFramesInFlight; i++)
    {
        if (vkCreateSemaphore(Device, &semaphoreInfo, nullptr, &ImageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(Device, &semaphoreInfo, nullptr, &RenderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(Device, &fenceInfo, nullptr, &InFlightFences[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create semaphores!");
        }
    }
}

uint32_t FXEFrameCreation::get_MaxFramesInFlight() const
{
    return MaxFramesInFlight;
}

/*---------------------------------*/
/*--------Private Methods----------*/
/*---------------------------------*/

//recreating the swap chain
void FXEFrameCreation::recreate_SwapChain(VkQueue graphicsQueue)
{
    TheWindowPtr->windowMinimized();

    vkDeviceWaitIdle(Device);

    cleanup_SwapChain();

    create_SwapChain();
    create_ImageViews();
    create_DepthResources(graphicsQueue);
    create_FrameBuffer();
}

void FXEFrameCreation::record_CommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = TheGraphicPipelinePtr->RenderPass;
    renderPassBeginInfo.framebuffer = SwapChainFramebuffers[imageIndex];
    renderPassBeginInfo.renderArea.offset = { 0, 0 };
    renderPassBeginInfo.renderArea.extent = SwapChainExtent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };

    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, TheGraphicPipelinePtr->GraphicsPipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(SwapChainExtent.width);
    viewport.height = static_cast<float>(SwapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = SwapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkBuffer vertexBuffers[] = { TheVertexBufferPtr->VertexBuffer };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, TheVertexBufferPtr->IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, TheGraphicPipelinePtr->PipelineLayout, 0, 1, 
        &TheVertexBufferPtr->DescriptorSets[CurrentFrame], 0, nullptr);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(TheVertexBufferPtr->Indices.size()), 1, 0, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void FXEFrameCreation::update_UniformBuffer(uint32_t currentImage)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    FXE::UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), static_cast<float>(SwapChainExtent.width) / static_cast<float>(SwapChainExtent.height), 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    memcpy(TheVertexBufferPtr->UniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

//Checks for if swap chain supports best possible surface format, else takes the first surface format available
VkSurfaceFormatKHR FXEFrameCreation::choose_SwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableSurfaceFormats)
{
    for (const auto& availableFormat : availableSurfaceFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return availableSurfaceFormats[0];
}

//Checks for if swap chain supports best possible present mode, else takes the FIFO present mode that is always available
VkPresentModeKHR FXEFrameCreation::choose_SwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D FXEFrameCreation::choose_SwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window)
{
	if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)())
    {
        return capabilities.currentExtent;
    }
  
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    VkExtent2D actualExtent = {
    	static_cast<uint32_t>(width),
    	static_cast<uint32_t>(height)
    };

    actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return actualExtent;
}
