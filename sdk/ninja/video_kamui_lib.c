/* the display's setup: how the frame buffer is written and read, and the
 * video signal's timing. horizontal timings count pixel clocks, and vertical
 * ones count lines */
#include <kamui.h>
#include <kamui_internal.h>

/* the video chip's registers, as the offsets kmiReadRegister and
 * kmiVerifyWriteRegister take */
#define VBLANK_INTERRUPT 0x0cc
#define SYNC_CONTROL 0x0d0
#define HBLANK 0x0d4
#define SYNC_LOAD 0x0d8
#define VBLANK 0x0dc
#define SYNC_WIDTH 0x0e0
/* KMI_FB_READ_CONTROL's bit that reads at the full pixel clock, as VGA does */
#define READ_FULL_CLOCK_SHIFT 23
#define READ_FULL_CLOCK (1 << READ_FULL_CLOCK_SHIFT)
/* KMI_SYNC_STATUS's bits that hold the line being sent */
#define SYNC_LINE 0x3ff

/* the scaler's vertical factors, in 1024ths: one, one and a 1024th, and two */
#define SCALE_ONE 0x400
#define SCALE_FIELD 0x401
#define SCALE_DOUBLE 0x800
/* a tile is TILE_SIZE pixels square, and a frame at most TILE_ROWS tiles
 * tall */
#define TILE_SIZE 32
#define TILE_ROWS 15

/* @hack spelling: an if that assigns in each branch, where field = test ?
 * when_set : when_clear would do */
#define SET_IF_ELSE(field, test, when_set, when_clear)                         \
    do {                                                                       \
        if (test) {                                                            \
            (field) = (when_set);                                              \
        } else {                                                               \
            (field) = (when_clear);                                            \
        }                                                                      \
    } while (0)

/* the pixel formats the renderer writes, and those the display reads */
#define WRITE_RGB555 0
#define WRITE_RGB565 1
#define WRITE_ARGB1555 3
#define WRITE_RGB888 4
#define WRITE_ARGB8888 6
#define READ_RGB555 0
#define READ_RGB565 1
#define READ_RGB888 2
#define READ_RGB0888 3

struct frame_setup {
    KMINT32 read_width; /* one less than a line's 32-bit words */
    KMINT32 read_lines; /* one less than the lines read */
    KMINT32 modulus; /* one more than the 32-bit words skipped between lines */
    KMINT32 half_width;
    KMINT32 vertical_scale;
    KMINT32 filter_a;
    KMINT32 filter_b;
    KMINT32 scaler_field;
    KMINT32 read_enable;
    KMINT32 interlace;
    KMINT32 repeat_lines;
    KMINT32 dither;
    KMINT32 write_format;
    KMINT32 pad_value;
    KMINT32 alpha_cutoff;
    KMINT32 read_format;
    KMINT32 concat;
    KMINT32 write_stride; /* the 64-bit words of a line the renderer writes */
    KMINT32 video_burst;
    KMINT32 video_latency;
    KMINT32 write_burst;
    KMINT32 max_x;
    KMINT32 min_x;
    KMINT32 max_y;
    KMINT32 min_y;
    KMINT32 strip_buffer;
    KMINT32 strip_lines;
    KMINT32 full_clock;
    KMINT32 key_threshold;
};

struct scan_setup {
    KMINT32 hblank_end;
    KMINT32 hblank_start;
    KMINT32 line_clocks;
    KMINT32 vblank_end;
    KMINT32 vblank_start;
    KMINT32 lines;
    KMINT32 hsync_width;
    KMINT32 vsync_width;
    KMINT32 broad_width;
    KMINT32 equalize_width;
    KMINT32 ntsc;
    KMINT32 pal;
    KMINT32 interlace;
    KMINT32 hsync_polarity;
    KMINT32 vsync_polarity;
    KMINT32 blank_polarity;
    KMINT32 blank_output;
    KMINT32 field_source;
    KMINT32 drive_sync;
    KMINT32 composite_on_hsync;
    KMINT32 master_hsync;
    KMINT32 master_vsync;
    KMINT32 master_blank;
    KMINT32 master_csync;
    KMINT32 second_field_only;
    KMINT32 compare_lines;
    KMINT32 sync_lock;
    KMINT32 repeat_pixels;
    KMINT32 pixel_delay;
    KMINT32 vblank_start_interrupt;
    KMINT32 vblank_end_interrupt;
};

