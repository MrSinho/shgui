#ifndef SH_GUI_H
#define SH_GUI_H

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus



#include <shgui-shaders/shguiShaders.h>

#include <shvulkan/shVulkan.h>

#include <stdio.h>
#include <stdint.h>



#ifndef _WIN32
#define SH_GUI_CALL
#else
#define SH_GUI_CALL __stdcall
#endif//_WIN32



/*
* @brief prints an error message and executes a specific failure expression when a condition is satisfied.
* 
* @param condition          condition that has to be satisfied, the value will be casted to an int type.
* @param error_msg          error message to print on the default system terminal.
* @param failure_expression any block of C code.
*/
#define shGuiError(\
	condition,\
	error_msg,\
	failure_expression\
)\
	if ((int)(condition)) {\
		printf("shgui error: %s\n", (const char*)(error_msg));\
		failure_expression;\
	}



/**
* @brief Macro for checking Vulkan API call results and handling errors.
*
* This macro is used to check the result of a Vulkan API call and handle any errors that occur. If the result is not VK_SUCCESS,
* it prints an error message along with the provided `error_msg` and the translated Vulkan result. Additionally, it executes
* the `failure_expression`, allowing for custom error handling code.
*
* @param result             The result code to be checked, typically returned by a Vulkan API call.
* @param error_msg          A user-defined error message to be displayed in case of failure.
* @param failure_expression Custom code or action to be executed in case of failure.
*/
#define shGuiVkResultError(\
	result,\
	error_msg,\
	failure_expression\
)\
	if ((VkResult)(result) != VK_SUCCESS) {\
		printf("shgui vulkan error: %s, %s\n", error_msg, shTranslateVkResult((VkResult)(result)));\
		failure_expression;\
	}



/*
* @brief Macro used to define an empty C struct with the shgui api.
*/
#define SH_GUI_STRUCTURE_ZERO { 0 }



/**
* @struct ShGuiCore
* @brief Represents the core Vulkan components for the shgui library.
*
* The @ref ShGuiCore structure encapsulates essential Vulkan components required by the shgui library for rendering and GUI operations. It includes the Vulkan device, physical device, graphics queue information, rendering surface, render pass, and other relevant settings.
*/
typedef struct ShGuiCore {
	VkDevice         device;                      /**< Vulkan logical device.             */
	VkPhysicalDevice physical_device;             /**< Vulkan physical device.            */
	uint32_t         graphics_queue_family_index; /**< Index of the graphics queue family.*/
	VkQueue          graphics_queue;              /**< Vulkan graphics queue.             */
	VkSurfaceKHR     surface;                     /**< rendering surface.                 */
	VkRenderPass     render_pass;                 /**< Vulkan render pass.                */
	uint32_t         sample_count;                /**< sample count for rendering.        */
	uint32_t         swapchain_image_count;       /**< number of swapchain images.        */
} ShGuiCore;								      



/**
* @brief The last valid key event code in the shgui library.
*
* This constant represents the highest valid key event code within the shgui library. Key event codes are typically used to identify and handle keyboard input in shgui applications.
*/
#define SH_GUI_KEY_LAST 348



/**
* @typedef ShGuiKeyEvents
* @brief A memory block to represent and store shgui key events.
*/
typedef int8_t ShGuiKeyEvents[SH_GUI_KEY_LAST + 1];



/**
* @brief A constant representing the absence of a key event.
*/
#define SH_GUI_NO_KEY UINT8_MAX



/**
* @typedef ShGui_UTF32_Char
* @brief Represents a Unicode character encoded in UTF-32 format.
*/
typedef uint32_t ShGui_UTF32_Char;



/**
* @enum ShGuiInputFlags
* @brief Flags for shgui input handling. These flags help control how input events, such as clicks and key presses, are processed.
*/
typedef enum ShGuiInputFlags {
	SH_GUI_CLICK_ONCE     = 1 << 0, /**< Single click event.                              */
	SH_GUI_CLICK_REPEAT   = 1 << 1, /**< Repeated click event.                            */
	SH_GUI_PRESS_ONCE     = 1 << 2, /**< Single press event.                              */
	SH_GUI_PRESS_REPEAT   = 1 << 3, /**< Repeated press event.                            */
    SH_GUI_PRESS_MAX_ENUM           /**< Maximum value for enumeration boundary checking. */
} ShGuiInputFlags;



/**
* @struct ShGuiInputs
* @brief Represents shgui input data.
*
* The `ShGuiInputs` structure encapsulates various input data used in shgui applications, including window dimensions, key events, and a system-submitted character. It is designed to help manage and process input-related information.
*/
typedef struct ShGuiInputs {
    uint32_t*        p_window_width;        /**< Pointer to the window's width.                      */
    uint32_t*        p_window_height;       /**< Pointer to the window's height.                     */
    int8_t*          key_events;            /**< Memory block to store key events.                   */
    int8_t*          last_key_events;       /**< Memory block to store the last recorded key events. */
    ShGui_UTF32_Char system_submitted_char; /**< System-submitted character for text input.          */
} ShGuiInputs;



/**
* @struct shguivec2
* @brief Represents a 2D vector with two float components.
*
* The `shguivec2` structure is used to represent 2D vectors with `x` and `y` components. It is commonly employed in shgui applications for various 2D operations.
*/
typedef struct shguivec2 {
	float x; /**< The x-component of the 2D vector.*/
	float y; /**< The y-component of the 2D vector.*/
} shguivec2;

/**
* @struct shguivec3
* @brief Represents a 3D vector with three float components.
*
* The `shguivec3` structure is used to represent 3D vectors with `x`, `y`, and `z` components. It is typically utilized in shgui applications for various 3D operations.
*/
typedef struct shguivec3 {
	float x; /**< The x-component of the 3D vector.*/
	float y; /**< The y-component of the 3D vector.*/
	float z; /**< The z-component of the 3D vector.*/
} shguivec3;

