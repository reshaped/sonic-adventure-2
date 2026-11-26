/* Serialized Ninja SimpleDraw lists in a downloadable kart model. */
#ifndef CART_CHUNK_STREAM_H
#define CART_CHUNK_STREAM_H
#include <sg_xpt.h>

/* Offsets are relative to the unrelocated model. A zero offset omits that
 * list. On success the byte extents include the lists' end markers. */
Bool cart_chunk_streams_valid(const Uint8 *model, Uint32 size, Uint32 vertices,
                              Uint32 polygons, Uint32 textures,
                              Uint32 *vertex_bytes, Uint32 *polygon_bytes);
#endif
