/*
 * Raw Video Codec
 * Copyright (c) 2001 Fabrice Bellard
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * Raw Video Codec
 */

#include "avcodec.h"
#include "raw.h"
#include "libavutil/common.h"

const PixelFormatTag ff_raw_pix_fmt_tags[] = {
    { AV_PIX_FMT_YUV420P, MKTAG('I', '4', '2', '0') }, /* Planar formats */
    { AV_PIX_FMT_YUV420P, MKTAG('I', 'Y', 'U', 'V') },
    { AV_PIX_FMT_YUV420P, MKTAG('Y', 'V', '1', '2') },
    { AV_PIX_FMT_YUV410P, MKTAG('Y', 'U', 'V', '9') },
    { AV_PIX_FMT_YUV410P, MKTAG('Y', 'V', 'U', '9') },
    { AV_PIX_FMT_YUV411P, MKTAG('Y', '4', '1', 'B') },
    { AV_PIX_FMT_YUV422P, MKTAG('Y', '4', '2', 'B') },
    { AV_PIX_FMT_YUV422P, MKTAG('P', '4', '2', '2') },
    { AV_PIX_FMT_YUV422P, MKTAG('Y', 'V', '1', '6') },
    /* yuvjXXX formats are deprecated hacks specific to libav*,
       they are identical to yuvXXX  */
    { AV_PIX_FMT_YUVJ420P, MKTAG('I', '4', '2', '0') }, /* Planar formats */
    { AV_PIX_FMT_YUVJ420P, MKTAG('I', 'Y', 'U', 'V') },
    { AV_PIX_FMT_YUVJ420P, MKTAG('Y', 'V', '1', '2') },
    { AV_PIX_FMT_YUVJ422P, MKTAG('Y', '4', '2', 'B') },
    { AV_PIX_FMT_YUVJ422P, MKTAG('P', '4', '2', '2') },
    { AV_PIX_FMT_GRAY8,    MKTAG('Y', '8', '0', '0') },
    { AV_PIX_FMT_GRAY8,    MKTAG('Y', '8', ' ', ' ') },

    { AV_PIX_FMT_YUYV422, MKTAG('Y', 'U', 'Y', '2') }, /* Packed formats */
    { AV_PIX_FMT_YUYV422, MKTAG('Y', '4', '2', '2') },
    { AV_PIX_FMT_YUYV422, MKTAG('V', '4', '2', '2') },
    { AV_PIX_FMT_YUYV422, MKTAG('V', 'Y', 'U', 'Y') },
    { AV_PIX_FMT_YUYV422, MKTAG('Y', 'U', 'N', 'V') },
    { AV_PIX_FMT_UYVY422, MKTAG('U', 'Y', 'V', 'Y') },
    { AV_PIX_FMT_UYVY422, MKTAG('H', 'D', 'Y', 'C') },
    { AV_PIX_FMT_UYVY422, MKTAG('U', 'Y', 'N', 'V') },
    { AV_PIX_FMT_UYVY422, MKTAG('U', 'Y', 'N', 'Y') },
    { AV_PIX_FMT_UYVY422, MKTAG('u', 'y', 'v', '1') },
    { AV_PIX_FMT_UYVY422, MKTAG('2', 'V', 'u', '1') },
    { AV_PIX_FMT_UYVY422, MKTAG('A', 'V', 'R', 'n') }, /* Avid AVI Codec 1:1 */
    { AV_PIX_FMT_UYVY422, MKTAG('A', 'V', '1', 'x') }, /* Avid 1:1x */
    { AV_PIX_FMT_UYVY422, MKTAG('A', 'V', 'u', 'p') },
    { AV_PIX_FMT_UYVY422, MKTAG('V', 'D', 'T', 'Z') }, /* SoftLab-NSK VideoTizer */
    { AV_PIX_FMT_UYVY422, MKTAG('a', 'u', 'v', '2') },
    { AV_PIX_FMT_UYVY422, MKTAG('c', 'y', 'u', 'v') }, /* CYUV is also Creative YUV */
    { AV_PIX_FMT_UYYVYY411, MKTAG('Y', '4', '1', '1') },
    { AV_PIX_FMT_GRAY8,   MKTAG('G', 'R', 'E', 'Y') },
    { AV_PIX_FMT_NV12,    MKTAG('N', 'V', '1', '2') },
    { AV_PIX_FMT_NV21,    MKTAG('N', 'V', '2', '1') },

    /* nut */
    { AV_PIX_FMT_RGB555LE, MKTAG('R', 'G', 'B', 15) },
    { AV_PIX_FMT_BGR555LE, MKTAG('B', 'G', 'R', 15) },
    { AV_PIX_FMT_RGB565LE, MKTAG('R', 'G', 'B', 16) },
    { AV_PIX_FMT_BGR565LE, MKTAG('B', 'G', 'R', 16) },
    { AV_PIX_FMT_RGB555BE, MKTAG(15 , 'B', 'G', 'R') },
    { AV_PIX_FMT_BGR555BE, MKTAG(15 , 'R', 'G', 'B') },
    { AV_PIX_FMT_RGB565BE, MKTAG(16 , 'B', 'G', 'R') },
    { AV_PIX_FMT_BGR565BE, MKTAG(16 , 'R', 'G', 'B') },
    { AV_PIX_FMT_RGB444LE, MKTAG('R', 'G', 'B', 12) },
    { AV_PIX_FMT_BGR444LE, MKTAG('B', 'G', 'R', 12) },
    { AV_PIX_FMT_RGB444BE, MKTAG(12 , 'B', 'G', 'R') },
    { AV_PIX_FMT_BGR444BE, MKTAG(12 , 'R', 'G', 'B') },
    { AV_PIX_FMT_RGBA64LE, MKTAG('R', 'B', 'A', 64 ) },
    { AV_PIX_FMT_BGRA64LE, MKTAG('B', 'R', 'A', 64 ) },
    { AV_PIX_FMT_RGBA64BE, MKTAG(64 , 'R', 'B', 'A') },
    { AV_PIX_FMT_BGRA64BE, MKTAG(64 , 'B', 'R', 'A') },
    { AV_PIX_FMT_RGBA,     MKTAG('R', 'G', 'B', 'A') },
    { AV_PIX_FMT_RGB0,     MKTAG('R', 'G', 'B',  0 ) },
    { AV_PIX_FMT_BGRA,     MKTAG('B', 'G', 'R', 'A') },
    { AV_PIX_FMT_BGR0,     MKTAG('B', 'G', 'R',  0 ) },
    { AV_PIX_FMT_ABGR,     MKTAG('A', 'B', 'G', 'R') },
    { AV_PIX_FMT_0BGR,     MKTAG( 0 , 'B', 'G', 'R') },
    { AV_PIX_FMT_ARGB,     MKTAG('A', 'R', 'G', 'B') },
    { AV_PIX_FMT_0RGB,     MKTAG( 0 , 'R', 'G', 'B') },
    { AV_PIX_FMT_RGB24,    MKTAG('R', 'G', 'B', 24 ) },
    { AV_PIX_FMT_BGR24,    MKTAG('B', 'G', 'R', 24 ) },
    { AV_PIX_FMT_YUV411P,  MKTAG('4', '1', '1', 'P') },
    { AV_PIX_FMT_YUV422P,  MKTAG('4', '2', '2', 'P') },
    { AV_PIX_FMT_YUVJ422P, MKTAG('4', '2', '2', 'P') },
    { AV_PIX_FMT_YUV440P,  MKTAG('4', '4', '0', 'P') },
    { AV_PIX_FMT_YUVJ440P, MKTAG('4', '4', '0', 'P') },
    { AV_PIX_FMT_YUV444P,  MKTAG('4', '4', '4', 'P') },
    { AV_PIX_FMT_YUVJ444P, MKTAG('4', '4', '4', 'P') },
    { AV_PIX_FMT_MONOWHITE,MKTAG('B', '1', 'W', '0') },
    { AV_PIX_FMT_MONOBLACK,MKTAG('B', '0', 'W', '1') },
    { AV_PIX_FMT_BGR8,     MKTAG('B', 'G', 'R',  8 ) },
    { AV_PIX_FMT_RGB8,     MKTAG('R', 'G', 'B',  8 ) },
    { AV_PIX_FMT_BGR4,     MKTAG('B', 'G', 'R',  4 ) },
    { AV_PIX_FMT_RGB4,     MKTAG('R', 'G', 'B',  4 ) },
    { AV_PIX_FMT_RGB4_BYTE,MKTAG('B', '4', 'B', 'Y') },
    { AV_PIX_FMT_BGR4_BYTE,MKTAG('R', '4', 'B', 'Y') },
    { AV_PIX_FMT_RGB48LE,  MKTAG('R', 'G', 'B', 48 ) },
    { AV_PIX_FMT_RGB48BE,  MKTAG( 48, 'R', 'G', 'B') },
    { AV_PIX_FMT_BGR48LE,  MKTAG('B', 'G', 'R', 48 ) },
    { AV_PIX_FMT_BGR48BE,  MKTAG( 48, 'B', 'G', 'R') },
    { AV_PIX_FMT_GRAY16LE,    MKTAG('Y', '1',  0 , 16 ) },
    { AV_PIX_FMT_GRAY16BE,    MKTAG(16 ,  0 , '1', 'Y') },
    { AV_PIX_FMT_YUV420P10LE, MKTAG('Y', '3', 11 , 10 ) },
    { AV_PIX_FMT_YUV420P10BE, MKTAG(10 , 11 , '3', 'Y') },
    { AV_PIX_FMT_YUV422P10LE, MKTAG('Y', '3', 10 , 10 ) },
    { AV_PIX_FMT_YUV422P10BE, MKTAG(10 , 10 , '3', 'Y') },
    { AV_PIX_FMT_YUV444P10LE, MKTAG('Y', '3',  0 , 10 ) },
    { AV_PIX_FMT_YUV444P10BE, MKTAG(10 ,  0 , '3', 'Y') },
    { AV_PIX_FMT_YUV420P12LE, MKTAG('Y', '3', 11 , 12 ) },
    { AV_PIX_FMT_YUV420P12BE, MKTAG(12 , 11 , '3', 'Y') },
    { AV_PIX_FMT_YUV422P12LE, MKTAG('Y', '3', 10 , 12 ) },
    { AV_PIX_FMT_YUV422P12BE, MKTAG(12 , 10 , '3', 'Y') },
    { AV_PIX_FMT_YUV444P12LE, MKTAG('Y', '3',  0 , 12 ) },
    { AV_PIX_FMT_YUV444P12BE, MKTAG(12 ,  0 , '3', 'Y') },
    { AV_PIX_FMT_YUV420P14LE, MKTAG('Y', '3', 11 , 14 ) },
    { AV_PIX_FMT_YUV420P14BE, MKTAG(14 , 11 , '3', 'Y') },
    { AV_PIX_FMT_YUV422P14LE, MKTAG('Y', '3', 10 , 14 ) },
    { AV_PIX_FMT_YUV422P14BE, MKTAG(14 , 10 , '3', 'Y') },
    { AV_PIX_FMT_YUV444P14LE, MKTAG('Y', '3',  0 , 14 ) },
    { AV_PIX_FMT_YUV444P14BE, MKTAG(14 ,  0 , '3', 'Y') },
    { AV_PIX_FMT_YUV420P16LE, MKTAG('Y', '3', 11 , 16 ) },
    { AV_PIX_FMT_YUV420P16BE, MKTAG(16 , 11 , '3', 'Y') },
    { AV_PIX_FMT_YUV422P16LE, MKTAG('Y', '3', 10 , 16 ) },
    { AV_PIX_FMT_YUV422P16BE, MKTAG(16 , 10 , '3', 'Y') },
    { AV_PIX_FMT_YUV444P16LE, MKTAG('Y', '3',  0 , 16 ) },
    { AV_PIX_FMT_YUV444P16BE, MKTAG(16 ,  0 , '3', 'Y') },
    { AV_PIX_FMT_YUVA420P,    MKTAG('Y', '4', 11 ,  8 ) },
    { AV_PIX_FMT_YUVA422P,    MKTAG('Y', '4', 10 ,  8 ) },
    { AV_PIX_FMT_YUVA444P,    MKTAG('Y', '4',  0 ,  8 ) },
    { AV_PIX_FMT_GRAY8A,      MKTAG('Y', '2',  0 ,  8 ) },

    /* quicktime */
    { AV_PIX_FMT_YUV420P, MKTAG('R', '4', '2', '0') }, /* Radius DV YUV PAL */
    { AV_PIX_FMT_YUV411P, MKTAG('R', '4', '1', '1') }, /* Radius DV YUV NTSC */
    { AV_PIX_FMT_UYVY422, MKTAG('2', 'v', 'u', 'y') },
    { AV_PIX_FMT_UYVY422, MKTAG('2', 'V', 'u', 'y') },
    { AV_PIX_FMT_UYVY422, MKTAG('A', 'V', 'U', 'I') }, /* FIXME merge both fields */
    { AV_PIX_FMT_UYVY422, MKTAG('b', 'x', 'y', 'v') },
    { AV_PIX_FMT_YUYV422, MKTAG('y', 'u', 'v', '2') },
    { AV_PIX_FMT_YUYV422, MKTAG('y', 'u', 'v', 's') },
    { AV_PIX_FMT_YUYV422, MKTAG('D', 'V', 'O', 'O') }, /* Digital Voodoo SD 8 Bit */
    { AV_PIX_FMT_RGB555LE,MKTAG('L', '5', '5', '5') },
    { AV_PIX_FMT_RGB565LE,MKTAG('L', '5', '6', '5') },
    { AV_PIX_FMT_RGB565BE,MKTAG('B', '5', '6', '5') },
    { AV_PIX_FMT_BGR24,   MKTAG('2', '4', 'B', 'G') },
    { AV_PIX_FMT_BGR24,   MKTAG('b', 'x', 'b', 'g') },
    { AV_PIX_FMT_BGRA,    MKTAG('B', 'G', 'R', 'A') },
    { AV_PIX_FMT_RGBA,    MKTAG('R', 'G', 'B', 'A') },
    { AV_PIX_FMT_RGB24,   MKTAG('b', 'x', 'r', 'g') },
    { AV_PIX_FMT_ABGR,    MKTAG('A', 'B', 'G', 'R') },
    { AV_PIX_FMT_GRAY16BE,MKTAG('b', '1', '6', 'g') },
    { AV_PIX_FMT_RGB48BE, MKTAG('b', '4', '8', 'r') },

    /* special */
    { AV_PIX_FMT_RGB565LE,MKTAG( 3 ,  0 ,  0 ,  0 ) }, /* flipped RGB565LE */
    { AV_PIX_FMT_YUV444P, MKTAG('Y', 'V', '2', '4') }, /* YUV444P, swapped UV */
    { AV_PIX_FMT_YUYV422, MKTAG('Y', 'V', 'Y', 'U') }, /* YUYV, swapped UV */

    { AV_PIX_FMT_NONE, 0 },
};

unsigned int avcodec_pix_fmt_to_codec_tag(enum AVPixelFormat fmt)
{
    const PixelFormatTag *tags = ff_raw_pix_fmt_tags;
    while (tags->pix_fmt >= 0) {
        if (tags->pix_fmt == fmt)
            return tags->fourcc;
        tags++;
    }
    return 0;
}