/**
* @struct shguivec4
* @brief Represents a 4D vector with four float components.
*
* The `shguivec4` structure is used to represent 4D vectors with `x`, `y`, `z`, and `w` components. It is commonly employed in shgui applications for various 4D operations.
*/
typedef struct shguivec4 {
	float x; /**< The x-component of the 4D vector.*/
	float y; /**< The y-component of the 4D vector.*/
	float z; /**< The z-component of the 4D vector.*/
	float w; /**< The w-component of the 4D vector.*/
} shguivec4;




/**
* @var _SH_GUI_VEC2_ZERO
* @brief Zero-initialized 2D vector constant.
*/
static const shguivec2 _SH_GUI_VEC2_ZERO = SH_GUI_STRUCTURE_ZERO;

/**
* @var _SH_GUI_VEC3_ZERO
* @brief Zero-initialized 3D vector constant.
*/
static const shguivec3 _SH_GUI_VEC3_ZERO = SH_GUI_STRUCTURE_ZERO;

/**
* @var _SH_GUI_VEC4_ZERO
* @brief Zero-initialized 4D vector constant.
*/
static const shguivec4 _SH_GUI_VEC4_ZERO = SH_GUI_STRUCTURE_ZERO;

/**
* @brief Alias for `_SH_GUI_VEC2_ZERO`.
*/
#define SH_GUI_VEC2_ZERO _SH_GUI_VEC2_ZERO

/**
* @brief Alias for `_SH_GUI_VEC3_ZERO`.
*/
#define SH_GUI_VEC3_ZERO _SH_GUI_VEC3_ZERO

/**
* @brief Alias for `_SH_GUI_VEC4_ZERO`.
*/
#define SH_GUI_VEC4_ZERO _SH_GUI_VEC4_ZERO

/**
* @brief Copy a 2D vector from source to destination.
*/
#define SH_GUI_VEC2_COPY(dst, src) (((dst).x = (src).x) && ((dst).y = (src).y))

/**
* @brief Copy a 3D vector from source to destination.
*/
#define SH_GUI_VEC3_COPY(dst, src) (((dst).x = (src).x) && ((dst).y = (src).y) && ((dst).z = (src).z))

/**
* @brief Copy a 4D vector from source to destination.
*/
#define SH_GUI_VEC4_COPY(dst, src) (((dst).x = (src).x) && ((dst).y = (src).y) && ((dst).z = (src).z) && ((dst).w = (src).w))

/**
* @brief Color constant representing black.
*/
#define SH_GUI_COLOR_BLACK (shguivec4){ 0.0f }

/**
* @brief Color constant representing white.
*/
#define SH_GUI_COLOR_WHITE (shguivec4){ 1.0f, 1.0f, 1.0f, 1.0f }

/**
* @brief Color constant representing red.
*/
#define SH_GUI_COLOR_RED (shguivec4){ 1.0f, 0.0f, 0.0f, 1.0f }

/**
* @brief Color constant representing green.
*/
#define SH_GUI_COLOR_GREEN (shguivec4){ 0.0f, 1.0f, 0.0f, 1.0f }

/**
* @brief Color constant representing blue.
*/
#define SH_GUI_COLOR_BLUE (shguivec4){ 0.0f, 0.0f, 1.0f, 1.0f }

/**
* @brief Color constant representing yellow.
*/
#define SH_GUI_COLOR_YELLOW (shguivec4){ 1.0f, 1.0f, 0.0f }

/**
* @brief Color constant representing cyan.
*/
#define SH_GUI_COLOR_CYAN (shguivec4){ 0.0f, 1.0f, 1.0f }

/**
* @brief Color constant representing magenta.
*/
#define SH_GUI_COLOR_MAGENTA (shguivec4){ 1.0f, 0.0f, 1.0f }

/**
* @brief Color constant representing grey.
*/
#define SH_GUI_COLOR_GREY (shguivec4){ 0.5f, 0.5f, 0.5f }



/**
* @struct ShGuiRegionRaw
* @brief Represents raw data for a shgui region.
*
* The `ShGuiRegionRaw` structure is used to represent raw data for a shgui region. It includes the following properties:
*/
typedef struct ShGuiRegionRaw {
	shguivec2 position;   /**< The @ref shguivec2 position of the region.                                                       */
	shguivec2 scale;      /**< The @ref shguivec2 scale of the region.                                                          */
	shguivec4 color;      /**< The @ref shguivec4 color of the region.                                                          */
	shguivec3 edge_color; /**< The @ref shguivec3 edge color of the region.                                                     */
    float     z_priority; /**< The priority of the region over other shgui widgets. Smaller values represent a higher priority. */
} ShGuiRegionRaw;

/**
* @brief Maximum number of regions supported in a shgui application.
*/
#define SH_GUI_MAX_REGION_COUNT 1024

/**
* @brief Maximum size in bytes for raw region data in a shgui application.
*/
#define SH_GUI_MAX_REGIONS_RAW_SIZE (SH_GUI_MAX_REGION_COUNT * sizeof(ShGuiRegionRaw))



/**
 * @enum ShGuiRegionRawWriteFlags
 * @brief Flags for specifying raw write operations on shgui regions.
 *
 * The `ShGuiRegionRawWriteFlags` enumeration defines a set of flags used to specify raw write operations on shgui regions. These flags indicate which properties of a region should be updated during the write operation.
 */
