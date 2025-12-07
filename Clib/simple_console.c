/*
 * simple_console.c - Console manipulation for Eiffel
 * Copyright (c) 2025 Larry Rix - MIT License
 */

#include "simple_console.h"

static HANDLE get_stdout_handle(void) {
    static HANDLE h = NULL;
    static int initialized = 0;

    if (!initialized) {
        h = GetStdHandle(STD_OUTPUT_HANDLE);
        /* Try to get a real console if we're being piped */
        if (h == INVALID_HANDLE_VALUE || GetFileType(h) != FILE_TYPE_CHAR) {
            /* Try to attach to or allocate a console */
            if (AttachConsole(ATTACH_PARENT_PROCESS) || AllocConsole()) {
                h = GetStdHandle(STD_OUTPUT_HANDLE);
            }
        }
        initialized = 1;
    }
    return h;
}

static int is_real_console(void) {
    HANDLE h = get_stdout_handle();
    DWORD mode;
    if (h == INVALID_HANDLE_VALUE || h == NULL) return 0;
    return GetConsoleMode(h, &mode) ? 1 : 0;
}

static WORD default_attributes = 0;
static int default_saved = 0;

static void save_default_color(void) {
    if (!default_saved) {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(get_stdout_handle(), &csbi)) {
            default_attributes = csbi.wAttributes;
            default_saved = 1;
        } else {
            default_attributes = SC_GRAY; /* Fallback */
            default_saved = 1;
        }
    }
}

int sc_set_color(int color) {
    save_default_color();
    return SetConsoleTextAttribute(get_stdout_handle(), (WORD)color) ? 1 : 0;
}

int sc_set_foreground(int color) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    WORD attr;

    save_default_color();
    if (!GetConsoleScreenBufferInfo(get_stdout_handle(), &csbi)) {
        return 0;
    }

    /* Keep background, change foreground */
    attr = (csbi.wAttributes & 0xF0) | (color & 0x0F);
    return SetConsoleTextAttribute(get_stdout_handle(), attr) ? 1 : 0;
}

int sc_set_background(int color) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    WORD attr;

    save_default_color();
    if (!GetConsoleScreenBufferInfo(get_stdout_handle(), &csbi)) {
        return 0;
    }

    /* Keep foreground, change background */
    attr = (csbi.wAttributes & 0x0F) | ((color & 0x0F) << 4);
    return SetConsoleTextAttribute(get_stdout_handle(), attr) ? 1 : 0;
}

int sc_reset_color(void) {
    save_default_color();
    return SetConsoleTextAttribute(get_stdout_handle(), default_attributes) ? 1 : 0;
}

int sc_get_color(void) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(get_stdout_handle(), &csbi)) {
        return -1;
    }
    return (int)csbi.wAttributes;
}

int sc_set_cursor(int x, int y) {
    COORD pos;
    pos.X = (SHORT)x;
    pos.Y = (SHORT)y;
    return SetConsoleCursorPosition(get_stdout_handle(), pos) ? 1 : 0;
}

int sc_get_cursor_x(void) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(get_stdout_handle(), &csbi)) {
        return -1;
    }
    return (int)csbi.dwCursorPosition.X;
}

int sc_get_cursor_y(void) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(get_stdout_handle(), &csbi)) {
        return -1;
    }
    return (int)csbi.dwCursorPosition.Y;
}

int sc_get_width(void) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(get_stdout_handle(), &csbi)) {
        return -1;
    }
    return (int)(csbi.srWindow.Right - csbi.srWindow.Left + 1);
}

int sc_get_height(void) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(get_stdout_handle(), &csbi)) {
        return -1;
    }
    return (int)(csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
}

int sc_clear(void) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE h = get_stdout_handle();
    COORD origin = {0, 0};
    DWORD written;
    DWORD size;

    if (!GetConsoleScreenBufferInfo(h, &csbi)) {
        return 0;
    }

    size = csbi.dwSize.X * csbi.dwSize.Y;

    /* Fill with spaces */
    if (!FillConsoleOutputCharacterA(h, ' ', size, origin, &written)) {
        return 0;
    }

    /* Fill with current attributes */
    if (!FillConsoleOutputAttribute(h, csbi.wAttributes, size, origin, &written)) {
        return 0;
    }

    /* Move cursor to origin */
    SetConsoleCursorPosition(h, origin);

    return 1;
}

int sc_clear_line(void) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE h = get_stdout_handle();
    DWORD written;
    DWORD len;

    if (!GetConsoleScreenBufferInfo(h, &csbi)) {
        return 0;
    }

    len = csbi.dwSize.X - csbi.dwCursorPosition.X;

    /* Fill rest of line with spaces */
    if (!FillConsoleOutputCharacterA(h, ' ', len, csbi.dwCursorPosition, &written)) {
        return 0;
    }

    /* Fill with current attributes */
    FillConsoleOutputAttribute(h, csbi.wAttributes, len, csbi.dwCursorPosition, &written);

    return 1;
}

int sc_set_title(const char* title) {
    if (!title) return 0;
    return SetConsoleTitleA(title) ? 1 : 0;
}

int sc_show_cursor(int visible) {
    CONSOLE_CURSOR_INFO cci;
    HANDLE h = get_stdout_handle();

    if (!GetConsoleCursorInfo(h, &cci)) {
        return 0;
    }

    cci.bVisible = visible ? TRUE : FALSE;
    return SetConsoleCursorInfo(h, &cci) ? 1 : 0;
}

int sc_is_cursor_visible(void) {
    CONSOLE_CURSOR_INFO cci;
    if (!GetConsoleCursorInfo(get_stdout_handle(), &cci)) {
        return 1; /* Assume visible on error */
    }
    return cci.bVisible ? 1 : 0;
}

int sc_has_real_console(void) {
    return is_real_console();
}
