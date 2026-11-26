/* the country database that mdmCountryCodeOpen gives the modem.
 * CountryDbGetData reads it as a stream of bytes. so every field is a byte
 * array, with no padding, and each word in one is big-endian. each country's
 * record is followed by its S-register limits */
#ifndef NETWORK_COUNTRY_IMAGE_H
#define NETWORK_COUNTRY_IMAGE_H

#include <sg_xpt.h>
#include "../modem/countrydb.h"

struct country_wire_interval {
    Uint8 maximum[2], minimum[2];
};

struct country_wire_cadence {
    struct country_wire_interval on1, off1, on2, off2;
};

struct country_wire_settings {
    char name[32];
    Uint8 field_20[2];
    struct {
        Uint8 level, table, on, off;
    } tone;
    struct {
        Uint8 normal_break[2], fast_break[2], pause[2], fast_pause[2], format;
    } pulse;
    struct country_wire_cadence busy, ringback, unobtainable, congested,
        dialtone;
    struct {
        Uint8 initial[4], subsequent[4];
    } dialtone_timeout;
    struct {
        Uint8 option_1, option_2, s35_default;
    } pump;
    struct {
        Uint8 off[2], on[2];
    } ring;
    struct {
        Uint8 options[2], delay_after_failures[2], delay_seconds[2],
            bar_after_failures[2], bar_after_minutes[2],
            expire_after_minutes[2];
        Uint8 count_failure_events[2], reserved_failure_events[2],
            clear_number_events[2], clear_all_events[2];
    } blacklist;
    struct {
        Uint8 redial[2], answer_tone[2], blind_dial[2], mute[2], unmute[2],
            hook_flash[2];
    } timing;
    struct {
        Uint8 options[2], reset_flags[2], handshake_tones[2];
    } handshake;
};

struct country_wire_record {
    Uint8 selector;
    Uint8 record_size;
    Uint8 code;
    struct country_wire_settings settings;
    Uint8 limit_bytes;
};

struct modem_country_image {
    Uint8 size[4];
    Uint8 count;
    struct country_wire_record usa;
    struct Slimit usa_limits[4];
    struct country_wire_record germany;
    struct Slimit germany_limits[8];
    struct country_wire_record uk;
    struct Slimit uk_limits[8];
    struct country_wire_record france;
    struct Slimit france_limits[8];
    struct country_wire_record sweden;
    struct Slimit sweden_limits[8];
    struct country_wire_record swiss;
    struct Slimit swiss_limits[8];
    struct country_wire_record japan;
    struct Slimit japan_limits[8];
    struct country_wire_record austria;
    struct Slimit austria_limits[8];
    struct country_wire_record belgium;
    struct Slimit belgium_limits[8];
    struct country_wire_record denmark;
    struct Slimit denmark_limits[8];
    struct country_wire_record italy;
    struct Slimit italy_limits[8];
    struct country_wire_record finland;
    struct Slimit finland_limits[8];
    struct country_wire_record norway;
    struct Slimit norway_limits[8];
    struct country_wire_record australia;
    struct Slimit australia_limits[9];
    struct country_wire_record greece;
    struct Slimit greece_limits[8];
    struct country_wire_record india;
    struct Slimit india_limits[7];
    struct country_wire_record ireland;
    struct Slimit ireland_limits[8];
    struct country_wire_record netherlands;
    struct Slimit netherlands_limits[8];
    struct country_wire_record newzealand;
    struct Slimit newzealand_limits[10];
    struct country_wire_record portugal;
    struct Slimit portugal_limits[8];
    struct country_wire_record luxembourg;
    struct Slimit luxembourg_limits[8];
    struct country_wire_record iceland;
    struct Slimit iceland_limits[8];
    struct country_wire_record spain;
    struct Slimit spain_limits[8];
    Uint8 reserved[91];
};

#endif