typedef enum ShGuiRegionRawWriteFlags {
    SH_GUI_REGION_RAW_WRITE_NONE         = 0,                                                                       /**< No raw write operation.                          */
    SH_GUI_REGION_RAW_WRITE_X_POSITION   = 1 << 0,                                                                  /**< Write the X-position property of a shgui region. */
    SH_GUI_REGION_RAW_WRITE_Y_POSITION   = 1 << 1,                                                                  /**< Write the Y-position property of a shgui region. */
    SH_GUI_REGION_RAW_WRITE_POSITION     = SH_GUI_REGION_RAW_WRITE_X_POSITION | SH_GUI_REGION_RAW_WRITE_Y_POSITION, /**< Write both X and Y positions of a shgui region.  */
    SH_GUI_REGION_RAW_WRITE_X_SCALE      = 1 << 2,                                                                  /**< Write the X-scale property of a shgui region.    */
    SH_GUI_REGION_RAW_WRITE_Y_SCALE      = 1 << 3,                                                                  /**< Write the Y-scale property of a shgui region.    */
    SH_GUI_REGION_RAW_WRITE_SCALE        = SH_GUI_REGION_RAW_WRITE_X_SCALE | SH_GUI_REGION_RAW_WRITE_Y_SCALE,       /**< Write both X and Y scales of a shgui region.     */
    SH_GUI_REGION_RAW_WRITE_COLOR        = 1 << 4,                                                                  /**< Write the color property of a shgui region.      */
    SH_GUI_REGION_RAW_WRITE_EDGE_COLOR   = 1 << 5,                                                                  /**< Write the edge color property of a shgui region. */
    SH_GUI_REGION_RAW_WRITE_Z_PRIORITY   = 1 << 6,                                                                  /**< Write the Z-priority property of a shgui region. */
    SH_GUI_REGION_RAW_WRITE_MAX_ENUM                                                                                /**< Maximum value for enumeration boundary checking. */
} ShGuiRegionRawWriteFlags;




/**
* @struct ShGuiRegionInfos
* @brief Stores information about shgui regions.
*
* The `ShGuiRegionInfos` structure is used to store information about shgui regions.
*/
typedef struct ShGuiRegionInfos {
    float                    region_scale_factor;                              /**< A floating-point value representing the region scale factor.                             */
    uint32_t                 region_count;                                     /**< The total count of regions.                                                              */
	ShGuiRegionRaw           regions_raw            [SH_GUI_MAX_REGION_COUNT]; /**< A memory block of @ref ShGuiRegionRaw structures representing raw region data.                 */
	ShGuiRegionRawWriteFlags regions_raw_write_flags[SH_GUI_MAX_REGION_COUNT]; /**< A memory block of @ref ShGuiRegionRawWriteFlags for specifying raw write operations on regions.*/
} ShGuiRegionInfos;



/**
* @struct ShGuiCharRaw
* @brief Represents raw data for a shgui character.
*
* The `ShGuiCharRaw` structure is used to represent raw data for a shgui character. It includes the following properties:
*/
typedef struct ShGuiCharRaw {
    shguivec2 position;   /**< A @ref shguivec2 representing the position of the character.                                  */
    float     scale;      /**< A floating-point value indicating the scale of the character.                                 */
    float     z_priority; /**< The priority of the char over other shgui widgets. Smaller values represent a higher priority.*/
    shguivec4 color;      /**< A @ref shguivec4 representing the color of the character.                                     */
} ShGuiCharRaw;

/**
* @enum ShGuiCharRawWriteFlags
* @brief Flags for specifying raw write operations on shgui characters.
*
* The `ShGuiCharRawWriteFlags` enumeration defines a set of flags used to specify raw write operations on shgui characters. These flags indicate which properties of a character should be updated during the write operation.
*/
typedef enum ShGuiCharRawWriteFlags {
    SH_GUI_CHAR_RAW_WRITE_X_POSITION = 1 << 1,                                                              /**< Overwrite the X position of the character.        */
    SH_GUI_CHAR_RAW_WRITE_Y_POSITION = 1 << 2,                                                              /**< Overwrite the Y position of the character.        */
    SH_GUI_CHAR_RAW_WRITE_POSITION   = SH_GUI_CHAR_RAW_WRITE_X_POSITION | SH_GUI_CHAR_RAW_WRITE_Y_POSITION, /**< Overwrite both X and Y positions of the character.*/
    SH_GUI_CHAR_RAW_WRITE_SCALE      = 1 << 3,                                                              /**< Overwrite the scale of the character.             */
    SH_GUI_CHAR_RAW_WRITE_Z_PRIORITY = 1 << 4,                                                              /**< Overwrite the priority of the character.          */
    SH_GUI_CHAR_RAW_WRITE_COLOR      = 1 << 5,                                                              /**< Overwrite the color of the character.             */
    SH_GUI_CHAR_RAW_MAX_ENUM                                                                                /**< Maximum value for enumeration boundary checking.  */
} ShGuiCharRawWriteFlags;

/**
 * @brief The maximum number of characters supported by the GUI.
 */
#define SH_GUI_MAX_CHAR_COUNT 2048

/**
 * @brief The raw size (in bytes) required to store the maximum number of characters.
 */
#define SH_GUI_MAX_CHARS_RAW_SIZE (SH_GUI_MAX_CHAR_COUNT * sizeof(ShGuiCharRaw))

/**
 * @brief The raw size (in bytes) required to store the maximum number of character vertices.
 */
#define SH_GUI_MAX_CHARS_VERTEX_RAW_SIZE (SH_GUI_MAX_CHAR_COUNT * sizeof(ShGuiCharVertexRaw))



/**
* @brief Maximum number of vertices required to render a character in a shgui application.
*
* The `SH_GUI_MAX_CHAR_VERTEX_COUNT` macro defines the maximum number of vertices required to render a character in a shgui application. It is calculated as 7x7 vertices, which corresponds to a 7x7 grid for character rendering.
*/
#define SH_GUI_MAX_CHAR_VERTEX_COUNT (7 * 7)

