#pragma once

namespace cgb
{
	/** Represents one specific native image format for the Vulkan context */
	struct image_format
	{
		image_format() noexcept;
		image_format(const vk::Format& pFormat) noexcept;
		image_format(const vk::SurfaceFormatKHR& pSrfFrmt) noexcept;

		static image_format default_depth_format() noexcept;
		static image_format default_depth_stencil_format() noexcept;

		vk::Format mFormat;
	};

	/** Analyze the given `cgb::image_usage` flags, and assemble some (hopefully valid) `vk::ImageUsageFlags`, and determine `vk::ImageLayout` and `vk::ImageTiling`. */
	extern std::tuple<vk::ImageUsageFlags, vk::ImageLayout, vk::ImageTiling, vk::ImageCreateFlags> determine_usage_layout_tiling_flags_based_on_image_usage(image_usage _ImageUsageFlags);

	/** Represents an image and its associated memory
	 */
	class image_t
	{
	public:
		image_t() = default;
		image_t(const image_t&) = delete;
		image_t(image_t&&) = default;
		image_t& operator=(const image_t&) = delete;
		image_t& operator=(image_t&&) = default;
		~image_t() = default;

		/** Get the config which is used to created this image with the API. */
		const auto& config() const { return mInfo; }
		/** Get the config which is used to created this image with the API. */
		auto& config() { return mInfo; }
		/** Gets the image handle. */
		const auto& image_handle() const { return mImage.get(); }
		/** Gets the handle to the image's memory. */
		const auto& memory_handle() const { return mMemory.get(); }
		/** Gets the width of the image */
		uint32_t width() const { return config().extent.width; }
		/** Gets the height of the image */
		uint32_t height() const { return config().extent.height; }
		/** Gets the depth of the image */
		uint32_t depth() const { return config().extent.depth; }
		/** Gets the format of the image */
		image_format format() const { return image_format{ config().format }; }

		/** Gets this image's target layout as specified during image creation. */
		const auto& target_layout() const { return mTargetLayout; }

		/** Sets the current image layout */
		void set_current_layout(vk::ImageLayout _NewLayout) { mCurrentLayout = _NewLayout; }
		/** Gets the current image layout */
		const auto& current_layout() const { return mCurrentLayout; }

		/** Gets the usage config flags as specified during image creation. */
		const auto& usage_config() const { return mImageUsage; }

		/** Creates a new image
		 *	@param	pWidth						The width of the image to be created
		 *	@param	pHeight						The height of the image to be created
		 *	@param	pFormat						The image format of the image to be created
		 *	@param	pUseMipMaps					Whether or not MIP maps shall be created for this image. Specifying `true` will set the maximum number of MIP map images.
		 *	@param	pMemoryUsage				Where the memory of the image shall be allocated (GPU or CPU) and how it is going to be used.
		 *	@param	pImageUsage					How this image is intended to being used.
		 *	@param	pNumLayers					How many layers the image to be created shall contain.
		 *	@param	pAlterConfigBeforeCreation	A context-specific function which allows to modify the `vk::ImageCreateInfo` just before the image will be created. Use `.config()` to access the configuration structure!
		 *	@return	Returns a newly created image.
		 */
		static owning_resource<image_t> create(uint32_t pWidth, uint32_t pHeight, image_format pFormat, bool pUseMipMaps = false, int pNumLayers = 1, memory_usage pMemoryUsage = memory_usage::device, image_usage pImageUsage = image_usage::versatile_image, context_specific_function<void(image_t&)> pAlterConfigBeforeCreation = {});

