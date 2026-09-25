/*
 *
 * Copyright (C) 2026 Daniel McCarthy <daniel@dragonzap.com>
  * This file is drerived from the PeachOS Kernel (github.com/nibblebits/PeachOS).
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <https://www.gnu.org/licenses/>.
 * 
 * See NOTICE for the full list of GPL-derived files in this project.
 */

#ifndef DISK_STREAMER_H
#define DISK_STREAMER_H

#include "disk.h"

struct disk_stream {
    int pos;
    struct disk *disk;
};

struct disk_stream *disk_streamer_new(char disk_id);
int disk_streamer_seek(struct disk_stream *stream, int pos);
int disk_streamer_read(struct disk_stream *stream, void *out, int total);
int disk_streamer_write(struct disk_stream *stream, void *in, int total);
void disk_stream_close(struct disk_stream *stream);

#endif