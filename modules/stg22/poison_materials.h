#ifndef STG22_POISON_MATERIALS_H
#define STG22_POISON_MATERIALS_H

/* four texture names that the poison tank's lists and its fragments' share.
 * each field pads its name to a multiple of 4 bytes */
struct poison_material_names {
    char miu128_cg005[16];
    char miu128_cg007[16];
    char miu64_cg001[12];
    char miu64_cg009[12];
};
extern const struct poison_material_names gadget_poison_materials;

#endif
