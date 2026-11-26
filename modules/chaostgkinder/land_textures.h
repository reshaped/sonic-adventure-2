/* each room's texture list, whose order the texture ids of its chunk models
 * follow. _texture_filenames holds the names its textures point at, and
 * _land_archive the archive's name */
#ifndef CHAOSTGKINDER_LAND_TEXTURES_H
#define CHAOSTGKINDER_LAND_TEXTURES_H
#include <ninja.h>

extern NJS_TEXLIST kinder_house_texlist, kinder_corridor_texlist;
extern NJS_TEXLIST kinder_classroom_texlist, kinder_health_center_texlist;
extern NJS_TEXLIST kinder_library_texlist, kinder_principal_texlist;

extern const char kinder_house_texture_filenames[585];
extern const char kinder_house_land_archive[21];
extern const char kinder_corridor_texture_filenames[643];
extern const char kinder_corridor_land_archive[21];
extern const char kinder_classroom_texture_filenames[331];
extern const char kinder_classroom_land_archive[21];
extern const char kinder_health_center_texture_filenames[483];
extern const char kinder_health_center_land_archive[21];
extern const char kinder_library_texture_filenames[615];
extern const char kinder_library_land_archive[21];
extern const char kinder_principal_texture_filenames[654];
extern const char kinder_principal_land_archive[21];

#endif
