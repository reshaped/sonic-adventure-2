/* Street bench wreck in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_STREET_BENCH_WRECK_H
#define SHARED_STREET_FIXTURES_STREET_BENCH_WRECK_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME street_bench_wreck_textures[3] = {
#include "shared/street_fixtures/street_bench_wreck/street_bench_wreck_textures.inc"
};

static NJS_TEXLIST street_bench_wreck_texlist = {
#include "shared/street_fixtures/street_bench_wreck/street_bench_wreck_texlist.inc"
};

static Sint16 street_bench_wreck_polygons[249] = {
#include "shared/street_fixtures/street_bench_wreck/street_bench_wreck_polygons.inc"
};

static Sint32 street_bench_wreck_vertices[243] = {
#include "shared/street_fixtures/street_bench_wreck/street_bench_wreck_vertices.inc"
};

static NJS_CNK_MODEL street_bench_wreck_mesh = {
#include "shared/street_fixtures/street_bench_wreck/street_bench_wreck_mesh.inc"
};

static NJS_CNK_OBJECT street_bench_wreck_object = {
#include "shared/street_fixtures/street_bench_wreck/street_bench_wreck_object.inc"
};

#endif
