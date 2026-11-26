/* the body of eternal_engine.c's two scenery draws: the pulse lights' land
 * under PULSE_SCENERY, and the poison's otherwise. each draws the entries of
 * the camera's land blocks that lie in view. an entry is in view where its
 * center's camera z is between -radius and SCENERY_DRAW_DEPTH. the pulse
 * shows texture frames 0 to 4 and back, a frame every 2 steps. the poison
 * shows all 13 in turn */
{
    Sint32 blocks;
    struct land_entry *entry;
    NJS_CNK_OBJECT *object;
    int count;
    int frame;
    NJS_POINT3 pos;
    NJS_TEXLIST texlist;

    if (camera_current_index == 0) {
        blocks = state->land_blocks[0];
    } else {
        blocks = state->land_blocks[1];
    }
#ifdef PULSE_SCENERY
    count = engine_pulse_land.count;
    entry = engine_pulse_land.entries;
    njPushMatrixEx();
    texlist = engine_pulse_texlist;
    if ((frame = (state->steps / 2) % 9) >= 5) {
        frame = 9 - frame;
    }
#else
    count = engine_poison_land.count;
    entry = engine_poison_land.entries;
    njPushMatrixEx();
    texlist = engine_poison_texlist;
    frame = (state->steps / 2) % 13;
#endif
    texlist.textures += frame;
    texlist.nbTexture = 1;
    njSetTexture(&texlist);
    while (count--) {
        if (!entry->blocks || (entry->blocks & blocks)) {
            njCalcPoint(NULL, &entry->center, &pos);
            if (pos.z < SCENERY_DRAW_DEPTH && pos.z > -entry->radius) {
                object = (NJS_CNK_OBJECT *)entry->object;
                translate((NJS_VECTOR *)object->pos);
                njRotateEx(object->ang, 0);
                njCnkEasyDrawModel(object->model);
                matrix_load_parent();
            }
        }
#ifdef PULSE_SCENERY
        entry++;
#else
        entry = entry + 1;
#endif
    }
    njPopMatrixEx();
}
