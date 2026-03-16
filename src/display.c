/***************************************************************************
--          Commodore 64 Emacs Editor - screen rendering
--
--           Copyright (C) 2026 By Ulrik Hørlyk Hjort
--
-- Permission is hereby granted, free of charge, to any person obtaining
-- a copy of this software and associated documentation files (the
-- "Software"), to deal in the Software without restriction, including
-- without limitation the rights to use, copy, modify, merge, publish,
-- distribute, sublicense, and/or sell copies of the Software, and to
-- permit persons to whom the Software is furnished to do so, subject to
-- the following conditions:
--
-- The above copyright notice and this permission notice shall be
-- included in all copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
-- EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
-- MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
-- NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
-- LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
-- OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
-- WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
-- ***************************************************************************/
/*
 * Layout (40x25):
 *   Rows  0-22  text content   (TEXT_ROWS = 23)
 *   Row  23     mode line      (reverse video, Emacs-style)
 *   Row  24     mini-buffer    (echo area / input prompts)
 */
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include "editor.h"

static void clear_to_eol (int from_col) {
    int n = SCREEN_COLS - from_col;
    if (n > 0) cclear((unsigned char)n);
}

/* ---- display_init ---- */

void display_init (void) {
    clrscr();
    /* Brief welcome in mode line */
    revers(1);
    gotoxy(0, MODE_ROW);
    cputs("C64 Emacs " PROG_VERSION " -- C-x f:open  C-x s:save");
    cclear((unsigned char)(SCREEN_COLS - 38));
    revers(0);
}

/* ---- display_line ---- */

void display_line (int lnum) {
    int scr_row = lnum - top_row;
    int col = 0;
    char *p;

    if (scr_row < 0 || scr_row >= TEXT_ROWS) return;

    gotoxy(0, (unsigned char)scr_row);

    if (lnum < num_lines) {
        p = lines[lnum];
        while (*p && col < SCREEN_COLS) {
            cputc(*p++);
            col++;
        }
    } else {
        /* beyond end of file */
        cputc('~');
        col = 1;
    }
    clear_to_eol(col);
}

/* ---- display_from ---- */

void display_from (int lnum) {
    int r;
    int scr_start = lnum - top_row;
    if (scr_start < 0) scr_start = 0;
    for (r = scr_start; r < TEXT_ROWS; r++)
        display_line(r + top_row);
}

/* ---- display_full ---- */

void display_full (void) {
    int r;
    for (r = 0; r < TEXT_ROWS; r++)
        display_line(r + top_row);
}

/* ---- display_mode_line ----
 *
 * Format (40 chars):  -*-  FILENAME  -*---------  L12,C5
 *   -**-  when buffer is modified
 */
void display_mode_line (void) {
    char pos[12];
    char fnbuf[21];
    int  used = 0;
    int  pad;

    revers(1);
    gotoxy(0, MODE_ROW);

    /* Modified indicator */
    if (modified)
        cputs("-**- ");
    else
        cputs("---- ");
    used += 5;

    /* Filename (truncated) */
    if (filename[0] != '\0') {
        strncpy(fnbuf, filename, 20);
        fnbuf[20] = '\0';
    } else {
        strcpy(fnbuf, "*scratch*");
    }
    cputs(fnbuf);
    used += (int)strlen(fnbuf);

    /* Fill with dashes then position */
    sprintf(pos, " L%d,C%d", cur_row + 1, cur_col + 1);
    pad = SCREEN_COLS - used - (int)strlen(pos);
    while (pad-- > 0) cputc('-');
    cputs(pos);

    revers(0);
}

/* ---- display_minibuf ----
 *
 * Shows the mini-buffer / echo area at row MINI_ROW.
 *   STATE_EDIT    : echo_msg (cleared to EOL)
 *   STATE_CTRLX   : "C-x -"
 *   STATE_MINIBUF : prompt + input so far
 */
void display_minibuf (void) {
    int len = 0;

    gotoxy(0, MINI_ROW);

    if (state == STATE_MINIBUF) {
        cputs(mb_prompt);
        cputs(mb_input);
        len = (int)strlen(mb_prompt) + mb_len;
    } else if (state == STATE_CTRLX) {
        cputs("C-x -");
        len = 5;
    } else if (echo_msg[0] != '\0') {
        cputs(echo_msg);
        len = (int)strlen(echo_msg);
        echo_msg[0] = '\0';   /* consume after display */
    }
    clear_to_eol(len);
}

/* ---- display_set_cursor ---- */

void display_set_cursor (void) {
    int scr_col, scr_row;

    if (state == STATE_MINIBUF) {
        scr_col = (int)strlen(mb_prompt) + mb_len;
        if (scr_col >= SCREEN_COLS) scr_col = SCREEN_COLS - 1;
        gotoxy((unsigned char)scr_col, MINI_ROW);
    } else {
        scr_row = cur_row - top_row;
        scr_col = cur_col;
        if (scr_row < 0)          scr_row = 0;
        if (scr_row >= TEXT_ROWS) scr_row = TEXT_ROWS - 1;
        if (scr_col >= SCREEN_COLS) scr_col = SCREEN_COLS - 1;
        gotoxy((unsigned char)scr_col, (unsigned char)scr_row);
    }
}