struct display_setup {
    struct scan_setup scan;
    struct frame_setup frame;
    KMINT32 origin_y1;
    KMINT32 origin_y2;
    KMINT32 origin_x;
};

/* the member exports this function, though no header of the SDK declares it */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void kmiDACControl(KMINT32 level, KMBOOLEAN green_sync, KMBOOLEAN power_down);

/* a register's two 11-bit fields: low from bit 0, and high from bit 16 */
#define PACK_PAIR(low, high) (((low) & 0x7ff) | (((high) & 0x7ff) << 16))

static void store_frame(struct frame_setup frame)
{
    kmiDeviceExtension.fb_clip_x = PACK_PAIR(frame.min_x, frame.max_x);
    kmiDeviceExtension.fb_clip_y = PACK_PAIR(frame.min_y, frame.max_y);
    kmiDeviceExtension.fb_burst_control = (frame.video_burst & 15) |
                                          ((frame.video_latency & 31) << 4) |
                                          ((frame.write_burst & 15) << 9);
    kmiDeviceExtension.fb_width = frame.read_width;
    kmiDeviceExtension.fb_lines = frame.read_lines;
    if (kmiDeviceExtension.offset_y > 0) {
        kmiDeviceExtension.fb_read_size =
            (frame.read_width & 0x3ff) |
            (((frame.read_lines - kmiDeviceExtension.offset_y) & 0x3ff) << 10) |
            ((frame.modulus & 0x3ff) << 20);
    } else {
        kmiDeviceExtension.fb_read_size = (frame.read_width & 0x3ff) |
                                          ((frame.read_lines & 0x3ff) << 10) |
                                          ((frame.modulus & 0x3ff) << 20);
    }
    kmiDeviceExtension.y_coefficients =
        (KMBYTE)frame.filter_a | ((KMBYTE)frame.filter_b << 8);
    kmiDeviceExtension.scaler_control = (unsigned short)frame.vertical_scale |
                                        ((frame.half_width & 1) << 16) |
                                        ((frame.scaler_field & 1) << 18);
    kmiDeviceExtension.fb_write_stride = frame.write_stride & 0x1ff;
    kmiDeviceExtension.fb_write_control =
        (frame.write_format & 7) | ((frame.dither & 1) << 3) |
        ((KMBYTE)frame.pad_value << 8) | ((KMBYTE)frame.alpha_cutoff << 16);
    kmiDeviceExtension.fb_read_control =
        (frame.read_enable & 1) | ((frame.repeat_lines & 1) << 1) |
        ((frame.read_format & 3) << 2) | ((frame.concat & 7) << 4) |
        ((KMBYTE)frame.key_threshold << 8) | ((frame.strip_lines & 31) << 16) |
        ((frame.strip_buffer & 1) << 22) |
        ((frame.full_clock & 1) << READ_FULL_CLOCK_SHIFT);
}

