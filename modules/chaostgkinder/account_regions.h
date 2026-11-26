/* the countries the profile editor offers, with their regions and the
 * regions' districts. an item's key counts from 1, and its message is its
 * name's kinder_string number. a country's regions are regions[key - 1], and
 * a region's districts are districts[key - 1]. items is NULL for a country
 * without regions, and districts for one whose regions have none */
#ifndef CHAOSTGKINDER_ACCOUNT_REGIONS_H
#define CHAOSTGKINDER_ACCOUNT_REGIONS_H
#include <sg_xpt.h>

struct kinder_list_item_desc;

struct account_districts {
    Sint32 count;
    const struct kinder_list_item_desc *items;
};
struct account_regions {
    Sint32 count;
    const struct kinder_list_item_desc *items;
    const struct account_districts *districts;
};
struct account_countries {
    Sint32 count;
    const struct kinder_list_item_desc *items;
    const struct account_regions *regions;
};
extern struct account_countries kinder_profile_countries;
#endif