/**
* @typedef ShGuiCharVertexRaw
* @brief Type for storing raw character vertex data in a shgui application.
*
* The `ShGuiCharVertexRaw` type is used to store raw character vertex data in a shgui application. It is a memory block of float values with a size corresponding to the maximum character vertex count.
*/
typedef float ShGuiCharVertexRaw[SH_GUI_MAX_CHAR_VERTEX_COUNT * 3];

/**
* @brief Maximum size in bytes required to store character vertex data in a shgui application.
*
* The `SH_GUI_MAX_CHAR_VERTEX_SIZE` macro defines the maximum size, in bytes, required to store character vertex data in a shgui application. It is calculated as the product of @ref SH_GUI_MAX_CHAR_VERTEX_COUNT, 3 components per vertex (x, y, z-priority), and 4 bytes per component.
*/
#define SH_GUI_MAX_CHAR_VERTEX_SIZE (SH_GUI_MAX_CHAR_VERTEX_COUNT * 3 * 4)

/**
 * @brief A static constant representing a blank GUI character vertex raw data.
 */
static const ShGuiCharVertexRaw _SH_GUI_NO_CHAR = { 0xbf800000 };

/**
 * @brief Macro alias of @ref _SH_GUI_NO_CHAR.
 */
#define SH_GUI_NO_CHAR _SH_GUI_NO_CHAR


#define SH_GUI_SUPPORTED_CHAR_COUNT 70

#define SH_GUI_SUPPORTED_CHARS\
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',\
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',\
    ' ', '\t', '\n',\
    '.', ',', '!', '?', ':', ';', '/', '\\', '_', '-', '|', '&', '%', '$', '#', '@', '\'', '"', '+', '-', '*', '/', '=', '<', '>', '(', ')', '[', ']', '~', '`'\
//space, tab and newline not made, just change the offset



typedef float ShGuiCharMesh[49];//7x7 matrix grid



/**
 * @brief Macro used to copy GUI character vertices from a source memory block to a destination.
 *
 * @param dst The destination array where character vertices will be copied.
 * @param src The source array containing the original character vertices.
 */
#define SH_GUI_LOAD_CHAR_VERTICES(dst, src)\
    memcpy(dst, src, SH_GUI_MAX_CHAR_VERTEX_SIZE)



/**
 * @struct ShGuiCharInfos
 * @brief Structure containing information about GUI characters.
 */
typedef struct ShGuiCharInfos {
    float                   char_scale_factor;                                /**< The scale factor applied to all characters in the GUI.           */
    uint32_t                char_count;                                       /**< The total count of characters in the GUI.                        */
    ShGuiCharRaw            chars_raw[SH_GUI_MAX_CHAR_COUNT];                 /**< Memory block storing raw data for each GUI character.            */
    ShGuiCharVertexRaw      chars_vertex_raw[SH_GUI_MAX_CHAR_COUNT];          /**< Memory block storing raw vertex data for each GUI character.     */
    ShGuiCharRawWriteFlags  chars_raw_write_flags[SH_GUI_MAX_CHAR_COUNT];     /**< Memory block storing write flags for each GUI character.         */
    ShGuiCharMesh           chars_template_mesh[SH_GUI_SUPPORTED_CHAR_COUNT]; /**< Memory block storing the vertex position for each character type */
} ShGuiCharInfos;



/**
 * @brief The raw staging size (in bytes) for regions, equivalent to @ref SH_GUI_MAX_REGIONS_RAW_SIZE.
 */
#define SH_GUI_REGIONS_RAW_STAGING_SIZE SH_GUI_MAX_REGIONS_RAW_SIZE

/**
 * @brief The raw staging size (in bytes) for characters, equivalent to @ref SH_GUI_MAX_CHARS_RAW_SIZE.
 */
#define SH_GUI_CHARS_RAW_STAGING_SIZE SH_GUI_MAX_CHARS_RAW_SIZE

/**
 * @brief The raw staging size (in bytes) for character vertices, equivalent to @ref SH_GUI_MAX_CHARS_VERTEX_RAW_SIZE.
 */
#define SH_GUI_CHARS_VERTEX_RAW_STAGING_SIZE SH_GUI_MAX_CHARS_VERTEX_RAW_SIZE

/**
 * @brief The offset for raw staging regions, set to 0.
 */
#define SH_GUI_REGIONS_RAW_STAGING_OFFSET (0)

/**
 * @brief The offset for raw staging characters.
 */
#define SH_GUI_CHARS_RAW_STAGING_OFFSET (SH_GUI_REGIONS_RAW_STAGING_OFFSET + SH_GUI_REGIONS_RAW_STAGING_SIZE) 

/**
 * @brief The offset for raw staging character vertices.
 */
#define SH_GUI_CHARS_VERTEX_RAW_STAGING_OFFSET (SH_GUI_CHARS_RAW_STAGING_OFFSET + SH_GUI_CHARS_RAW_STAGING_SIZE)

/**
 * @brief The total size of the staging buffer.
 */
#define SH_GUI_STAGING_BUFFER_SIZE (SH_GUI_CHARS_VERTEX_RAW_STAGING_OFFSET + SH_GUI_CHARS_VERTEX_RAW_STAGING_SIZE)

/**
 * @brief The raw size (in bytes) for destination regions, equivalent to @ref SH_GUI_REGIONS_RAW_STAGING_SIZE.
 */
#define SH_GUI_REGIONS_RAW_DST_SIZE SH_GUI_REGIONS_RAW_STAGING_SIZE

/**
 * @brief The raw size (in bytes) for destination characters, equivalent to @ref SH_GUI_CHARS_RAW_STAGING_SIZE.
 */
