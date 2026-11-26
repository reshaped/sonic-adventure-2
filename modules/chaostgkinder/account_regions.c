#include "chaostgkinder/account_regions.h"
#include "chaostgkinder/widget.h"

/* an item's message is its list's first message plus key - 1. a district's
 * key counts across all of its country's districts, and each region takes a
 * slice of them */
#pragma section ATA
static struct kinder_list_item_desc japan[47] = {
#include "chaostgkinder/account_regions/japan.inc"
};

static struct kinder_list_item_desc united_states[51] = {
#include "chaostgkinder/account_regions/united_states.inc"
};

static struct kinder_list_item_desc canada[13] = {
#include "chaostgkinder/account_regions/canada.inc"
};

static struct kinder_list_item_desc country12[46] = {
#include "chaostgkinder/account_regions/country12.inc"
};

static struct kinder_list_item_desc country13[9] = {
#include "chaostgkinder/account_regions/country13.inc"
};

static struct kinder_list_item_desc country14[8] = {
#include "chaostgkinder/account_regions/country14.inc"
};

static struct kinder_list_item_desc france[22] = {
#include "chaostgkinder/account_regions/france.inc"
};

static struct kinder_list_item_desc departments[96] = {
#include "chaostgkinder/account_regions/departments.inc"
};

static struct account_districts french_regions[22] = {
#include "chaostgkinder/account_regions/french_regions.inc"
};

static struct kinder_list_item_desc germany[16] = {
#include "chaostgkinder/account_regions/germany.inc"
};

static struct kinder_list_item_desc spain[17] = {
#include "chaostgkinder/account_regions/spain.inc"
};

static struct kinder_list_item_desc spanish_provinces[50] = {
#include "chaostgkinder/account_regions/spanish_provinces.inc"
};

static struct account_districts spanish_regions[17] = {
#include "chaostgkinder/account_regions/spanish_regions.inc"
};

static struct kinder_list_item_desc italy[20] = {
#include "chaostgkinder/account_regions/italy.inc"
};

static struct kinder_list_item_desc italian_provinces[101] = {
#include "chaostgkinder/account_regions/italian_provinces.inc"
};

static struct account_districts italian_regions[20] = {
#include "chaostgkinder/account_regions/italian_regions.inc"
};

static struct account_regions by_country[33] = {
#include "chaostgkinder/account_regions/by_country.inc"
};

static struct kinder_list_item_desc countries[33] = {
#include "chaostgkinder/account_regions/countries.inc"
};

#pragma section
struct account_countries kinder_profile_countries = {
#include "chaostgkinder/account_regions/kinder_profile_countries.inc"
};
