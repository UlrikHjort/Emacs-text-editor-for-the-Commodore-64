/***************************************************************************
--      Commodore 64 Emacs Editor - disk file I/O via CBM kernal
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
 * Files are stored as sequential (SEQ) files on the disk drive.
 * Lines are separated by PETSCII carriage-return (0x0D).
 *
 * Device 8 is assumed (first disk drive).
 *
 * IMPORTANT - cc65 PETSCII char-literal mapping for C64 target:
 *   Uppercase C literals ('S','R','W') map to PETSCII 193-218 (shifted range).
 *   CBM DOS expects file-type/mode letters in PETSCII 65-90 (unshifted range).
 *   So all CBM DOS command letters MUST use lowercase C literals:
 *     ",s,r"  ->  bytes 44, 83, 44, 82   (correct for CBM DOS)
 *     ",s,w"  ->  bytes 44, 83, 44, 87   (correct for CBM DOS)
 *
 * Secondary address 2 is the standard data channel for sequential files
 * opened with explicit ",s,r" / ",s,w" mode in the filename string.
 */
#include <string.h>
#include <cbm.h>
#include "editor.h"

#define DISK_DEVICE  8
#define FILE_LFN     2     /* logical file number used for data */
#define FILE_SA      2     /* secondary address: data channel   */

/* ---- file_load ---- */

int file_load (const char *name) {
    unsigned char buf[1];
    int  line = 0;
    int  col  = 0;
    int  r;
    char open_name[48];

    /* "0:name,s,r"  - lowercase 's','r' = PETSCII 83,82 as CBM DOS expects */
    strcpy(open_name, "0:");
    strncat(open_name, name, 43);
    strcat(open_name, ",s,r");

    if (cbm_open(FILE_LFN, DISK_DEVICE, FILE_SA, open_name) != 0)
        return -1;

    /* Clear the buffer */
    lines[0][0] = '\0';
    num_lines   = 0;

    while (1) {
        r = cbm_read(FILE_LFN, buf, 1);
        if (r <= 0) break;   /* 0 = EOF, -1 = error */

        if (buf[0] == 0x0D || buf[0] == 0x0A) {
            /* End of line */
            lines[line][col] = '\0';
            line++;
            col = 0;
            if (line >= MAX_LINES) break;
            lines[line][0] = '\0';
        } else {
            if (col < MAX_LINE_LEN)
                lines[line][col++] = (char)buf[0];
        }
    }

    cbm_close(FILE_LFN);

    /* Finish last line */
    lines[line][col] = '\0';
    num_lines = line + 1;

    /* Always keep at least one line */
    if (num_lines == 0) {
        lines[0][0] = '\0';
        num_lines   = 1;
    }

    return 0;
}

/* ---- file_save ---- */

int file_save (const char *name) {
    int  i;
    int  len;
    char open_name[48];
    unsigned char cr = 0x0D;

    /* "@0:name,s,w" - '@' = replace existing, lowercase 's','w' = PETSCII 83,87 */
    strcpy(open_name, "@0:");
    strncat(open_name, name, 42);
    strcat(open_name, ",s,w");

    if (cbm_open(FILE_LFN, DISK_DEVICE, FILE_SA, open_name) != 0)
        return -1;

    for (i = 0; i < num_lines; i++) {
        len = (int)strlen(lines[i]);
        if (len > 0) {
            if (cbm_write(FILE_LFN, lines[i], (unsigned int)len) < 0) {
                cbm_close(FILE_LFN);
                return -1;
            }
        }
        /* Write line terminator (CR) after every line */
        cbm_write(FILE_LFN, &cr, 1);
    }

    cbm_close(FILE_LFN);
    return 0;
}