#define SH_GUI_CHARS_RAW_DST_SIZE SH_GUI_CHARS_RAW_STAGING_SIZE

/**
 * @brief The raw size (in bytes) for destination character vertices, equivalent to @ref SH_GUI_CHARS_VERTEX_RAW_STAGING_SIZE.
 */
#define SH_GUI_CHARS_VERTEX_RAW_DST_SIZE SH_GUI_CHARS_VERTEX_RAW_STAGING_SIZE

/**
 * @brief The offset for destination regions, equivalent to @ref SH_GUI_REGIONS_RAW_STAGING_OFFSET.
 */
#define SH_GUI_REGIONS_RAW_DST_OFFSET SH_GUI_REGIONS_RAW_STAGING_OFFSET

/**
 * @brief The offset for destination characters, equivalent to @ref SH_GUI_CHARS_RAW_STAGING_OFFSET.
 */
#define SH_GUI_CHARS_RAW_DST_OFFSET SH_GUI_CHARS_RAW_STAGING_OFFSET

/**
 * @brief The offset for destination character vertices, equivalent to @ref SH_GUI_CHARS_VERTEX_RAW_STAGING_OFFSET.
 */
#define SH_GUI_CHARS_VERTEX_RAW_DST_OFFSET SH_GUI_CHARS_VERTEX_RAW_STAGING_OFFSET

/**
 * @brief The total size of the destination buffer, equivalent to @ref SH_GUI_STAGING_BUFFER_SIZE.
 */
#define SH_GUI_DST_BUFFER_SIZE SH_GUI_STAGING_BUFFER_SIZE



/**
 * @brief Macro representing the SPIR-V code for the GUI region vertex shader. It is defined as the corresponding static constant.
 */
#define SH_GUI_REGION_VERT_SPV _SH_GUI_REGION_VERT_SPV

/**
 * @brief Macro representing the SPIR-V code for the GUI region fragment shader. It is defined as the corresponding static constant.
 */
#define SH_GUI_REGION_FRAG_SPV _SH_GUI_REGION_FRAG_SPV

/**
 * @brief Macro representing the SPIR-V code for the GUI character vertex shader. It is defined as the corresponding static constant.
 */
#define SH_GUI_CHAR_VERT_SPV   _SH_GUI_CHAR_VERT_SPV

/**
 * @brief Macro representing the SPIR-V code for the GUI character fragment shader. It is defined as the corresponding static constant.
 */
#define SH_GUI_CHAR_FRAG_SPV   _SH_GUI_CHAR_FRAG_SPV




/**
 * @struct ShGui
 * @brief Structure representing the main context for the GUI.
 *
 * The ShGui structure encapsulates the core components of the GUI, including
 * the GUI core and Vulkan components, input handling, buffers for staging and destination, 
 * regions and characters information, and Vulkan pipelines for regions
 * and characters.
 */
typedef struct ShGui {
    ShGuiCore        core;                /**< Core components of the GUI.                    */
    ShGuiInputs      inputs;              /**< Input handling for the GUI.                    */
    VkBuffer         staging_buffer;      /**< Vulkan buffer for staging data.                */
    VkDeviceMemory   staging_memory;      /**< Vulkan device memory for staging data.         */
    VkBuffer         dst_buffer;          /**< Vulkan buffer for destination data.            */
    VkDeviceMemory   dst_memory;          /**< Vulkan device memory for destination data.     */
    ShGuiRegionInfos region_infos;        /**< Information about GUI regions.                 */
    ShGuiCharInfos   char_infos;          /**< Information about GUI characters.              */
    ShVkPipelinePool pipeline_pool;       /**< shvulkan pipeline pool for managing pipelines. */
    ShVkPipeline     region_pipeline;     /**< shvulkan pipeline for GUI regions.             */
    ShVkPipeline     char_pipeline;       /**< shvulkan pipeline for GUI characters.          */
} ShGui;


/**
* @brief Allocates memory for an instance of the ShGui structure.
*
* @return A pointer to the newly allocated instance of the ShGui structure.
*/
#define shAllocateGui() ((ShGui*)calloc(1, sizeof(ShGui)))



/**
* @brief Initializes the GUI with the provided `ShGuiCore` structure.
*
* @param p_gui    A pointer to a valid @ref ShGui structure.
* @param core     A valid @ref ShGuiCore structure containing information about the GUI core components.
*
* @return A result code indicating the success or failure of the function call.
*/
extern uint8_t shGuiInit(
    ShGui*    p_gui,
    ShGuiCore core
);

/**
* @brief Sets the Vulkan surface for the GUI.
*
* @param p_gui      A pointer to a valid @ref ShGui structure.
* @param surface    A valid Vulkan surface handle, see the Vulkan API documentation for more details.
*
* @return A result code indicating the success or failure of the function call.
*/
extern uint8_t shGuiSetSurface(
    ShGui*       p_gui,
    VkSurfaceKHR surface
);

/**
* @brief Sets the Vulkan render pass for the GUI.
*
* @param p_gui         A pointer to a valid @ref ShGui structure.
* @param renderpass    A valid Vulkan render pass handle, see the Vulkan API documentation for more details.
*
* @return A result code indicating the success or failure of the function call.
*/
extern uint8_t shGuiSetRenderpass(
    ShGui* p_gui,
    VkRenderPass renderpass
);

/**
* @brief Returns the available heap size for allocating memory, taking in consideration the host heap memory (RAM) and the GPU heap memory (VRAM).
*
* @param p_gui    A pointer to a valid @ref ShGui structure.
*
* @return The available heap size.
*/
extern uint32_t SH_GUI_CALL shGuiGetAvailableHeap(
    ShGui* p_gui
);

