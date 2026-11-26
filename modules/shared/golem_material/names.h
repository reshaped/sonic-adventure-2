/* the texture names of the Egg Golem's art. the fights' object texture lists
 * and the models' own lists point at the same names. each field's size rounds
 * its name up to a multiple of 4 bytes */
#ifndef SHARED_GOLEM_MATERIAL_NAMES_H
#define SHARED_GOLEM_MATERIAL_NAMES_H

struct golem_effect_names {
    char bs0_ml1[8];
    char ef_beam00[12];
    char ef_beam10[12];
    char ef_gl_lockon[16];
    char emecha1[8];
    char ew22[8];
    char eww06[8];
    char gl_bobin[12];
};
extern const struct golem_effect_names golem_effect_materials;

struct golem_masonry_names {
    char kd_bs1_kabe1[16];
    char kd_bs1_kabe2[16];
    char kd_bs1_kabe3[16];
    char kd_bs1_kabe4[16];
    char kd_bs1_kabe7[16];
    char kd_hb_hasira3[16];
    char kd_hb_hasira5[16];
    char kd_hb_kage1[12];
    char kd_hb_sekihi1[16];
};
extern const struct golem_masonry_names golem_masonry_materials;

struct golem_impact_names {
    char nc_kyo_isi01[16];
    char pika01[8];
    char pika02[8];
    char pika03[8];
    char pika04[8];
    char pika05[8];
    char pika06[8];
    char pika07[8];
    char pika08[8];
    char pika09[8];
};
extern const struct golem_impact_names golem_impact_materials;

struct golem_mechanism_names {
    char gl_weakpoint[16];
    char gl_w_light_b[16];
    char gl_w_light_y[16];
    char gl_w_mech0[12];
    char gl_w_mech1[12];
};
extern const struct golem_mechanism_names golem_mechanism_materials;

extern const char golem_flash_texture[12];

extern const char golem_face_textures[2][12];

extern const char golem_magic_ring_textures[2][12];

extern const char golem_wave_texture[12];

#endif