KMINT32 kmiFBControl(KMINT32 width, KMINT32 height, KMBPPMODE format,
                     KMBOOLEAN flicker_filter, KMBOOLEAN dither,
                     KMBOOLEAN anti_alias)
{
    struct frame_setup frame;
    KMINT32 pixel_bits[] = {16, 16, 16, 16, 24, 32};
    KMINT32 pal_field[] = {990, 959, 930, 903, 877};
    KMINT32 pal_double[] = {1981, 1920, 1861, 1807, 1755};
    KMINT32 extra_rows;
    KMINT32 fields;
    KMINT32 vertical_factor;
    KMINT32 bits;
    KMINT32 horizontal_factor;
    KMINT32 stride;
    KMINT32 half_height;

#ifdef NONMATCHING
    /* GCC's side refuses a format past pixel_bits. neither switch below has a
     * default, so it sets their fields first */
    if ((KMUINT32)format >= sizeof(pixel_bits) / sizeof(pixel_bits[0])) {
        return KMSTATUS_INVALID_VIDEO_MODE;
    }
    frame.vertical_scale = SCALE_ONE;
    frame.write_format = WRITE_RGB555;
    frame.read_format = READ_RGB555;
    frame.dither = 0;
#endif
    bits = pixel_bits[format];
    kmiDeviceExtension.display_width = width;
    kmiDeviceExtension.display_height = height;
    if (KMI_FLICKER_FREE) {
        kmiDeviceExtension.display_height =
            kmiDeviceExtension.display_height / 2;
    }
    if (anti_alias && !(kmiDeviceExtension.display_mode & KMI_HEIGHT_480)) {
        vertical_factor = 2;
    } else {
        vertical_factor = 1;
    }
    frame.read_enable = 1;
    frame.alpha_cutoff = 0;
    frame.pad_value = 0;
    frame.concat = 0;
    frame.key_threshold = 0;
    frame.strip_lines = 0;
    frame.strip_buffer = 0;
    if (kmiDeviceExtension.display_mode & KM_VGA) {
        frame.full_clock = 1;
        fields = 1;
        extra_rows = 0;
    } else {
        frame.full_clock = 0;
        if (flicker_filter == 1) {
            fields = 2;
            extra_rows = 0;
        } else {
            fields = 1;
            extra_rows = 1;
        }
    }
    switch (bits) {
    case 16:
        if (format == KM_DSPBPP_ARGB1555) {
            frame.write_format = WRITE_ARGB1555;
            frame.read_format = READ_RGB555;
        } else if (format == KM_DSPBPP_RGB565) {
            frame.write_format = WRITE_RGB565;
            frame.read_format = READ_RGB565;
        } else if (format == KM_DSPBPP_RGB555) {
            frame.write_format = WRITE_RGB555;
            frame.read_format = READ_RGB555;
        } else {
            frame.write_format = WRITE_RGB565;
            frame.read_format = READ_RGB565;
        }
        frame.dither = dither;
        break;
    case 24:
        frame.write_format = WRITE_RGB888;
        frame.read_format = READ_RGB888;
        frame.dither = 0;
        break;
    case 32:
        frame.write_format = WRITE_ARGB8888;
        frame.read_format = READ_RGB0888;
        frame.dither = 0;
        break;
    }
    frame.repeat_lines = 0;
    frame.interlace = flicker_filter;
    frame.scaler_field = 0;
    frame.write_stride = width * bits / 64;
    frame.read_width = (width * bits / 32) - 1;
    half_height = height / 2;
    stride = bits / 8 * width;
    if (flicker_filter == 1) {
        if (kmiDeviceExtension.display_mode & KMI_HEIGHT_480) {
            frame.read_lines = half_height;
            frame.modulus = (width * bits / 32) + 1;
            kmiDeviceExtension.field_offset[0] = 0;
            kmiDeviceExtension.field_offset[1] = stride;
        } else {
            frame.read_lines = height;
            kmiDeviceExtension.field_offset[0] = 0;
            kmiDeviceExtension.field_offset[1] = stride;
            frame.modulus = 1;
        }
    } else if (kmiDeviceExtension.display_mode & KM_VGA) {
        frame.read_lines = height;
        kmiDeviceExtension.field_offset[0] = 0;
        kmiDeviceExtension.field_offset[1] = stride;
        frame.modulus = 1;
    } else if (kmiDeviceExtension.display_mode & KMI_HEIGHT_480) {
        frame.read_lines = half_height;
        kmiDeviceExtension.field_offset[0] = 0;
        kmiDeviceExtension.field_offset[1] = stride;
        frame.modulus = (width * bits / 32) + 1;
    } else {
        frame.read_lines = height;
        kmiDeviceExtension.field_offset[0] = 0;
        kmiDeviceExtension.field_offset[1] = stride;
        frame.modulus = 1;
    }
    frame.read_lines--;
    if (anti_alias) {
        kmiDeviceExtension.ta_options |= KMI_OPTION_BY_COLUMNS;
        frame.half_width = 1;
        horizontal_factor = 2;
        frame.read_lines--;
        if (kmiDeviceExtension.display_mode & KM_VGA) {
            frame.vertical_scale = SCALE_FIELD;
        } else {
            switch (kmiDeviceExtension.display_mode &
                    ~(KMI_WIDTH_640 | KM_VGA | KM_PAL)) {
            case KMI_EXTENDED_LINES | KMI_HEIGHT_480 | KMI_INTERLACE:
            case KMI_EXTENDED_LINES | KMI_HEIGHT_480 | KMI_PSEUDO_NONINTERLACE:
            case KMI_HEIGHT_480 | KMI_INTERLACE:
            case KMI_HEIGHT_480 | KMI_PSEUDO_NONINTERLACE:
                SET_IF_ELSE(frame.vertical_scale, KMI_PAL_EXTENDED_MODE,
                            pal_field[kmiDeviceExtension.pal_selection],
                            SCALE_FIELD);
                frame.modulus = (width * bits / 32) + 1;
                break;
            case KMI_EXTENDED_LINES | KMI_HEIGHT_480:
            case KMI_EXTENDED_LINES:
            case KMI_EXTENDED_LINES | KMI_INTERLACE:
            case KMI_HEIGHT_480:
            case 0:
            case KMI_INTERLACE:
                kmiDeviceExtension.field_offset[0] = 0;
                kmiDeviceExtension.field_offset[1] = 0;
                SET_IF_ELSE(frame.vertical_scale, KMI_PAL_EXTENDED_MODE,
                            pal_double[kmiDeviceExtension.pal_selection],
                            SCALE_DOUBLE);
                frame.modulus = 1;
                break;
            }
        }
    } else {
        frame.half_width = 0;
        if (KMI_FLICKER_FREE) {
            SET_IF_ELSE(frame.vertical_scale, KMI_PAL_EXTENDED_MODE,
                        pal_double[kmiDeviceExtension.pal_selection],
                        SCALE_DOUBLE);
            kmiDeviceExtension.field_offset[0] = 0;
            kmiDeviceExtension.field_offset[1] = 0;
            frame.modulus = 1;
            frame.read_lines--;
        } else if (kmiDeviceExtension.display_mode & KM_VGA) {
            frame.vertical_scale = SCALE_ONE;
        } else if (kmiDeviceExtension.display_mode & KMI_HEIGHT_480) {
            SET_IF_ELSE(frame.vertical_scale, KMI_PAL_EXTENDED_MODE,
                        pal_field[kmiDeviceExtension.pal_selection],
                        SCALE_FIELD);
            frame.read_lines--;
        } else if (KMI_PAL_EXTENDED_MODE) {
            frame.vertical_scale = pal_field[kmiDeviceExtension.pal_selection];
            frame.read_lines--;
        } else {
            frame.vertical_scale = SCALE_ONE;
        }
        horizontal_factor = 1;
    }
    frame.filter_a = 64;
    frame.filter_b = 128;
    kmiDeviceExtension.tiles_x = width / TILE_SIZE * horizontal_factor;
    kmiDeviceExtension.tiles_y =
        (height * fields * vertical_factor / TILE_SIZE) + extra_rows;
    if (kmiDeviceExtension.tiles_y > TILE_ROWS) {
        kmiDeviceExtension.tiles_y = TILE_ROWS;
    }
    frame.video_latency = 9;
    frame.video_burst = 6;
    SET_IF_ELSE(frame.write_burst, bits == 16, 8, 11);
    frame.min_x = 0;
    frame.max_x = width - 1;
    frame.min_y = 0;
    SET_IF_ELSE(frame.max_y, KMI_FLICKER_FREE, half_height - 1, height - 1);
    store_frame(frame);
    kmiDeviceExtension.skip_lines = 0;
    kmiDeviceExtension.level_adjust = 0;
    return 0;
}