/**
* @brief Allocates memory for the GUI components.
*
* @param p_gui A pointer to a valid @ref ShGui structure.
*
* @return A result code indicating the success or failure of the function call.
*/
extern uint8_t SH_GUI_CALL shGuiAllocateMemory(
    ShGui* p_gui
);

/**
* @brief Reads a binary file and returns the binary data.
*
* @param path           The path to the binary file.
* @param p_code_size    A pointer to a variable to store the size of the binary file.
*
* @return The binary data located in a heap-allocated block. Needs to be freed after usage.
*/
extern char* SH_GUI_CALL shGuiReadBinary(
    const char* path,
    uint32_t*   p_code_size
);

/**
* @brief Allocates resources for the GUI pipelines.
*
* @param p_gui A pointer to a valid @ref ShGui structure.
* @param swapchain_image_count The number of swapchain images.
*
* @return A result code indicating the success or failure of the function call.
*/
extern uint8_t SH_GUI_CALL shGuiAllocatePipelineResources(
    ShGui* p_gui,
    uint32_t swapchain_image_count
);

/**
* @brief Builds the region pipeline using the provided vertex and fragment shader paths.
*
* @param p_gui                   A pointer to a valid @ref ShGui structure.
* @param vertex_shader_path      The path to the vertex shader file in the spv file format.
* @param fragment_shader_path    The path to the fragment shader file in the spv file format.
*
* @return A result code indicating the success or failure of the function call.
*/
extern uint8_t SH_GUI_CALL shGuiBuildRegionPipeline(
    ShGui*      p_gui,
    const char* vertex_shader_path,
    const char* fragment_shader_path
);

/**
* @brief Builds the char pipeline using the provided vertex and fragment shader paths.
*
* @param p_gui                   A pointer to a valid @ref ShGui structure.
* @param vertex_shader_path      The path to the vertex shader file in the spv file format.
* @param fragment_shader_path    The path to the fragment shader file in the spv file format.
*
* @return A result code indicating the success or failure of the function call.
*/
extern uint8_t SH_GUI_CALL shGuiBuildCharPipeline(
    ShGui*      p_gui,
    const char* vertex_shader_path,
    const char* fragment_shader_path
);

/**
* @brief Destroys the resources allocated for the GUI Vulkan pipelines.
*
* @param p_gui A pointer to a valid @ref ShGui structure.
*
* @return A result code indicating the success or failure of the function call.
*/
extern uint8_t SH_GUI_CALL shGuiDestroyPipelineResources(
    ShGui* p_gui
);

/**
* @brief Destroys the GUI Vulkan pipelines.
*
* @param p_gui A pointer to a valid @ref ShGui structure.
*
* @return A result code indicating the success or failure of the function call.
*/
extern uint8_t SH_GUI_CALL shGuiDestroyPipelines(
    ShGui* p_gui
);

/**
* @brief Writes memory to the GUI.
*
* @param p_gui                A pointer to a valid @ref ShGui structure.
* @param transfer_cmd_buffer  A Vulkan command buffer valid for memory transfer purposes, for more information see the Vulkan API documentation.
* @param begin_cmd_buffer     A byte indicating whether to start recording the command buffer, for more information see the Vulkan API documentation.
*
* @return A result code indicating the success or failure of the function call.
*/
extern uint8_t SH_GUI_CALL shGuiWriteMemory(
    ShGui*          p_gui,
    VkCommandBuffer transfer_cmd_buffer,
    uint8_t         begin_cmd_buffer
);

/**
* @brief Resizes the GUI interface.
*
* @param p_gui             A pointer to a valid @ref ShGui structure.
* @param last_width        The previous width of the interface.
* @param last_height       The previous height of the interface.
* @param current_width     The current width of the interface.
* @param current_height    The current height of the interface.
*
* @return A result code indicating the success or failure of the function call.
*/
extern uint8_t SH_GUI_CALL shGuiResizeInterface(
    ShGui*   p_gui,
    uint32_t last_width,
    uint32_t last_height,
    uint32_t current_width,
    uint32_t current_height
);

/**
* @brief Releases the memory allocated for the GUI.
*
* @param p_gui A pointer to a valid @ref ShGui structure.
*
* @return A result code indicating the success or failure of the function call.
*/
extern uint8_t SH_GUI_CALL shGuiReleaseMemory(
    ShGui* p_gui
);

/**
* @brief Renders the GUI using the provided command buffer and swapchain image index.
*
* @param p_gui                  A pointer to a valid @ref ShGui structure.
* @param cmd_buffer             A Vulkan command buffer valid for graphics operations, see the Vulkan API documentation for more details.
* @param swapchain_image_idx    The index of the current swapchain image.
*
* @return A result code indicating the success or failure of the function call.
*/
extern uint8_t SH_GUI_CALL shGuiRender(
    ShGui*          p_gui,
    VkCommandBuffer cmd_buffer,
    uint32_t        swapchain_image_idx
);

/**
* @brief Submits the recorded inputs to the main @ref ShGui handle.
*
* @param p_gui A pointer to a valid @ref ShGui structure.
*
* @return A result code indicating the success or failure of the function call.
*/
extern uint8_t SH_GUI_CALL shGuiSubmitInputs(
    ShGui* p_gui
);

/**
* @brief Releases the GUI.
*
* @param p_gui A pointer to a valid @ref ShGui structure.
*
* @return A result code indicating the success or failure of the function call.
*/
extern uint8_t SH_GUI_CALL shGuiRelease(
    ShGui* p_gui
);



/**
 * @enum ShGuiWidgetFlags
 * @brief Represents different flags that can be used to configure a GUI widget.
 */
