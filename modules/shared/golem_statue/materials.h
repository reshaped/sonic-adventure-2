/* the indexes of the statue's skin textures, in the order its archive holds
 * them in both fights. golem_skin_names holds the first six names, at these
 * indexes */
#ifndef SHARED_GOLEM_STATUE_MATERIALS_H
#define SHARED_GOLEM_STATUE_MATERIALS_H

enum golem_skin_texture {
    GOLEM_TEXTURE_GL_BODY00,
    GOLEM_TEXTURE_GL_BODY01,
    GOLEM_TEXTURE_GL_BODY04,
    GOLEM_TEXTURE_GL_BODY05,
    GOLEM_TEXTURE_GL_BODY06,
    GOLEM_TEXTURE_GL_BODY07,
    GOLEM_TEXTURE_GOLEMBODY00,
    GOLEM_TEXTURE_GOLEMFACE00,
    GOLEM_SKIN_TEXTURES
};

/* the bytes that each name of golem_skin_names takes, its terminator and
 * padding included */
#define GOLEM_BODY_NAME_LENGTH 12
extern const char golem_skin_names[GOLEM_TEXTURE_GL_BODY07 + 1]
                                  [GOLEM_BODY_NAME_LENGTH];

#endif