void kmiDACControl(KMINT32 level, KMBOOLEAN green_sync, KMBOOLEAN power_down)
{
}

static void program_signal(struct display_setup display)
{
    kmiDeviceExtension.origin_x = display.origin_x;
    kmiDeviceExtension.origin_y1 = display.origin_y1;
    kmiDeviceExtension.origin_y2 = display.origin_y2;
    kmiDeviceExtension.sync_control =
        display.scan.master_hsync | (display.scan.master_vsync << 1) |
        (display.scan.master_csync << 2) | (display.scan.sync_lock << 3) |
        (display.scan.interlace << 4) | (display.scan.second_field_only << 5) |
        (display.scan.ntsc << 6) | (display.scan.pal << 7) |
        (display.scan.drive_sync << 8) | (display.scan.composite_on_hsync << 9);
    kmiDeviceExtension.hblank =
        PACK_PAIR(display.scan.hblank_start, display.scan.hblank_end);
    kmiDeviceExtension.hblank_interrupt = (display.scan.hblank_start & 0x7ff)
                                          << 16;
    kmiDeviceExtension.sync_load =
        PACK_PAIR(display.scan.line_clocks, display.scan.lines);
    kmiDeviceExtension.vblank =
        PACK_PAIR(display.scan.vblank_start, display.scan.vblank_end);
    kmiDeviceExtension.vblank_interrupt = PACK_PAIR(
        display.scan.vblank_start_interrupt, display.scan.vblank_end_interrupt);
    kmiDeviceExtension.sync_width = (display.scan.hsync_width & 127) |
                                    ((display.scan.vsync_width & 15) << 8) |
                                    ((display.scan.broad_width & 0x3ff) << 12) |
                                    ((display.scan.equalize_width & 127) << 22);
    kmiDeviceExtension.video_control =
        (display.scan.hsync_polarity & 1) |
        ((display.scan.vsync_polarity & 1) << 1) |
        ((display.scan.blank_polarity & 1) << 2) |
        ((display.scan.blank_output & 1) << 3) |
        ((display.scan.field_source & 15) << 4) |
        ((display.scan.repeat_pixels & 1) << 8) |
        ((display.scan.pixel_delay & 63) << 16);
    kmiDeviceExtension.video_start_x =
        kmiDeviceExtension.origin_x + kmiDeviceExtension.offset_x;
    kmiDeviceExtension.video_start_y =
        (kmiDeviceExtension.origin_y1 + kmiDeviceExtension.offset_y) |
        ((kmiDeviceExtension.origin_y2 + kmiDeviceExtension.offset_y) << 16);
    if (kmiDeviceExtension.no_wait_display_change != 1 &&
        (kmiDeviceExtension.capabilities & KMI_CAN_READ_SYNC)) {
        while (!(kmiReadRegister(KMI_SYNC_STATUS) & SYNC_LINE)) {
        }
        while (kmiReadRegister(KMI_SYNC_STATUS) & SYNC_LINE) {
        }
    }
    kmiVerifyWriteRegister(KMI_VIDEO_CONTROL,
                           kmiDeviceExtension.video_control | KMI_VIDEO_BLANK);
    if (kmiDeviceExtension.video_mode & KM_VGA) {
        kmiVerifyWriteRegister(KMI_FB_READ_CONTROL, READ_FULL_CLOCK);
    } else {
        kmiVerifyWriteRegister(KMI_FB_READ_CONTROL, 0);
    }
    kmiVerifyWriteRegister(KMI_FB_READ_SIZE, 0);
    kmiVerifyWriteRegister(SYNC_CONTROL, kmiDeviceExtension.sync_control);
    kmiVerifyWriteRegister(SYNC_LOAD, kmiDeviceExtension.sync_load);
    kmiVerifyWriteRegister(VBLANK_INTERRUPT,
                           kmiDeviceExtension.vblank_interrupt);
    kmiVerifyWriteRegister(HBLANK, kmiDeviceExtension.hblank);
    kmiVerifyWriteRegister(VBLANK, kmiDeviceExtension.vblank);
    kmiVerifyWriteRegister(SYNC_WIDTH, kmiDeviceExtension.sync_width);
    kmiVerifyWriteRegister(KMI_VIDEO_START_X, kmiDeviceExtension.video_start_x);
    kmiVerifyWriteRegister(KMI_VIDEO_START_Y, kmiDeviceExtension.video_start_y);
    kmiVerifyWriteRegister(KMI_HBLANK_INTERRUPT,
                           kmiDeviceExtension.hblank_interrupt);
}

