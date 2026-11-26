/* the garden's animations: an entry of chao_motion_table and a label for
 * each chao_motion_id. several entries play one clip */
#include "chao/motion.h"
#include "chao/motion_clips.h"
#include "chao/motion_ids.h"
#include "draw/motion.h"

#include "chao/motion_data/text.inc"

/* an entry's pose is the Chao's while the entry plays. it is 0 standing, 1
 * sitting, 2 supine, 3 prone or 4 any other */
struct motion_table_entry chao_motion_table[CHAO_ANIM_COUNT] = {
#include "chao/motion_data/chao_motion_table.inc"
};

const char *chao_motion_names[CHAO_ANIM_COUNT] = {
#include "chao/motion_data/chao_motion_names.inc"
};
