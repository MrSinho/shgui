#ifdef __cplusplus
"C" {
#endif//__cplusplus



#include "shgui/shgui.h"
#include "shgui/shgui-font.h"

#include <stdlib.h>
#include <string.h>



uint8_t shGuiGenerateCharVertices(
    const char*    s_letter,
    ShGuiCharIndex char_idx,
    float*         char_vertex_dst_raw
) {
    shGuiError(s_letter            == NULL, "shgui error: invalid source character",                 return 0);
    shGuiError(char_vertex_dst_raw == NULL, "shgui error: invalid char vertices destination memory", return 0);

    shGuiError(
        char_idx >= SH_GUI_CHAR_INDEX_MAX_ENUM, 
        "shgui error: invalid destination mesh", 
        return 0
    );

    memset(char_vertex_dst_raw, 0, sizeof(ShGuiCharVertexRaw));

    float x_step = 0.65f;
    float y_step = 1.0f;

    float current_vertex_x = 0.0f;
    float current_vertex_y = 6.0f;

    uint32_t vertex_idx  = 0;
    uint32_t literal_idx = 0;

    for (uint32_t literal_idx = 0; literal_idx < strlen(SH_GUI_LETTER_A); literal_idx++) {
    
        if (s_letter[literal_idx] == '*') {
            char_vertex_dst_raw[vertex_idx * 3 + 0] = current_vertex_x;
            char_vertex_dst_raw[vertex_idx * 3 + 1] = current_vertex_y;
            vertex_idx++;
        }

        else if (s_letter[literal_idx] == ' ') {
            char_vertex_dst_raw[vertex_idx * 3 + 2] = SH_GUI_OUT_OF_CANVAS_Z_PRIORITY;
            vertex_idx++;
        }

        else if (s_letter[literal_idx] == '\n') {
            current_vertex_y -=  y_step;
            current_vertex_x  = -x_step;//gets cancelled when adding 1

            vertex_idx += vertex_idx % SH_GUI_CHAR_COLUMN_COUNT;
        }

        current_vertex_x += x_step;
    }

    return 1;
}

uint8_t shGuiLoadFont(
    ShGui* p_gui
) {
    shGuiError(p_gui == NULL, "shgui error: invalid gui memory", return 0);

    const char* src[SH_GUI_CHAR_INDEX_MAX_ENUM] = {
        SH_GUI_LETTER_A,
        SH_GUI_LETTER_B,
        SH_GUI_LETTER_C,
        SH_GUI_LETTER_D,
        SH_GUI_LETTER_E,
        SH_GUI_LETTER_F,
        SH_GUI_LETTER_G,
        SH_GUI_LETTER_H,
        SH_GUI_LETTER_I,
        SH_GUI_LETTER_J,
        SH_GUI_LETTER_K,
        SH_GUI_LETTER_L,
        SH_GUI_LETTER_M,
        SH_GUI_LETTER_N,
        SH_GUI_LETTER_O,
        SH_GUI_LETTER_P,
        SH_GUI_LETTER_Q,
        SH_GUI_LETTER_R,
        SH_GUI_LETTER_S,
        SH_GUI_LETTER_T,
        SH_GUI_LETTER_U,
        SH_GUI_LETTER_V,
        SH_GUI_LETTER_W,
        SH_GUI_LETTER_X,
        SH_GUI_LETTER_Y,
        SH_GUI_LETTER_Z,
        SH_GUI_NUMBER_0,
        SH_GUI_NUMBER_1,
        SH_GUI_NUMBER_2,
        SH_GUI_NUMBER_3,
        SH_GUI_NUMBER_4,
        SH_GUI_NUMBER_5,
        SH_GUI_NUMBER_6,
        SH_GUI_NUMBER_7,
        SH_GUI_NUMBER_8,
        SH_GUI_NUMBER_9,
        SH_GUI_SPACE,
        SH_GUI_TAB,
        SH_GUI_NEWLINE,
        SH_GUI_PERIOD,
        SH_GUI_COMMA,
        SH_GUI_EXCLAMATION,
        SH_GUI_QUESTION,
        SH_GUI_COLON,
        SH_GUI_SEMICOLON,
        SH_GUI_FORWARD_SLASH,
        SH_GUI_BACKSLASH,
        SH_GUI_UNDERSCORE,
        SH_GUI_HYPHEN,
        SH_GUI_VERTICAL_BAR,
        SH_GUI_AMPERSAND,
        SH_GUI_PERCENT,
        SH_GUI_DOLLAR,
        SH_GUI_HASH,
        SH_GUI_AT,
        SH_GUI_SINGLE_QUOTE,
        SH_GUI_DOUBLE_QUOTE,
        SH_GUI_PLUS,
        SH_GUI_MINUS,
        SH_GUI_ASTERISK,
        SH_GUI_EQUALS,
        SH_GUI_LESS_THAN,
        SH_GUI_GREATER_THAN,
        SH_GUI_LEFT_PAREN,
        SH_GUI_RIGHT_PAREN,
        SH_GUI_LEFT_BRACKET,
        SH_GUI_RIGHT_BRACKET,
        SH_GUI_TILDE,
        SH_GUI_GRAVE_ACCENT
    };

    for (uint32_t char_idx = SH_GUI_CHAR_INDEX_A; char_idx < SH_GUI_CHAR_INDEX_MAX_ENUM; char_idx++) {
        shGuiGenerateCharVertices(src[char_idx], char_idx, p_gui->char_infos.chars_font_mesh[char_idx]);
    }

    return 1;
}


#ifdef __cplusplus
}
#endif//__cplusplus