#define NORMAL_POLARITY(scan)                                                  \
    {                                                                          \
        (scan).hsync_polarity = 0;                                             \
        (scan).vsync_polarity = 0;                                             \
        (scan).blank_polarity = 0;                                             \
        (scan).blank_output = 0;                                               \
    }
#define INTERNAL_SYNC(scan)                                                    \
    {                                                                          \
        (scan).drive_sync = 1;                                                 \
        (scan).composite_on_hsync = 0;                                         \
        (scan).master_hsync = 0;                                               \
        (scan).master_vsync = 0;                                               \
        (scan).master_blank = 0;                                               \
        (scan).master_csync = 0;                                               \
        (scan).sync_lock = 0;                                                  \
    }
#define PAL_BLANK_LINES(scan)                                                  \
    {                                                                          \
        if (KMI_TWO_FIELDS) {                                                  \
            (scan).vblank_end = 44;                                            \
            (scan).vblank_start = 620;                                         \
        } else {                                                               \
            (scan).vblank_end = 22;                                            \
            (scan).vblank_start = 308;                                         \
        }                                                                      \
        if (kmiDeviceExtension.display_mode & KMI_HEIGHT_480) {                \
            (scan).vblank_start_interrupt = 308;                               \
        } else {                                                               \
            (scan).vblank_start_interrupt = 310;                               \
        }                                                                      \
        (scan).vblank_end_interrupt = 21;                                      \
    }