		/** Creates a new image
		*	@param	pWidth						The width of the depth buffer to be created
		*	@param	pHeight						The height of the depth buffer to be created
		*	@param	pFormat						The image format of the image to be created, or a default depth format if not specified.
		*	@param	pMemoryUsage				Where the memory of the image shall be allocated (GPU or CPU) and how it is going to be used.
		*	@param	pUseMipMaps					Whether or not MIP maps shall be created for this image. Specifying `true` will set the maximum number of MIP map images.
		*	@param	pNumLayers					How many layers the image to be created shall contain.
		*	@param	pAlterConfigBeforeCreation	A context-specific function which allows to modify the `vk::ImageCreateInfo` just before the image will be created. Use `.config()` to access the configuration structure!
		*	@return	Returns a newly created depth buffer.
		*/
		static owning_resource<image_t> create_depth(uint32_t pWidth, uint32_t pHeight, std::optional<image_format> pFormat = std::nullopt, bool pUseMipMaps = false, int pNumLayers = 1,  memory_usage pMemoryUsage = memory_usage::device, image_usage pImageUsage = image_usage::read_only_depth_stencil_attachment, context_specific_function<void(image_t&)> pAlterConfigBeforeCreation = {});

		/** Creates a new image
		*	@param	pWidth						The width of the depth+stencil buffer to be created
		*	@param	pHeight						The height of the depth+stencil buffer to be created
		*	@param	pFormat						The image format of the image to be created, or a default depth format if not specified.
		*	@param	pMemoryUsage				Where the memory of the image shall be allocated (GPU or CPU) and how it is going to be used.
		*	@param	pUseMipMaps					Whether or not MIP maps shall be created for this image. Specifying `true` will set the maximum number of MIP map images.
		*	@param	pNumLayers					How many layers the image to be created shall contain.
		*	@param	pAlterConfigBeforeCreation	A context-specific function which allows to modify the `vk::ImageCreateInfo` just before the image will be created. Use `.config()` to access the configuration structure!
		*	@return	Returns a newly created depth+stencil buffer.
		*/
		static owning_resource<image_t> create_depth_stencil(uint32_t pWidth, uint32_t pHeight, std::optional<image_format> pFormat = std::nullopt, bool pUseMipMaps = false, int pNumLayers = 1,  memory_usage pMemoryUsage = memory_usage::device, image_usage pImageUsage = image_usage::read_only_depth_stencil_attachment, context_specific_function<void(image_t&)> pAlterConfigBeforeCreation = {});

		// TODO: What to do with this one: ??
		vk::ImageMemoryBarrier create_barrier(vk::AccessFlags pSrcAccessMask, vk::AccessFlags pDstAccessMask, vk::ImageLayout pOldLayout, vk::ImageLayout pNewLayout, std::optional<vk::ImageSubresourceRange> pSubresourceRange = std::nullopt) const;

	private:
		// The memory handle. This member will contain a valid handle only after successful image creation.
		vk::UniqueDeviceMemory mMemory;
		// The image create info which contains all the parameters for image creation
		vk::ImageCreateInfo mInfo;
		// The image handle. This member will contain a valid handle only after successful image creation.
		vk::UniqueImage mImage;
		// The image's target layout
		vk::ImageLayout mTargetLayout;
		// The current image layout
		vk::ImageLayout mCurrentLayout;
		// The image_usage flags specified during creation
		image_usage mImageUsage;
	};

	/** Typedef representing any kind of OWNING image representations. */
	using image	= owning_resource<image_t>;

	/** Compares two `image_t`s for equality.
	 *	They are considered equal if all their handles (image, memory) are the same.
	 *	The config structs' data is not evaluated for equality comparison.
	 */
	static bool operator==(const image_t& left, const image_t& right)
	{
		return left.image_handle() == right.image_handle()
			&& left.memory_handle() == right.memory_handle();
	}

	/** Returns `true` if the two `image_t`s are not equal. */
	static bool operator!=(const image_t& left, const image_t& right)
	{
		return !(left == right);
	}
}

namespace std // Inject hash for `cgb::image_sampler_t` into std::
{
	template<> struct hash<cgb::image_t>
	{
		std::size_t operator()(cgb::image_t const& o) const noexcept
		{
			std::size_t h = std::hash<VkImage>{}(o.image_handle());
			return h;
		}
	};
}
