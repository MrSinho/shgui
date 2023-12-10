#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


#include <shgui-app/shgui-app.h>

int main(void) {

	/*
	you can also write your own Vulkan implementation, 
	then copy the required data to ShGui::ShGuiCore.
	Same concept applies in the application loop, 
	the shgui-app library could be used as an easy implementation and as an example
	of shgui using vulkan
	*/
	ShGuiApp* p_app =  shGuiCreateApp(720, 480, "shgui app");
	ShGui*    p_gui = &p_app->gui;

	while (shGuiAppRunning(p_app)) {

		shGuiAppCheckWindowSize(p_app);

		shGuiText(
			p_gui,
			SH_GUI_VEC2_ZERO,
			SH_GUI_COLOR_WHITE,
			20.0f,
			"hello I am anas",
			SH_GUI_PIXELS | SH_GUI_CENTER_WIDTH | SH_GUI_CENTER_HEIGHT
		);

		shGuiRegion(
			p_gui,
			(shguivec2) {   0.0f,   0.0f },
			(shguivec2) {  20.0f, 100.0f },
			SH_GUI_COLOR_YELLOW,
			SH_GUI_COLOR_WHITE,
			SH_GUI_CENTER_HEIGHT | SH_GUI_EDGE_LEFT | SH_GUI_RELATIVE
		);

		shGuiAppUpdate(p_app);
	}
	

	shGuiReleaseApp(p_app);

	return 0;

}

#ifdef __cplusplus
}
#endif//__cplusplus