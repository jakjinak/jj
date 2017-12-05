#ifndef JJ_DEFINES_H
#define JJ_DEFINES_H

#ifdef JJ_CUSTOM_DEFINES_FILE
#undef JJ_CUSTOM_INCLUDER_HELPER
#define JJ_CUSTOM_INCLUDER_HELPER(x) #x
#include JJ_CUSTOM_INCLUDER_HELPER(JJ_CUSTOM_DEFINES_FILE)
#undef JJ_CUSTOM_INCLUDER_HELPER
#endif // JJ_CUSTOM_DEFINES_FILE

#define JJ_DEFINED_VALUE_GUI_NONE 0
#define JJ_DEFINED_VALUE_GUI_WX 1

#ifndef JJ_USE_GUI
#define JJ_USE_GUI JJ_DEFINED_VALUE_GUI_WX
#endif // JJ_USE_GUI

#endif // JJ_DEFINES_H