void kmiInitPAL(void)
{
    struct display_setup display;
    display.scan.line_clocks = 863;
    display.scan.hblank_end = 141;
    display.scan.hblank_start = 843;
    display.scan.hsync_width = 63;
    SET_IF_ELSE(display.scan.lines, KMI_TWO_FIELDS, 624, 312);
    /* @hack redundant: the KMI_PAL_EXTENDED_MODE test, whose two branches are
     * alike */
    if (KMI_PAL_EXTENDED_MODE) {
        PAL_BLANK_LINES(display.scan);
    } else {
        PAL_BLANK_LINES(display.scan);
    }
    if (KMI_TWO_FIELDS) {
        display.scan.broad_width = 362;
        display.scan.equalize_width = 31;
        display.scan.vsync_width = 5;
    } else {
        display.scan.broad_width = 799;
        display.scan.equalize_width = 15;
        display.scan.vsync_width = 3;
    }
    SET_IF_ELSE(display.scan.interlace, KMI_TWO_FIELDS, 1, 0);
    NORMAL_POLARITY(display.scan);
    SET_IF_ELSE(display.scan.field_source, KMI_TWO_FIELDS, 0, 2);
    SET_IF_ELSE(display.scan.pixel_delay, kmiDeviceExtension.core == 6, 22, 63);
    SET_IF_ELSE(display.scan.repeat_pixels,
                kmiDeviceExtension.display_mode & KMI_WIDTH_640, 0, 1);
    display.scan.ntsc = 0;
    display.scan.pal = 1;
    INTERNAL_SYNC(display.scan);
    display.scan.second_field_only = 0;
    display.scan.compare_lines = 240;
    display.origin_x = 174;
    if (KMI_PAL_EXTENDED_MODE) {
        KMINT32 origin_by_height[] = {42, 38, 34, 30, 26};
        display.origin_y2 = origin_by_height[kmiDeviceExtension.pal_selection];
        SET_IF_ELSE(display.origin_y1, KMI_TWO_FIELDS, display.origin_y2 - 1,
                    display.origin_y2);
        kmiDeviceExtension.picture_lines =
            kmiDeviceExtension.pal_lines[kmiDeviceExtension.pal_selection] / 2;
    } else {
        if (KMI_TWO_FIELDS) {
            display.origin_y1 = 45;
            display.origin_y2 = 46;
        } else {
            display.origin_y1 = 46;
            display.origin_y2 = 46;
        }
        kmiDeviceExtension.picture_lines = 240;
    }
    kmiDeviceExtension.picture_end =
        (display.scan.vblank_end / 2) + kmiDeviceExtension.picture_lines;
    kmiDeviceExtension.picture_start = display.scan.vblank_end;
    program_signal(display);
}

