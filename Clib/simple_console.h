/*
 * simple_console.h - Console manipulation for Eiffel
 * Copyright (c) 2025 Larry Rix - MIT License
 */

#ifndef SIMPLE_CONSOLE_H
#define SIMPLE_CONSOLE_H

#include <windows.h>

/* Color constants (foreground) */
#define SC_BLACK        0
#define SC_DARK_BLUE    1
#define SC_DARK_GREEN   2
#define SC_DARK_CYAN    3
#define SC_DARK_RED     4
#define SC_DARK_MAGENTA 5
#define SC_DARK_YELLOW  6
#define SC_GRAY         7
#define SC_DARK_GRAY    8
#define SC_BLUE         9
#define SC_GREEN        10
#define SC_CYAN         11
#define SC_RED          12
#define SC_MAGENTA      13
#define SC_YELLOW       14
#define SC_WHITE        15

/* Set text color (foreground + background*16) */
int sc_set_color(int color);

/* Set foreground color only */
int sc_set_foreground(int color);

/* Set background color only */
int sc_set_background(int color);

/* Reset to default colors */
int sc_reset_color(void);

/* Get current color attribute */
int sc_get_color(void);

/* Set cursor position (0-based) */
int sc_set_cursor(int x, int y);

/* Get cursor X position */
int sc_get_cursor_x(void);

/* Get cursor Y position */
int sc_get_cursor_y(void);

/* Get console width */
int sc_get_width(void);

/* Get console height */
int sc_get_height(void);

/* Clear screen */
int sc_clear(void);

/* Clear to end of line */
int sc_clear_line(void);

/* Set console title */
int sc_set_title(const char* title);

/* Show/hide cursor */
int sc_show_cursor(int visible);

/* Is cursor visible? */
int sc_is_cursor_visible(void);

/* Check if we have a real Windows console (not mintty/pipe) */
int sc_has_real_console(void);

#endif /* SIMPLE_CONSOLE_H */
