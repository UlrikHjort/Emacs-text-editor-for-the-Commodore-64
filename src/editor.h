/***************************************************************************
--         Commodore 64 Emacs Editor - shared definitions
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
 * Key-code notes (cc65 C64 PETSCII mapping):
 *   Ctrl+letter: C64 KERNAL generates (PETSCII_letter & 0x1F)
 *     PETSCII lowercase letters are 65-90, so CTRL+A = 65&31 = 1, etc.
 *   Caveats:
 *     CTRL+N (14) switches C64 charset at IRQ level; may not reach cgetc().
 *     CTRL+S (19) = same code as HOME key; we avoid it as a binding.
 *   Cursor / special keys return raw PETSCII codes (not affected by cc65
 *   char-literal translation, which only touches printable letters).
 */
#ifndef EDITOR_H
#define EDITOR_H

#define PROG_VERSION  "0.1"

/* Buffer limits */
#define MAX_LINES     200
#define MAX_LINE_LEN   79

/* Screen layout (40x25) */
#define SCREEN_COLS    40
#define TEXT_ROWS      23   /* rows  0-22 : text content    */
#define MODE_ROW       23   /* row 23     : mode line       */
#define MINI_ROW       24   /* row 24     : mini-buffer     */

/* Editor state machine */
#define STATE_EDIT     0    /* normal editing               */
#define STATE_CTRLX    1    /* C-x prefix received          */
#define STATE_MINIBUF  2    /* reading input in mini-buffer */

/* What to do when mini-buffer input is confirmed */
#define MBA_NONE       0
#define MBA_OPEN       1    /* C-x f : open file            */
#define MBA_SAVEAS     2    /* C-x w : save as              */

/* Emacs control codes (Ctrl+letter = PETSCII_letter & 0x1F) */
#define CTRL_A    1         /* beginning-of-line            */
#define CTRL_B    2         /* backward-char                */
#define CTRL_D    4         /* delete-char (forward)        */
#define CTRL_E    5         /* end-of-line                  */
#define CTRL_F    6         /* forward-char                 */
#define CTRL_G    7         /* keyboard-quit / cancel       */
#define CTRL_K   11         /* kill-line                    */
#define CTRL_L   12         /* recenter / redraw            */
#define CTRL_O   15         /* open-line                    */
#define CTRL_P   16         /* previous-line                */
#define CTRL_V   22         /* scroll-down (page)           */
#define CTRL_W   23         /* kill-region                  */
#define CTRL_X   24         /* prefix key                   */
#define CTRL_Y   25         /* yank                         */

/* PETSCII special keys */
#define KEY_RETURN   13
#define KEY_STOP      3     /* RUN/STOP                     */
#define KEY_DEL      20     /* DEL / backspace              */
#define KEY_UP      145
#define KEY_DOWN     17
#define KEY_LEFT    157
#define KEY_RIGHT    29
#define KEY_HOME     19
#define KEY_F1      133     /* extra cancel / exit insert   */

/* ---- Global editor state ---- */

extern char  lines[MAX_LINES][MAX_LINE_LEN + 1];
extern int   num_lines;
extern int   cur_row;
extern int   cur_col;
extern int   top_row;
extern int   state;          /* STATE_*                     */
extern char  filename[40];
extern char  modified;
extern int   running;
extern char  echo_msg[41];   /* one-shot mini-buffer text   */
extern char  kill_buf[MAX_LINE_LEN + 1];

/* Mini-buffer input fields */
extern char  mb_prompt[20];
extern char  mb_input[40];
extern int   mb_len;
extern int   mb_action;      /* MBA_*                       */

/* ---- Function prototypes ---- */

/* editor.c */
void editor_init        (void);
void editor_process_key (unsigned char c);

/* display.c */
void display_init       (void);
void display_full       (void);
void display_line       (int lnum);
void display_from       (int lnum);
void display_mode_line  (void);
void display_minibuf    (void);
void display_set_cursor (void);

/* fileio.c */
int  file_load (const char *name);
int  file_save (const char *name);

#endif /* EDITOR_H */
