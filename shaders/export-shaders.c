#ifdef __cplusplus
extern "C" {
#endif//__cplusplus



#include <native-export/native-export.h>



#define SHADER_COUNT 4

static char* shader_paths[SHADER_COUNT] = {
	"../shaders/bin/shgui-region.vert.spv",
	"../shaders/bin/shgui-region.frag.spv",
	"../shaders/bin/shgui-text.vert.spv",
	"../shaders/bin/shgui-text.frag.spv",
};
static char* shader_names[SHADER_COUNT] = {
	"shgui_region_vert_spv",
	"shgui_region_frag_spv",
	"shgui_text_vert_spv",
	"shgui_text_frag_spv",
};



char* shGuiReadBinary(char* path, uint32_t* p_code_size);



int main(void) {

	NativeExportInfo export_info = {
		SHADER_COUNT,
		calloc(SHADER_COUNT, sizeof(NativeExportBuffer)),
		"SH_GUI_SHADERS_H"
	};
	nativeExportError(export_info.p_buffers == NULL, "implementation: invalid native export buffers memory", return -1);

	for (uint32_t shader_idx = 0; shader_idx < SHADER_COUNT; shader_idx++) {
		export_info.p_buffers[shader_idx].p_src = (void*)shGuiReadBinary(shader_paths[shader_idx], &export_info.p_buffers[shader_idx].size);
		export_info.p_buffers[shader_idx].name = shader_names[shader_idx];
		export_info.p_buffers[shader_idx].format = NATIVE_EXPORT_FORMAT_UNSPECIFIED;
	}

	nativeExportWriteHeader(export_info, "../shaders/include/shgui/shgui-shaders.h");


	for (uint32_t shader_idx = 0; shader_idx < SHADER_COUNT; shader_idx++) {
		void* p_src = export_info.p_buffers[shader_idx].p_src;
		if (p_src != NULL) {
			free(p_src);
		}
	}
	nativeExportRelease(&export_info);

	return 0;


    return 0;
}

#ifdef _MSC_VER
#pragma warning (disable: 4996)
#endif//_MSC_VER


char* shGuiReadBinary(char* path, uint32_t* p_code_size) {

	FILE* stream = fopen(path, "rb");

	if (stream == NULL) { return NULL; }

	fseek(stream, 0, SEEK_END);
	uint32_t code_size = ftell(stream);
	fseek(stream, 0, SEEK_SET);

	char* code = (char*)malloc(code_size);
	if (code == NULL) { free(stream); return NULL; }

	fread(code, code_size, 1, stream);
	(p_code_size != NULL) && (*p_code_size = code_size);

	fclose(stream);

	return code;
}


#ifdef __cplusplus
}
#endif//__cplusplus