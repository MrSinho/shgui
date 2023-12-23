#ifndef SH_GUI_FONTS_H
#define SH_GUI_FONTS_H

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus



#include <shgui/shgui.h>


/**
 * @brief Enumeration representing different character indices for the `shgui` library.
 * 
 * This enum defines indices for various characters used in the `shgui` library, including letters,
 * numbers, and special characters.
 */
typedef enum ShGuiCharIndex {
    SH_GUI_CHAR_INDEX_A = 0,
    SH_GUI_CHAR_INDEX_B,
    SH_GUI_CHAR_INDEX_C,
    SH_GUI_CHAR_INDEX_D,
    SH_GUI_CHAR_INDEX_E,
    SH_GUI_CHAR_INDEX_F,
    SH_GUI_CHAR_INDEX_G,
    SH_GUI_CHAR_INDEX_H,
    SH_GUI_CHAR_INDEX_I,
    SH_GUI_CHAR_INDEX_J,
    SH_GUI_CHAR_INDEX_K,
    SH_GUI_CHAR_INDEX_L,
    SH_GUI_CHAR_INDEX_M,
    SH_GUI_CHAR_INDEX_N,
    SH_GUI_CHAR_INDEX_O,
    SH_GUI_CHAR_INDEX_P,
    SH_GUI_CHAR_INDEX_Q,
    SH_GUI_CHAR_INDEX_R,
    SH_GUI_CHAR_INDEX_S,
    SH_GUI_CHAR_INDEX_T,
    SH_GUI_CHAR_INDEX_U,
    SH_GUI_CHAR_INDEX_V,
    SH_GUI_CHAR_INDEX_W,
    SH_GUI_CHAR_INDEX_X,
    SH_GUI_CHAR_INDEX_Y,
    SH_GUI_CHAR_INDEX_Z,
    SH_GUI_CHAR_INDEX_0,
    SH_GUI_CHAR_INDEX_1,
    SH_GUI_CHAR_INDEX_2,
    SH_GUI_CHAR_INDEX_3,
    SH_GUI_CHAR_INDEX_4,
    SH_GUI_CHAR_INDEX_5,
    SH_GUI_CHAR_INDEX_6,
    SH_GUI_CHAR_INDEX_7,
    SH_GUI_CHAR_INDEX_8,
    SH_GUI_CHAR_INDEX_9,
    SH_GUI_CHAR_INDEX_SPACE,
    SH_GUI_CHAR_INDEX_TAB,
    SH_GUI_CHAR_INDEX_NEWLINE,
    SH_GUI_CHAR_INDEX_PERIOD,
    SH_GUI_CHAR_INDEX_COMMA,
    SH_GUI_CHAR_INDEX_EXCLAMATION,
    SH_GUI_CHAR_INDEX_QUESTION,
    SH_GUI_CHAR_INDEX_COLON,
    SH_GUI_CHAR_INDEX_SEMICOLON,
    SH_GUI_CHAR_INDEX_FORWARD_SLASH,
    SH_GUI_CHAR_INDEX_BACKSLASH,
    SH_GUI_CHAR_INDEX_UNDERSCORE,
    SH_GUI_CHAR_INDEX_HYPHEN,
    SH_GUI_CHAR_INDEX_VERTICAL_BAR,
    SH_GUI_CHAR_INDEX_AMPERSAND,
    SH_GUI_CHAR_INDEX_PERCENT,
    SH_GUI_CHAR_INDEX_DOLLAR,
    SH_GUI_CHAR_INDEX_HASH,
    SH_GUI_CHAR_INDEX_AT,
    SH_GUI_CHAR_INDEX_SINGLE_QUOTE,
    SH_GUI_CHAR_INDEX_DOUBLE_QUOTE,
    SH_GUI_CHAR_INDEX_PLUS,
    SH_GUI_CHAR_INDEX_MINUS,
    SH_GUI_CHAR_INDEX_ASTERISK,
    SH_GUI_CHAR_INDEX_EQUALS,
    SH_GUI_CHAR_INDEX_LESS_THAN,
    SH_GUI_CHAR_INDEX_GREATER_THAN,
    SH_GUI_CHAR_INDEX_LEFT_PAREN,
    SH_GUI_CHAR_INDEX_RIGHT_PAREN,
    SH_GUI_CHAR_INDEX_LEFT_BRACKET,
    SH_GUI_CHAR_INDEX_RIGHT_BRACKET,
    SH_GUI_CHAR_INDEX_TILDE,
    SH_GUI_CHAR_INDEX_GRAVE_ACCENT,
    SH_GUI_CHAR_INDEX_MAX_ENUM
} ShGuiCharIndex;