typedef enum ShGuiWidgetFlags {
    SH_GUI_CENTER_WIDTH    = 1 << 0, /**< Center the widget horizontally.                    */
    SH_GUI_CENTER_HEIGHT   = 1 << 1, /**< Center the widget vertically.                      */
    SH_GUI_EDGE_LEFT       = 1 << 2, /**< Place the widget at the left edge.                 */
    SH_GUI_EDGE_RIGHT      = 1 << 3, /**< Place the widget at the right edge.                */
    SH_GUI_EDGE_TOP        = 1 << 4, /**< Place the widget at the top edge.                  */
    SH_GUI_EDGE_BOTTOM     = 1 << 5, /**< Place the widget at the bottom edge.               */
    SH_GUI_PIXELS          = 1 << 6, /**< Specify widget dimensions in pixels.               */
    SH_GUI_RELATIVE        = 1 << 7, /**< Specify widget dimensions as relative percentages. */
    SH_GUI_WIDGET_FLAGS_MAX_ENUM     /**< Maximum value for enumeration boundary checking.   */
} ShGuiWidgetFlags;

/**
 * @enum ShGuiWriteFlags
 * @brief Represents different flags for writing specific properties of a GUI element.
 */
typedef enum ShGuiWriteFlags {
    SH_GUI_WIDTH            = 1 << 0, /**< Write the width property of a GUI element.        */
    SH_GUI_HEIGHT           = 1 << 1, /**< Write the height property of a GUI element.       */
    SH_GUI_POSITION_X       = 1 << 2, /**< Write the X-coordinate property of a GUI element. */
    SH_GUI_POSITION_Y       = 1 << 3, /**< Write the Y-coordinate property of a GUI element. */
    SH_GUI_TITLE            = 1 << 4, /**< Write the title property of a GUI element.        */
    SH_GUI_WRITE_FLAGS_MAX_ENUM       /**< Maximum value for enumeration boundary checking.  */
} ShGuiWriteFlags;



/**
* @brief The z-priority for text elements in the GUI.
*
* This macro defines the z-priority for text elements in the GUI, which determines
* their rendering order. Smaller values indicate elements that should be rendered
* closer to the viewer, and have a higher priority over other UI elements.
*/
#define SH_GUI_TEXT_Z_PRIORITY   0.0001f

/**
* @brief The z-priority for region elements in the GUI.
*
* This macro defines the z-priority for region elements in the GUI, which determines
* their rendering order. Smaller values indicate elements that should be rendered
* closer to the viewer, and have a higher priority over other UI elements.
*/
#define SH_GUI_REGION_Z_PRIORITY 0.0002f

/**
* @brief The offset for character distance in the GUI.
*
* This macro defines the offset used for character distance in the GUI.
*/
#define SH_GUI_CHAR_DISTANCE_OFFSET 3.0f

/**
* @brief Calculate the X offset for a character at a given distance.
*
* This macro calculates the X offset for a character based on the provided
* character distance and scale values. It is used to position characters horizontally.
*
* @param char_distance_offset    The character distance offset.
* @param scale                   The scaling factor for character placement.
*
* @return The X offset for the character.
*/
#define SH_GUI_CHAR_X_OFFSET(\
	char_distance_offset,\
	scale\
)\
	(((float)char_distance_offset) / 4.0f * ((float)scale))

/**
* @brief Calculates the Y offset for a character at a given distance.
*
* This macro calculates the Y offset for a character based on the provided
* character distance, line count, and scale values. It is used to position characters vertically.
*
* @param char_distance_offset    The character distance offset.
* @param line_count              The number of lines.
* @param scale                   The scaling factor for character placement.
*
* @return The Y offset for the character.
*/
#define SH_GUI_CHAR_Y_OFFSET(\
	char_distance_offset,\
	line_count,\
	scale\
)\
	(((float)scale) * ((float)line_count))

/**
* @brief Calculates the final X offset for a character.
*
* This macro calculates the final X offset for a character based on the character distance,
* scale, and character index. It is used to position characters horizontally with respect to
* their index.
*
* @param char_distance_offset    The character distance offset.
* @param scale                   The scaling factor for character placement.
* @param char_idx                The index of the character.
*
* @return The final X offset for the character.
*/
#define SH_GUI_CHAR_FINAL_X_OFFSET(\
	char_distance_offset,\
	scale,\
	char_idx\
)\
	((float)char_distance_offset) / 4.0f* ((float)scale)* ((float)char_idx)

/**
* @brief Calculates the final Y offset for a character.
*
* This macro is an alias for SH_GUI_CHAR_Y_OFFSET and calculates the Y offset for a character
* based on the character distance, line count, and scale values.
*/
#define SH_GUI_CHAR_FINAL_Y_OFFSET SH_GUI_CHAR_Y_OFFSET




/**
* @brief Links the inputs to the @ref ShGui handle.
*
* @param p_gui              A pointer to a valid @ref ShGui structure.
* @param p_window_width     A pointer to the window width variable.
* @param p_window_height    A pointer to the window height variable.
* @param p_cursor_pos_x     A pointer to the cursor position x variable.
* @param p_cursor_pos_y     A pointer to the cursor position y variable.
* @param key_events         A valid of @ref ShGuiKeyEvents.
* @param p_delta_time       A pointer to the delta time variable.
*
* @return The result of the function call, indicating success or failure.
*/
extern uint8_t SH_GUI_CALL shGuiLinkInputs(
    ShGui*         p_gui,
    uint32_t*      p_window_width,
    uint32_t*      p_window_height,
    float*         p_cursor_pos_x,
    float*         p_cursor_pos_y,
    ShGuiKeyEvents key_events,
    ShGuiKeyEvents last_key_events,
    double*        p_delta_time
);

