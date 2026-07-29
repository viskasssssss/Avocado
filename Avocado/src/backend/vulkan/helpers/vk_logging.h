#pragma once

#include "pch.h"

#include <vulkan/vulkan.hpp>

namespace avo_vk
{
	VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	)
	{
		AVO_CRITICAL("VK: {0}", pCallbackData->pMessage);
		return VK_FALSE;
	}

	vk::DebugUtilsMessengerEXT make_debug_messenger(vk::Instance& instance, vk::detail::DispatchLoaderDynamic& dldi)
	{
		vk::DebugUtilsMessengerCreateInfoEXT create_info = vk::DebugUtilsMessengerCreateInfoEXT(
			vk::DebugUtilsMessengerCreateFlagsEXT(),
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
			vk::PFN_DebugUtilsMessengerCallbackEXT(debug_callback),
			nullptr
		);

		return instance.createDebugUtilsMessengerEXT(create_info, nullptr, dldi);
	}

	std::vector<std::string> log_transform_bits(vk::SurfaceTransformFlagsKHR bits)
	{
		std::vector<std::string> result;

		if (bits & vk::SurfaceTransformFlagBitsKHR::eIdentity)
		{
			result.push_back("Identity");
		}

		if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate90)
		{
			result.push_back("90 degree rotation");
		}

		if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate180)
		{
			result.push_back("180 degree rotation");
		}

		if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate270)
		{
			result.push_back("270 degree rotation");
		}

		if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirror)
		{
			result.push_back("Horizontal mirror");
		}

		if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate90)
		{
			result.push_back("Horizontal mirror, then 90 degree rotation");
		}

		if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate180)
		{
			result.push_back("Horizontal mirror, then 180 degree rotation");
		}

		if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate270)
		{
			result.push_back("Horizontal mirror, then 270 degree rotation");
		}

		if (bits & vk::SurfaceTransformFlagBitsKHR::eInherit)
		{
			result.push_back("Inherited");
		}

		return result;
	}

	std::vector<std::string> log_alpha_composite_bits(vk::CompositeAlphaFlagsKHR bits)
	{
		std::vector<std::string> result;

		if (bits & vk::CompositeAlphaFlagBitsKHR::eOpaque)
		{
			result.push_back("Opaque (Alpha ignored)");
		}

		if (bits & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied)
		{
			result.push_back("Pre multiplied (Alpha expected to alredy be multiplied in image)");
		}

		if (bits & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied)
		{
			result.push_back("Post multiplied (Aplha will be applied during composition)");
		}

		if (bits & vk::CompositeAlphaFlagBitsKHR::eInherit)
		{
			result.push_back("Inherited");
		}

		return result;
	}

	std::vector<std::string> log_image_usage_bits(vk::ImageUsageFlags bits)
	{
		std::vector<std::string> result;

		if (bits & vk::ImageUsageFlagBits::eTransferSrc)
		{
			result.push_back("Transfer src");
		}

		if (bits & vk::ImageUsageFlagBits::eTransferDst)
		{
			result.push_back("Transfer Dst");
		}

		if (bits & vk::ImageUsageFlagBits::eStorage)
		{
			result.push_back("Storage");
		}

		if (bits & vk::ImageUsageFlagBits::eColorAttachment)
		{
			result.push_back("Color Attachment");
		}

		if (bits & vk::ImageUsageFlagBits::eDepthStencilAttachment)
		{
			result.push_back("Depth/Stencil Attachment");
		}

		if (bits & vk::ImageUsageFlagBits::eTransientAttachment)
		{
			result.push_back("Transient Attachment");
		}

		if (bits & vk::ImageUsageFlagBits::eInputAttachment)
		{
			result.push_back("Input Attachment");
		}

		if (bits & vk::ImageUsageFlagBits::eFragmentDensityMapEXT)
		{
			result.push_back("Fragment Density Map");
		}

		if (bits & vk::ImageUsageFlagBits::eFragmentShadingRateAttachmentKHR)
		{
			result.push_back("Fragment Shading Rate Attachment");
		}

		return result;
	}

	std::string log_present_mode(vk::PresentModeKHR present_mode)
	{
		if (present_mode == vk::PresentModeKHR::eImmediate)
		{
			return "Immediate";
		}

		if (present_mode == vk::PresentModeKHR::eMailbox)
		{
			return "Mailbox";
		}

		if (present_mode == vk::PresentModeKHR::eFifo)
		{
			return "Fifo";
		}

		if (present_mode == vk::PresentModeKHR::eFifoRelaxed)
		{
			return "Fifo Relaxed";
		}

		if (present_mode == vk::PresentModeKHR::eSharedDemandRefresh)
		{
			return "Shared Demand Refresh";
		}

		if (present_mode == vk::PresentModeKHR::eSharedContinuousRefresh)
		{
			return "Shared Continuous Refresh";
		}

		return "None / Undefined";
	}
}