/**
* @brief Generate vertex data for a specific character in the `shgui` library.
* 
* This function generates vertex data for a given character, identified by its character index,
* in the `shgui` library.
* 
* @param s_letter            The string representation of the character (not used in the current implementation).
* @param char_idx            The character index for which to generate vertex data.
* @param char_vertex_dst_raw Pointer to the raw vertex data destination.
* 
* @return An integer indicating the success (1) or failure (0) of the operation.
*/
extern uint8_t shGuiGenerateCharVertices(
    const char*    s_letter,
    ShGuiCharIndex char_idx,
    float*         char_vertex_dst_raw
);

/**
* @brief Load font data for the `shgui` library.
* 
* This function loads font data for the `shgui` library, initializing necessary resources.
* 
* @param p_gui Pointer to the ShGui structure to be initialized with font data.
* 
* @return An integer indicating the success (1) or failure (0) of the operation.
*/
extern uint8_t shGuiLoadFont(
    ShGui* p_gui
);



static const char* SH_GUI_LETTER_A = "\
 ***** \n\
*     *\n\
*     *\n\
*******\n\
*     *\n\
*     *\n\
*     *";

static const char* SH_GUI_LETTER_B = "\
****** \n\
*     *\n\
*     *\n\
****** \n\
*     *\n\
*     *\n\
******  ";

static const char* SH_GUI_LETTER_C = "\
 ***** \n\
*     *\n\
*      \n\
*      \n\
*      \n\
*     *\n\
 ***** ";

static const char* SH_GUI_LETTER_D = "\
****** \n\
*     *\n\
*     *\n\
*     *\n\
*     *\n\
*     *\n\
****** ";

static const char* SH_GUI_LETTER_E = "\
*******\n\
*      \n\
*      \n\
****** \n\
*      \n\
*      \n\
*******";

static const char* SH_GUI_LETTER_F = "\
*******\n\
*      \n\
*      \n\
****** \n\
*      \n\
*      \n\
*      ";

static const char* SH_GUI_LETTER_G = "\
 ***** \n\
*     *\n\
*      \n\
*      \n\
*   ***\n\
*     *\n\
 *****  ";

static const char* SH_GUI_LETTER_H = "\
*     *\n\
*     *\n\
*     *\n\
*******\n\
*     *\n\
*     *\n\
*     *";

static const char* SH_GUI_LETTER_I = "\
*******\n\
   *   \n\
   *   \n\
   *   \n\
   *   \n\
   *   \n\
*******";

static const char* SH_GUI_LETTER_J = "\
  *****\n\
      *\n\
      *\n\
      *\n\
*     *\n\
*     *\n\
 ***** ";

static const char* SH_GUI_LETTER_K = "\
*     *\n\
*    * \n\
*  **  \n\
***    \n\
*  **  \n\
*    * \n\
*    **";

static const char* SH_GUI_LETTER_L = "\
*      \n\
*      \n\
*      \n\
*      \n\
*      \n\
*      \n\
*******";

static const char* SH_GUI_LETTER_M = "\
*     *\n\
**   **\n\
* * * *\n\
*  *  *\n\
*     *\n\
*     *\n\
*     * ";

static const char* SH_GUI_LETTER_N = "\
*     *\n\
**    *\n\
* *   *\n\
*  *  *\n\
*   ***\n\
*     *\n\
*     * ";

static const char* SH_GUI_LETTER_O = "\
 ***** \n\
*     *\n\
*     *\n\
*     *\n\
*     *\n\
*     *\n\
 ***** ";

static const char* SH_GUI_LETTER_P = "\
****** \n\
*     *\n\
*     *\n\
****** \n\
*      \n\
*      \n\
*      ";

static const char* SH_GUI_LETTER_Q = "\
 ***** \n\
*     *\n\
*     *\n\
*     *\n\
*   * *\n\
*    **\n\
 ******";

static const char* SH_GUI_LETTER_R = "\
****** \n\
*     *\n\
*     *\n\
****** \n\
*    * \n\
*     *\n\
*     * ";

