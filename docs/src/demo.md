## Demo with code

![](https://img.shields.io/badge/Sinho_softworks-FF0080?style=for-the-badge&logo=&logoColor=white&labelColor=990042)
[![](https://img.shields.io/badge/GitHub_repository-000000?style=for-the-badge&logo=github&logoColor=white)](https://github.com/mrsinho/shgui)

![](/docs/media/shgui-demo.png)

```c
#include <shgui-app/shgui-app.h>

int main(void) {

	/*
	You can also write your own Vulkan implementation, 
	then copy the required data to ShGui::ShGuiCore.
	The same concept applies with the application loop, 
	the shgui-app library could be used as an easy implementation 
    or template code for vulkan.
	*/
	ShGuiApp* p_app =  shGuiCreateApp(800, 500, "shgui demo");
	ShGui*    p_gui = &p_app->gui;

	while (shGuiAppRunning(p_app)) {

		shGuiAppCheckWindowSize(p_app);

		shGuiText(
			p_gui,
			SH_GUI_VEC2_ZERO,
			SH_GUI_COLOR_WHITE,
			40.0f,
			"ABCDEFGHIJKLM\nNOPQRSTUVWXYZ",
			SH_GUI_CENTER_WIDTH | SH_GUI_CENTER_HEIGHT
		);
		
		shGuiText(
			p_gui,
			SH_GUI_VEC2_ZERO,
			SH_GUI_COLOR_WHITE,
			15.0f,
			"Left edge,\ncentering height",
			SH_GUI_EDGE_LEFT | SH_GUI_CENTER_HEIGHT
		);
		
		shGuiText(
			p_gui,
			SH_GUI_VEC2_ZERO,
			SH_GUI_COLOR_WHITE,
			15.0f,
			"Right edge,\ncentering height",
			SH_GUI_EDGE_RIGHT | SH_GUI_CENTER_HEIGHT
		);
		
		shGuiText(
			p_gui,
			SH_GUI_VEC2_ZERO,
			SH_GUI_COLOR_WHITE,
			15.0f,
			"Bottom edge,\ncentering width",
			SH_GUI_EDGE_BOTTOM | SH_GUI_CENTER_WIDTH
		);
		
		shGuiText(
			p_gui,
			SH_GUI_VEC2_ZERO,
			SH_GUI_COLOR_WHITE,
			15.0f,
			"Top edge,\ncentering width",
			SH_GUI_EDGE_TOP | SH_GUI_CENTER_WIDTH
		);
		
		shGuiText(
			p_gui,
			SH_GUI_VEC2_ZERO,
			SH_GUI_COLOR_WHITE,
			15.0f,
			"Bottom left corner",
			SH_GUI_EDGE_LEFT | SH_GUI_EDGE_BOTTOM
		);
		
		shGuiText(
			p_gui,
			SH_GUI_VEC2_ZERO,
			SH_GUI_COLOR_WHITE,
			15.0f,
			"Bottom right corner",
			SH_GUI_EDGE_RIGHT | SH_GUI_EDGE_BOTTOM
		);
		
		shGuiText(
			p_gui,
			SH_GUI_VEC2_ZERO,
			SH_GUI_COLOR_WHITE,
			15.0f,
			"Top left corner",
			SH_GUI_EDGE_LEFT | SH_GUI_EDGE_TOP
		);
		
		shGuiText(
			p_gui,
			SH_GUI_VEC2_ZERO,
			SH_GUI_COLOR_WHITE,
			15.0f,
			"Top right corner",
			SH_GUI_EDGE_RIGHT | SH_GUI_EDGE_TOP
		);
		
		shGuiRegion(
			p_gui,
			(shguivec2) { -50.0f,  50.0f },
			(shguivec2) {  25.0f,  25.0f },
			SH_GUI_COLOR_RED,
			SH_GUI_COLOR_WHITE,
			SH_GUI_RELATIVE
		);
		
		shGuiRegion(
			p_gui,
			(shguivec2) {  50.0f,  50.0f },
			(shguivec2) {  25.0f, 25.0f },
			SH_GUI_COLOR_MAGENTA,
			SH_GUI_COLOR_WHITE,
			SH_GUI_RELATIVE
		);
		
		shGuiRegion(
			p_gui,
			(shguivec2) { -50.0f, -50.0f },
			(shguivec2) {  25.0f,  25.0f },
			SH_GUI_COLOR_YELLOW,
			SH_GUI_COLOR_WHITE,
			SH_GUI_RELATIVE
		);
		
		shGuiRegion(
			p_gui,
			(shguivec2) {  50.0f, -50.0f },
			(shguivec2) {  25.0f,  25.0f },
			SH_GUI_COLOR_BLUE,
			SH_GUI_COLOR_WHITE,
			SH_GUI_RELATIVE
		);
		
		
		shGuiText(
			p_gui,
			(shguivec2) { 0.0f, 250.0f },
			SH_GUI_COLOR_WHITE,
			20.0f,
			"12345\n67890",
			SH_GUI_CENTER_WIDTH
		);
		
		
		shGuiText(
			p_gui,
			(shguivec2) { 0.0f, -250.0f },
			SH_GUI_COLOR_WHITE,
			20.0f,
			",.!?:;/\\_-|&\n%$#@\'\"+-*=<>\n()[]^`",
			SH_GUI_CENTER_WIDTH
		);

		shGuiAppUpdate(p_app);
	}
	

	shGuiReleaseApp(p_app);

	return 0;

}
```