void kmiInitNTSC(void)
{
    struct display_setup display;
    display.scan.line_clocks = 857;
    SET_IF_ELSE(display.scan.lines, KMI_TWO_FIELDS, 524, 262);
    display.scan.hblank_end = 126;
    display.scan.hblank_start = 837;
    if (KMI_TWO_FIELDS) {
        display.scan.vblank_end = 36;
        display.scan.vblank_start = 516;
    } else {
        display.scan.vblank_end = 18;
        display.scan.vblank_start = 258;
    }
    display.scan.hsync_width = 63;
    if (kmiDeviceExtension.display_mode & KMI_HEIGHT_480) {
        display.scan.vblank_start_interrupt = 260;
        display.scan.vblank_end_interrupt = 21;
    } else {
        display.scan.vblank_start_interrupt = 258;
        display.scan.vblank_end_interrupt = 21;
    }
    SET_IF_ELSE(display.scan.vsync_width, KMI_TWO_FIELDS, 6, 3);
    SET_IF_ELSE(display.scan.broad_width, KMI_TWO_FIELDS, 364, 793);
    SET_IF_ELSE(display.scan.equalize_width, KMI_TWO_FIELDS, 31, 15);
    display.scan.ntsc = 1;
    display.scan.pal = 0;
    SET_IF_ELSE(display.scan.interlace, KMI_TWO_FIELDS, 1, 0);
    NORMAL_POLARITY(display.scan);
    display.scan.field_source = 0;
    SET_IF_ELSE(display.scan.pixel_delay, kmiDeviceExtension.core == 6, 22, 31);
    SET_IF_ELSE(display.scan.repeat_pixels,
                kmiDeviceExtension.display_mode & KMI_WIDTH_640, 0, 1);
    INTERNAL_SYNC(display.scan);
    display.origin_x = 164;
    display.scan.compare_lines = 480;
    display.scan.second_field_only = 0;
    if (KMI_TWO_FIELDS) {
        display.origin_y1 = 18;
        display.origin_y2 = 18;
    } else {
        display.origin_y1 = 17;
        display.origin_y2 = 18;
    }
    kmiDeviceExtension.picture_lines = 240;
    kmiDeviceExtension.picture_end =
        (display.scan.vblank_end / 2) + kmiDeviceExtension.picture_lines;
    kmiDeviceExtension.picture_start = display.scan.vblank_end;
    program_signal(display);
}

void kmiInitVGA(void)
{
    struct display_setup display;
    display.scan.line_clocks = 857;
    display.scan.lines = 524;
    display.scan.hblank_end = 126;
    display.scan.hblank_start = 837;
    display.scan.vblank_end = 40;
    display.scan.vblank_start = 520;
    display.scan.hsync_width = 63;
    display.scan.vsync_width = 3;
    display.scan.broad_width = 793;
    display.scan.equalize_width = 15;
    display.scan.vblank_start_interrupt = 520;
    display.scan.vblank_end_interrupt = 21;
    NORMAL_POLARITY(display.scan);
    display.scan.field_source = 0;
    SET_IF_ELSE(display.scan.pixel_delay, kmiDeviceExtension.core == 6, 22, 31);
    display.scan.repeat_pixels = 0;
    display.scan.ntsc = 0;
    display.scan.pal = 0;
    display.scan.interlace = 0;
    INTERNAL_SYNC(display.scan);
    display.scan.second_field_only = 0;
    display.scan.compare_lines = 480;
    /* @hack redundant: the first three origins, which the next three
     * overwrite */
    display.origin_x = 144;
    display.origin_y1 = 35;
    display.origin_y2 = 35;
    display.origin_x = 168;
    display.origin_y1 = 40;
    display.origin_y2 = 40;
    kmiDeviceExtension.picture_lines = 480;
    kmiDeviceExtension.picture_end =
        display.scan.vblank_end + kmiDeviceExtension.picture_lines;
    kmiDeviceExtension.picture_start = display.scan.vblank_end;
    program_signal(display);
}

KMINT32 kmiEnableVSyncInterrupt(KMBOOLEAN enable)
{
    return 1;
}

KMINT32 kmiEnableHSyncInterrupt(KMBOOLEAN enable)
{
    return 1;
}