/**
* @brief Resets the GUI inputs.
*
* @param p_gui A pointer to a valid @ref ShGui structure.
* 
* @return The result of the function call, indicating success or failure.
*/
extern uint8_t SH_GUI_CALL shGuiResetInputs(
    ShGui* p_gui
);

/**
* @brief Updates the GUI inputs.
*
* @param p_gui A pointer to a valid @ref ShGui structure.
* 
* @return The result of the function call, indicating success or failure.
*/
extern uint8_t SH_GUI_CALL shGuiUpdateInputs(
    ShGui* p_gui
);

/**
* @brief Scales the UI of the GUI.
*
* @param p_gui                    A pointer to a valid @ref ShGui structure.
* @param region_scaling_factor    The scaling factor for regions.
* @param char_scaling_factor      The scaling factor for characters.
*
* @return The result of the function call, indicating success or failure.
*/
extern uint8_t SH_GUI_CALL shGuiScaleUI(
    ShGui* p_gui,
    float  region_scaling_factor,
    float  char_scaling_factor
);

/**
* @brief Creates a region in the GUI.
*
* @param p_gui               A pointer to a valid @ref ShGui structure.
* @param position_zero       The position of region.
* @param scale_zero          The scale of the region, in pixels or relative to the window dimensions, depending on the `flags` argument.
* @param color_zero          The color of the region.
* @param edge_color_zero     The edge color of the region.
* @param flags               Flags for creating a region.
*
* @return The result of the function call, indicating success or failure.
*/
extern uint8_t SH_GUI_CALL shGuiRegion(
    ShGui*           p_gui,
    shguivec2        position_zero,
    shguivec2        scale_zero,
    shguivec4        color_zero,
    shguivec4        edge_color_zero,
    ShGuiWidgetFlags flags
);

/**
* @brief Overwrites a region in the GUI.
*
* @param p_gui         A pointer to a valid @ref ShGui structure.
* @param region_idx    The index of the region to overwrite.
* @param p_src_data    A pointer to the source data for overwriting the region.
* @param flags         Flags for overwriting the region, see @ref ShGuiRegionRawWriteFlags.
*
* @return The result of the function call, indicating success or failure.
*/
extern uint8_t SH_GUI_CALL shGuiOverwriteRegion(
    ShGui*                   p_gui,
    uint32_t                 region_idx,
    ShGuiRegionRaw*          p_src_data,
    ShGuiRegionRawWriteFlags flags
);

/**
* @brief Overwrites multiple regions in the GUI.
*
* @param p_gui           A pointer to a valid @ref ShGui structure.
* @param first_region    The index of the first region to overwrite.
* @param region_count    The number of regions to overwrite.
* @param p_src_data      A pointer to the source data for overwriting the regions.
* @param flags           Flags for overwriting the regions.
*
* @return The result of the function call, indicating success or failure.
*/
extern uint8_t SH_GUI_CALL shGuiOverwriteRegions(
    ShGui*                   p_gui,
    uint32_t                 first_region,
    uint32_t                 region_count,
    ShGuiRegionRaw*          p_src_data,
    ShGuiRegionRawWriteFlags flags
);


extern uint8_t shGuiGenerateCharVertices(
    ShGui* p_gui
);

/**
* @brief Counts the number of lines in a string.
*
* @param s_text     The text to count the lines of.
* @param p_count    A pointer to the variable to store the line count.
*
* @return The result of the function call, indicating success or failure.
*/
extern uint8_t SH_GUI_CALL shGuiTextLineCount(
    const char* s_text,
    uint32_t* p_count
);

/**
* @brief Renders text in the GUI.
*
* @param p_gui                A pointer to a valid @ref ShGui structure.
* @param position_zero        The position of the text UI in the window canvas, could be in pixels or relative to the window dimensions, depending on the `flags` argument.
* @param color_zero           The color of the text characters.
* @param scale_pixels_zero    The scale of the text characters in pixels.
* @param s_text               Source string of characters to render.
* @param flags                Flags for rendering the text, see @ref ShGuiWidgetFlags.
*
* @return The result of the function call, indicating success or failure.
*/
extern uint8_t SH_GUI_CALL shGuiText(
    ShGui*           p_gui,
    shguivec2        position_zero,
    shguivec4        color_zero,
    float            scale_zero_pixels,
    char*            s_text,
    ShGuiWidgetFlags flags
);

/**
* @brief Overwrites a character in the GUI.
*
* @param p_gui         A pointer to a valid @ref ShGui structure.
* @param char_idx      The index of the character to overwrite.
* @param p_src_data    A pointer to the source data for overwriting the character.
* @param flags         Flags for overwriting the character, see @ref ShGuiCharRawWriteFlags.
*
* @return The result of the function call, indicating success or failure.
*/
extern uint8_t SH_GUI_CALL shGuiOverwriteChar(
    ShGui*                 p_gui,
    uint32_t               char_idx,
    ShGuiCharRaw*          p_src_data,
    ShGuiCharRawWriteFlags flags
);

/**
* Overwrites multiple characters in the GUI.
*
* @param p_gui         A pointer to a valid @ref ShGui structure.
* @param first_char    The index of the first character to overwrite.
* @param char_count    The number of characters to overwrite.
* @param p_src_data    A pointer to the source data for overwriting the characters.
* @param flags         Flags for overwriting the characters, see @ref ShGuiCharRawWriteFlags.
*
* @return The result of the function call, indicating success or failure.
*/
extern uint8_t SH_GUI_CALL shGuiOverwriteChars(
    ShGui*                 p_gui,
    uint32_t               first_char,
    uint32_t               char_count,
    ShGuiCharRaw*          p_src_data,
    ShGuiCharRawWriteFlags flags
);



#ifdef __cplusplus
}
#endif//__cplusplus

#endif//SH_GUI_H