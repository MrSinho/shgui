#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


#include <shgui-app/shgui-app.h>

int main(void) {

	/*
	you can also write your own Vulkan implementation, 
	then copy the required data to ShGui::ShGuiCore.
	Same concept applies in the application loop, 
	the shgui-app library is just an example  
	of shgui using vulkan
	*/
	ShGuiApp* p_app =  shGuiCreateApp(720, 480, "shgui app");
	ShGui*    p_gui = &p_app->gui;


	SH_GUI_POPUP_VAR(mypopup);

	while (shGuiAppRunning(p_app)) {

		shGuiAppCheckWindowSize(p_app);

		shGuiResetInputs(p_gui);


		shGuiText(
			p_gui,
			SH_GUI_VEC2_ZERO,
			SH_GUI_COLOR_WHITE,
			SH_GUI_WINDOW_TEXT_SIZE * p_gui->char_infos.char_scale_factor,
			"@mrsinho/shgui\nthis is an example",
			SH_GUI_PIXELS | SH_GUI_CENTER_WIDTH | SH_GUI_CENTER_HEIGHT
		);

		shGuiWindow(
			p_gui,
			SH_GUI_VEC2_ZERO,
			(shguivec2) { 30.0f, 50.0f },
			SH_GUI_COLOR_CYAN,
			SH_GUI_COLOR_GREY,
			SH_GUI_COLOR_WHITE,
			SH_GUI_COLOR_GREY,
			SH_GUI_LEFT_MOUSE_BUTTON,
			SH_GUI_VEC2_ZERO,
			SH_GUI_VEC2_ZERO,
			SH_GUI_RELATIVE | SH_GUI_EDGE_LEFT | SH_GUI_MOVABLE | SH_GUI_RESIZABLE,
			"1st window",
			SH_GUI_COLOR_BLACK
		);
		shGuiWindowText(
			p_gui,
			SH_GUI_COLOR_BLACK,
			SH_GUI_WINDOW_TEXT_SIZE,
			"It's a window",
			0
		);
		
		shGuiWindow(
			p_gui,
			SH_GUI_VEC2_ZERO,
			(shguivec2) { 20.0f, 50.0f },
			SH_GUI_COLOR_YELLOW,
			SH_GUI_COLOR_GREY,
			SH_GUI_COLOR_WHITE,
			SH_GUI_COLOR_GREY,
			SH_GUI_LEFT_MOUSE_BUTTON,
			SH_GUI_VEC2_ZERO,
			SH_GUI_VEC2_ZERO,
			SH_GUI_RELATIVE | SH_GUI_MOVABLE | SH_GUI_RESIZABLE | SH_GUI_EDGE_RIGHT,
			"2nd window",
			SH_GUI_COLOR_BLACK
		);
		if (shGuiWindowButton(
			p_gui, 
			SH_GUI_WINDOW_TEXT_SIZE,
			(shguivec4) { 1.0f, 1.0f, 1.0f, 1.0f },
			SH_GUI_VEC4_ZERO,
			"LMAO",
			SH_GUI_VEC4_ZERO,
			SH_GUI_CENTER_WIDTH,
			SH_GUI_PRESS_ONCE
		)) {
			puts("LMAO");
		}
		shGuiWindowSeparator(
			p_gui,
			SH_GUI_VEC4_ZERO
		);
		float lmao = 0.0f;
		shGuiWindowSliderf(
			p_gui,
			SH_GUI_WINDOW_TEXT_SIZE,
			0.0f, 100.0f,
			"NONE",
			SH_GUI_VEC4_ZERO,
			SH_GUI_VEC4_ZERO,
			(shguivec4) { 1.0f, 1.0f, 1.0f, 1.0f },
			SH_GUI_VEC4_ZERO,
			SH_GUI_VEC4_ZERO,
			&lmao,
			SH_GUI_CENTER_WIDTH
		);
	
		shGuiMenuBar(
			p_gui,
			SH_GUI_WINDOW_TEXT_SIZE,
			SH_GUI_COLOR_WHITE,
			SH_GUI_COLOR_GREY,
			SH_GUI_PIXELS | SH_GUI_EDGE_TOP
		);
		if (shGuiMenuItem(
			p_gui,
			"file",
			SH_GUI_COLOR_BLACK,
			SH_GUI_COLOR_WHITE,
			SH_GUI_WINDOW_TEXT_SIZE,
			SH_GUI_COLOR_WHITE,
			SH_GUI_EDGE_LEFT
		)) {
			SH_GUI_SWITCH_POPUP(mypopup);
		}
		if (shGuiMenuItem(
			p_gui,
			"edit",
			SH_GUI_COLOR_BLACK,
			SH_GUI_COLOR_WHITE,
			SH_GUI_WINDOW_TEXT_SIZE,
			SH_GUI_COLOR_WHITE,
			SH_GUI_EDGE_LEFT
		)) {
			SH_GUI_SWITCH_POPUP(mypopup);
		}

		SH_GUI_CHECK_POPUP(mypopup)
		shGuiWindow(
			p_gui,
			(shguivec2) { 10.0f, 50.0f },
			(shguivec2) { 30.0f, 20.0f },
			SH_GUI_COLOR_WHITE,
			SH_GUI_COLOR_GREY,
			SH_GUI_COLOR_WHITE,
			SH_GUI_COLOR_GREY,
			SH_GUI_LEFT_MOUSE_BUTTON,
			SH_GUI_VEC2_ZERO,
			SH_GUI_VEC2_ZERO,
			SH_GUI_RELATIVE | SH_GUI_MOVABLE | SH_GUI_RESIZABLE | SH_GUI_CENTER_WIDTH | SH_GUI_CENTER_HEIGHT,
			"My popup",
			SH_GUI_COLOR_BLACK
		);
		shGuiWindowText(
			p_gui,
			SH_GUI_COLOR_BLACK,
			SH_GUI_WINDOW_TEXT_SIZE,
			"Edit or save!!",
			SH_GUI_CENTER_WIDTH
		);
		SH_GUI_END_POPUP()

		shGuiAppUpdate(p_app);
	}
	

	shGuiReleaseApp(p_app);

	return 0;

}

#ifdef __cplusplus
}
#endif//__cplusplus