static const char* SH_GUI_LETTER_S = "\
 ***** \n\
*     *\n\
*      \n\
 ***** \n\
      *\n\
*     *\n\
 ***** ";

static const char* SH_GUI_LETTER_T = "\
*******\n\
   *   \n\
   *   \n\
   *   \n\
   *   \n\
   *   \n\
   *   ";

static const char* SH_GUI_LETTER_U = "\
*     *\n\
*     *\n\
*     *\n\
*     *\n\
*     *\n\
*     *\n\
 *****  ";

static const char* SH_GUI_LETTER_V = "\
*     *\n\
*     *\n\
*     *\n\
 *   * \n\
 *   * \n\
  * *  \n\
   *   ";

static const char* SH_GUI_LETTER_W = "\
*     *\n\
*     *\n\
*  *  *\n\
*  *  *\n\
* * * *\n\
* * * *\n\
 ** **  ";

static const char* SH_GUI_LETTER_X = "\
*     *\n\
*     *\n\
 *   * \n\
   *   \n\
 *   * \n\
*     *\n\
*     * ";

static const char* SH_GUI_LETTER_Y = "\
*     *\n\
*     *\n\
 *   * \n\
  * *  \n\
   *   \n\
   *   \n\
   *    ";

static const char* SH_GUI_LETTER_Z = "\
*******\n\
     * \n\
    *  \n\
   *   \n\
  *    \n\
 *     \n\
******* ";

static const char* SH_GUI_NUMBER_0 = "\
  ***  \n\
 *   * \n\
*     *\n\
*     *\n\
*     *\n\
 *   * \n\
  ***  ";

static const char* SH_GUI_NUMBER_1 = "\
   *   \n\
  **   \n\
 * *   \n\
   *   \n\
   *   \n\
   *   \n\
 ***** ";

static const char* SH_GUI_NUMBER_2 = "\
 ***** \n\
*     *\n\
     * \n\
   **  \n\
  *    \n\
 *     \n\
*******";

static const char* SH_GUI_NUMBER_3 = "\
 ***** \n\
*     *\n\
      *\n\
  **** \n\
      *\n\
*     *\n\
 *****  ";

static const char* SH_GUI_NUMBER_4 = "\
   **  \n\
  * *  \n\
 *  *  \n\
*   *  \n\
*******\n\
    *  \n\
    *   ";

static const char* SH_GUI_NUMBER_5 = "\
*******\n\
*      \n\
*      \n\
****** \n\
      *\n\
*     *\n\
 *****  ";

static const char* SH_GUI_NUMBER_6 = "\
  **** \n\
 *     \n\
*      \n\
****** \n\
*     *\n\
*     *\n\
 *****  ";

static const char* SH_GUI_NUMBER_7 = "\
*******\n\
      *\n\
     * \n\
    *  \n\
   *   \n\
  *    \n\
 *      ";

static const char* SH_GUI_NUMBER_8 = "\
 ***** \n\
*     *\n\
*     *\n\
 ***** \n\
*     *\n\
*     *\n\
 ***** ";

static const char* SH_GUI_NUMBER_9 = "\
 ***** \n\
*     *\n\
*     *\n\
 ******\n\
      *\n\
*     *\n\
 *****  ";

static const char* SH_GUI_SPACE = "\
       \n\
       \n\
       \n\
       \n\
       \n\
       \n\
       ";

static const char* SH_GUI_TAB = "\
       \n\
       \n\
       \n\
       \n\
       \n\
       \n\
       ";

static const char* SH_GUI_NEWLINE = "\
       \n\
       \n\
       \n\
       \n\
       \n\
       \n\
       ";

static const char* SH_GUI_PERIOD = "\
       \n\
       \n\
       \n\
       \n\
       \n\
  **   \n\
  **   ";

static const char* SH_GUI_COMMA = "\
       \n\
       \n\
       \n\
       \n\
       \n\
  **   \n\
 **    ";

static const char* SH_GUI_EXCLAMATION = "\
   *   \n\
   *   \n\
   *   \n\
   *   \n\
   *   \n\
       \n\
   *   ";

static const char* SH_GUI_QUESTION = "\
 ****  \n\
*    * \n\
     * \n\
    *  \n\
   *   \n\
       \n\
   *   ";

static const char* SH_GUI_COLON = "\
       \n\
       \n\
   *   \n\
       \n\
       \n\
       \n\
   *   ";

