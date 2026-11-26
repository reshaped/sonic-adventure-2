/* The sampled floor supplies height, tilt and texture origin for the
 * projected disc. The two edge vertices close each sector against the center.
 */
pos = CORE_GLOW_FLOOR(task)->pos;
floor = &CORE_GLOW_FLOOR(task)->sides[0];
pos.y = floor->pos;
origin_u = pos.x;
origin_v = pos.z;
alpha = (int)(work->scl.x * 255.0F);
if (alpha > 255) {
    alpha = 255;
}
if (alpha < 0) {
    alpha = 0;
}
color = ((Uint32)alpha << 24) | 0xffffff;
pitch = floor->ang_x;
roll = floor->ang_z;
angle = 0;
#ifdef CORE_TAILS_FLOOR
/* @hack redundant: the tilt selects a clock value the draw never reads. */
unused_clock = roll ? dat_8c0db20c.steps : 0;
#endif
for (GLOW_SECTOR_INDEX = 0; GLOW_SECTOR_INDEX < GLOW_SECTORS;
     ++GLOW_SECTOR_INDEX) {
    vertex_index = GLOW_SECTOR_INDEX * 3;
#ifdef CORE_TAILS_FLOOR
    (vertex = &vertices[vertex_index])->x = 0.0F;
#else
    vertex = &vertices[vertex_index];
    vertex->x = 0.0F;
#endif
    vertex->y = 0.0F;
    vertex->z = 0.0F;
    vertex->col = color_multiply(core_glow_center_color, color);
    vertices[vertex_index].u = glow_uv(origin_u);
    vertices[vertex_index].v = glow_uv(origin_v);
#define GLOW_RIM_VERTEX()                                                      \
    sine = njSin(angle);                                                       \
    cosine = njCos(angle);                                                     \
    GLOW_NARROW_RIM;                                                           \
    ++vertex_index;                                                            \
    vertices[vertex_index].x = core_floor_probe_reach * sine;                  \
    vertices[vertex_index].y = 0.0F;                                           \
    vertices[vertex_index].z = core_floor_probe_reach * cosine;                \
    vertices[vertex_index].col = color_multiply(core_glow_edge_color, color);  \
    vertices[vertex_index].u = glow_uv(origin_u + vertices[vertex_index].x);   \
    vertices[vertex_index].v = glow_uv(origin_v + vertices[vertex_index].z);
    GLOW_RIM_VERTEX();
    angle += FULL_TURN / GLOW_SECTORS;
    GLOW_RIM_VERTEX();
#undef GLOW_RIM_VERTEX
}
#ifdef CORE_TAILS_FLOOR
unused_clock = pitch ? dat_8c0db20c.steps : 0;
#endif
vertex = &vertices[GLOW_VERTICES - 1];
vertex->x = 0.0F;
vertex->y = 0.0F;
vertex->z = 0.0F;
vertex->col = color_multiply(core_glow_center_color, color);
vertices[GLOW_VERTICES - 1].u = glow_uv(origin_u);
vertices[GLOW_VERTICES - 1].v = glow_uv(origin_v);
/* @hack redundant: a disabled test retains the texture coordinate and
 * vertex. */
if (0) {
    if (origin_u && vertex) {
    }
}
njSetTexture(&GLOW_TEXLIST);
njFogDisable();
njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
draw_control_set(GLOW_CONTROL);
njPushMatrixEx();
njTranslate(NULL, pos.x + world_gravity.x, pos.y + world_gravity.y,
            pos.z + world_gravity.z);
GLOW_ROLL(roll);
njRotateX(NULL, pitch);
njSetTextureNum(core_glow_texture);
njTextureClampMode(NJD_TEXTURECLAMP_NOCLAMP);
njTextureFlipMode(NJD_TEXTUREFLIP_FLIP_UV);
njDrawTexture3DEx(vertices, GLOW_VERTICES, 1);
njTextureFlipMode(NJD_TEXTUREFLIP_NOFLIP);
njTextureClampMode(NJD_TEXTURECLAMP_CLAMP_UV);
njPopMatrixEx();
draw_control_clear(GLOW_CONTROL);
njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
njFogEnable();
#ifndef CORE_TAILS_FLOOR
/* @hack redundant: a disabled test retains the sector index. */
if (0) {
    if (GLOW_SECTOR_INDEX) {
    }
}
#endif