static const char* SH_GUI_SEMICOLON = "\
       \n\
       \n\
   *   \n\
       \n\
       \n\
  **   \n\
 **    ";

static const char* SH_GUI_FORWARD_SLASH = "\
      *\n\
     * \n\
    *  \n\
   *   \n\
  *    \n\
 *     \n\
*      ";

static const char* SH_GUI_BACKSLASH = "\
*      \n\
 *     \n\
  *    \n\
   *   \n\
    *  \n\
     * \n\
      *";

static const char* SH_GUI_UNDERSCORE = "\
       \n\
       \n\
       \n\
       \n\
       \n\
       \n\
*******";

static const char* SH_GUI_HYPHEN = "\
       \n\
       \n\
       \n\
*******\n\
       \n\
       \n\
       ";

static const char* SH_GUI_VERTICAL_BAR = "\
   *   \n\
   *   \n\
   *   \n\
   *   \n\
   *   \n\
   *   \n\
   *    ";

static const char* SH_GUI_AMPERSAND = "\
 ****  \n\
*    * \n\
*    * \n\
 ****  \n\
* * * *\n\
*  ** *\n\
 ** ***";

static const char* SH_GUI_PERCENT = "\
**    *\n\
**   * \n\
    *  \n\
   *   \n\
  *    \n\
 *   **\n\
*    **";

static const char* SH_GUI_DOLLAR = "\
  ***  \n\
 * * * \n\
 * *   \n\
  ***  \n\
   * * \n\
 * * * \n\
  ***  ";

static const char* SH_GUI_HASH = "\
       \n\
   *   \n\
*******\n\
 *   * \n\
*******\n\
   *   \n\
       ";

static const char* SH_GUI_AT = "\
  ***  \n\
 *   * \n\
*  *** \n\
* * * *\n\
* *****\n\
 *     \n\
  ***  ";

static const char* SH_GUI_SINGLE_QUOTE = "\
 *     \n\
 *     \n\
       \n\
       \n\
       \n\
       \n\
       ";

static const char* SH_GUI_DOUBLE_QUOTE = "\
* *    \n\
* *    \n\
       \n\
       \n\
       \n\
       \n\
       ";

static const char* SH_GUI_PLUS = "\
   *   \n\
   *   \n\
   *   \n\
*******\n\
   *   \n\
   *   \n\
   *   ";

static const char* SH_GUI_MINUS = "\
       \n\
       \n\
       \n\
*******\n\
       \n\
       \n\
       ";

static const char* SH_GUI_ASTERISK = "\
       \n\
 ** ** \n\
  ***  \n\
*******\n\
  ***  \n\
 ** ** \n\
       ";

static const char* SH_GUI_SLASH2 = "\
*      \n\
 *     \n\
  *    \n\
   *   \n\
    *  \n\
     * \n\
      *";

static const char* SH_GUI_EQUALS = "\
       \n\
       \n\
*******\n\
       \n\
*******\n\
       \n\
       ";

static const char* SH_GUI_LESS_THAN = "\
     * \n\
    *  \n\
   *   \n\
  *    \n\
   *   \n\
    *  \n\
     * ";

static const char* SH_GUI_GREATER_THAN = "\
*      \n\
 *     \n\
  *    \n\
   *   \n\
  *    \n\
 *     \n\
*      ";

static const char* SH_GUI_LEFT_PAREN = "\
  *    \n\
 *     \n\
*      \n\
*      \n\
*      \n\
 *     \n\
  *    ";

static const char* SH_GUI_RIGHT_PAREN = "\
   *   \n\
    *  \n\
     * \n\
     * \n\
     * \n\
    *  \n\
   *   ";

static const char* SH_GUI_LEFT_BRACKET = "\
  **** \n\
  *    \n\
  *    \n\
  *    \n\
  *    \n\
  *    \n\
  **** ";

static const char* SH_GUI_RIGHT_BRACKET = "\
 ****  \n\
    *  \n\
    *  \n\
    *  \n\
    *  \n\
    *  \n\
 ****  ";

static const char* SH_GUI_TILDE = "\
       \n\
 **    \n\
*  *   \n\
       \n\
       \n\
       \n\
       ";

static const char* SH_GUI_GRAVE_ACCENT = "\
 *     \n\
  *    \n\
   *   \n\
       \n\
       \n\
       \n\
       ";



#ifdef __cplusplus
}
#endif//__cplusplus

#endif//_SH_GUI_FONTS_H
