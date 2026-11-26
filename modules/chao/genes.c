#include <sg_xpt.h>
#include <stdlib.h>
#include <string.h>

#include "chao/emotion.h"
#include "chao/face.h"
#include "chao/garden.h"
#include "chao/genes.h"
#include "chao/identity.h"
#include "chao/legacy.h"
#include "chao/life.h"
#include "chao/parts.h"
#include "chao/residents.h"
#include "chao/tuning.h"
#include "chao/work.h"
#include "system/task.h"
#include "system/util.h"

#define PERSONALITY_TRAITS (CHAO_FICKLENESS - CHAO_CURIOSITY + 1)
#define GENE_ROLL(range) ((Uint32)(rand() * ((range) * RAND_SCALE)))
/* each is 0 or 1 at random, the index of one allele of a pair. the two roll
 * on different scales */
#define EITHER_ALLELE() ((int)(rand() * (1.99F * RAND_SCALE)))
#define EXPRESSION_ALLELE() GENE_ROLL(1.9999F)
/* a personality allele is a trait / TRAIT_STEP + NEUTRAL_ALLELE, from 0 to 4 */
#define TRAIT_STEP 40
#define NEUTRAL_ALLELE 2
/* a skill shows its larger allele DOMINANT_ODDS of the time */
#define DOMINANT_ODDS 0.7F
/* a trait is in one of TRAIT_BANDS bands. band 0 lies below -TRAIT_BAND,
 * band 1 below TRAIT_BAND, and band 2 from TRAIT_BAND up */
#define TRAIT_BANDS 3
#define TRAIT_BAND 35
/* each range a new gene's alleles roll below. a skill's run from 0 to 4,
 * and a lifespan's from 0 to 9. a personality trait's run from 0 to 4. a
 * taste's, a TV's and music's run from 0 to 5. a hero gene's alignment
 * alleles are HERO_ALLELE, and a dark gene's -HERO_ALLELE */
#define SKILL_ALLELES 4.9999F
#define LIFE_ALLELES 9.9999F
#define TRAIT_ALLELES 4.9999F
#define TASTE_ALLELES 5.9999F
#define HERO_ALLELE 5

/* a hatchling's eye and mouth, by its bands of aggression, kindness and
 * curiosity */
/* clang-format off */
static Sint8 default_eyes[TRAIT_BANDS][TRAIT_BANDS][TRAIT_BANDS] = {
    {{CHAO_EYE_HALF_SHUT, CHAO_EYE_OPEN, CHAO_EYE_OPEN},
     {CHAO_EYE_HALF_SHUT, CHAO_EYE_OPEN, CHAO_EYE_OPEN},
     {CHAO_EYE_HALF_SHUT, CHAO_EYE_OPEN, CHAO_EYE_OPEN}},
    {{CHAO_EYE_HALF_SHUT, CHAO_EYE_OPEN, CHAO_EYE_OPEN},
     {CHAO_EYE_HALF_SHUT, CHAO_EYE_OPEN, CHAO_EYE_GLAD},
     {CHAO_EYE_HALF_SHUT, CHAO_EYE_OPEN, CHAO_EYE_GLAD}},
    {{CHAO_EYE_NARROW, CHAO_EYE_NARROW, CHAO_EYE_NARROW},
     {CHAO_EYE_HALF_SHUT, CHAO_EYE_OPEN, CHAO_EYE_GLAD},
     {CHAO_EYE_OPEN, CHAO_EYE_GLAD, CHAO_EYE_GLAD}}};
static Sint8 default_mouths[TRAIT_BANDS][TRAIT_BANDS][TRAIT_BANDS] = {
    {{8, 0, 4}, {8, 8, 4}, {8, 0, 3}},
    {{0, 1, 1}, {0, 0, 0}, {0, 3, 3}},
    {{0, 1, 1}, {0, 4, 4}, {3, 3, 3}}};
/* clang-format on */

void chao_new_gene(struct chao_gene *gene)
{
    int i;

    for (i = 0; i < CHAO_SKILLS; i++) {
        gene->skill[i][0] = gene->skill[i][1] = GENE_ROLL(SKILL_ALLELES);
    }
    gene->life[0] = GENE_ROLL(LIFE_ALLELES);
    gene->life[1] = GENE_ROLL(LIFE_ALLELES);
    gene->run_power[0] = 0;
    gene->run_power[1] = 0;
    gene->fly_swim[0] = 0;
    gene->fly_swim[1] = 0;
    gene->alignment[0] = 0;
    gene->alignment[1] = 0;
    for (i = 0; i < PERSONALITY_TRAITS; i++) {
        gene->personality[i][0] = GENE_ROLL(TRAIT_ALLELES);
        gene->personality[i][1] = GENE_ROLL(TRAIT_ALLELES);
    }
    gene->taste[0] = GENE_ROLL(TASTE_ALLELES);
    gene->taste[1] = GENE_ROLL(TASTE_ALLELES);
    gene->tv[0] = GENE_ROLL(TASTE_ALLELES);
    gene->tv[1] = GENE_ROLL(TASTE_ALLELES);
    gene->music[0] = GENE_ROLL(TASTE_ALLELES);
    gene->music[1] = GENE_ROLL(TASTE_ALLELES);
    gene->color[0] = 0;
    gene->color[1] = 0;
    gene->texture[0] = 0;
    gene->texture[1] = 0;
    gene->shiny[0] = 0;
    gene->shiny[1] = 0;
}

void chao_new_hero_gene(struct chao_gene *gene)
{
    chao_new_gene(gene);
    gene->alignment[0] = HERO_ALLELE;
    gene->alignment[1] = HERO_ALLELE;
}

void chao_new_dark_gene(struct chao_gene *gene)
{
    chao_new_gene(gene);
    gene->alignment[0] = -HERO_ALLELE;
    gene->alignment[1] = -HERO_ALLELE;
}

void chao_learn_traits(struct chao_gene *gene,
                       const struct chao_profile *profile)
{
    int i;
    for (i = 0; i < CHAO_SKILLS; i++) {
        gene->skill[i][0] = gene->skill[i][1] =
            profile->parameters.stats[i] / (chao_tuning.skill_step + 1);
    }
    for (i = 0; i < PERSONALITY_TRAITS; i++) {
        gene->personality[i][EITHER_ALLELE()] =
            (profile->emotions.personality.preferences.traits[i] / TRAIT_STEP) +
            NEUTRAL_ALLELE;
    }
}

void chao_make_saved_egg(const struct chao_save *first,
                         struct chao_save *second)
{
    const struct chao_gene *first_gene = &first->record.saved.gene;
    struct chao_gene *second_gene = &second->record.saved.gene;
    struct chao_parameters *parameters = second->record.head;
    struct chao_gene child;

    chao_cross_genes(first_gene, second_gene, &child);
    *second_gene = child;
    parameters->type = CHAO_EGG;
}

/* the child's first allele of trait is either of first's, and its second
 * either of second's */
#define INHERIT(trait)                                                         \
    do {                                                                       \
        child->trait[0] = first->trait[EITHER_ALLELE()];                       \
        child->trait[1] = second->trait[EITHER_ALLELE()];                      \
    } while (0)

void chao_cross_genes(const struct chao_gene *first,
                      const struct chao_gene *second, struct chao_gene *child)
{
    int i;

    memset(child, 0, sizeof(*child));
    for (i = 0; i < CHAO_SKILLS; i++) {
        INHERIT(skill[i]);
    }
    INHERIT(life);
    INHERIT(run_power);
    INHERIT(fly_swim);
    INHERIT(alignment);
    for (i = 0; i < PERSONALITY_TRAITS; i++) {
        INHERIT(personality[i]);
    }
    INHERIT(taste);
    INHERIT(tv);
    INHERIT(music);
    INHERIT(color);
    INHERIT(texture);
    INHERIT(shiny);
}

void chao_inherit_traits(const struct task *first, const struct task *second,
                         struct chao_gene *child)
{
    const struct chao_profile *first_profile =
        &((const struct chao_work *)first->work)->profile;
    const struct chao_profile *second_profile =
        &((const struct chao_work *)second->work)->profile;
    const struct chao_gene *first_gene =
        &((const struct chao_work *)first->work)->gene;
    const struct chao_gene *second_gene =
        &((const struct chao_work *)second->work)->gene;
    /* @hack redundant: copies of the profiles, which chao_learn_traits only
     * reads, where the profiles themselves would do */
    struct chao_profile first_traits;
    struct chao_profile second_traits;
    struct chao_gene first_copy;
    struct chao_gene second_copy;

    first_traits = *first_profile;
    second_traits = *second_profile;
    first_copy = *first_gene;
    second_copy = *second_gene;
    chao_learn_traits(&first_copy, &first_traits);
    chao_learn_traits(&second_copy, &second_traits);
    chao_cross_genes(&first_copy, &second_copy, child);
}

#define TRAIT_INDEX(kind) ((kind) - CHAO_CURIOSITY)
void chao_hatch_traits(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_gene *gene = &work->gene;
    struct chao_parameters *parameters = &work->profile.parameters;
    /* @hack type: the emotions reached through parameters cast to its
     * profile, where &work->profile.emotions would do */
    struct chao_emotions *traits =
        &((struct chao_profile *)parameters)->emotions;
    Uint8 skill;
    Uint16 life_base;
    Uint16 life_step;
    int i;
    int kindness;
    int aggression;
    int curiosity;
    Sint8 *trait;
    Uint8(*alleles)[2];

    chao_clear_animal_parts(chao);
    parameters->type = CHAO_CHILD;
    parameters->garden = chao_current_area();
    parameters->classroom_slot = -1;
    chao_stamp_birth(chao);
    parameters->stats[CHAO_STAMINA] = 0;
    parameters->race_stamina = parameters->stats[CHAO_STAMINA];
    for (i = 0; i < CHAO_SKILLS; i++) {
        if (gene->skill[i][0] >= gene->skill[i][1]) {
            if (rand() * RAND_SCALE < DOMINANT_ODDS) {
                skill = gene->skill[i][0];
            } else {
                skill = gene->skill[i][1];
            }
        } else if (rand() * RAND_SCALE < DOMINANT_ODDS) {
            skill = gene->skill[i][1];
        } else {
            skill = gene->skill[i][0];
        }
        parameters->best_stats[i] = skill * chao_tuning.skill_step;
    }
    life_base = chao_tuning.lifespan_base;
    life_step = chao_tuning.lifespan_step;
    if (gene->life[0] >= gene->life[1]) {
        parameters->life = parameters->lifespan =
            life_base + (gene->life[0] * life_step);
    } else {
        parameters->life = parameters->lifespan =
            life_base + (gene->life[1] * life_step);
    }
    /* @hack redundant: a block that never runs, and its test of life_step */
    if (0) {
        if (life_step) {
        }
    }
    i = gene->run_power[EXPRESSION_ALLELE()];
    i *= chao_tuning.shape_step;
    parameters->run_power = i;
    i = gene->fly_swim[EXPRESSION_ALLELE()];
    i *= chao_tuning.shape_step;
    parameters->fly_swim = i;
    i = gene->alignment[EXPRESSION_ALLELE()];
    i *= chao_tuning.alignment_step;
    parameters->alignment = i;
    /* personality.initial shares its bytes with personality.preferences, so
     * the traits and the taste set below overwrite both */
    traits->personality.initial.first = chao_tuning.need_max;
    traits->personality.initial.second = chao_tuning.need_max;
    for (trait = traits->personality.preferences.traits,
        alleles = gene->personality;
         trait < traits->personality.preferences.traits + PERSONALITY_TRAITS;
         trait++, alleles++) {
        *trait = ((*alleles)[EITHER_ALLELE()] - NEUTRAL_ALLELE) * TRAIT_STEP;
    }
    traits->personality.preferences.taste = gene->taste[EXPRESSION_ALLELE()];
    traits->personality.preferences.tv = gene->tv[EXPRESSION_ALLELE()];
    traits->personality.preferences.music = gene->music[EXPRESSION_ALLELE()];
    if (traits->personality.preferences.traits[TRAIT_INDEX(CHAO_KINDNESS)] <
        -TRAIT_BAND) {
        kindness = 0;
    } else if (traits->personality.preferences
                   .traits[TRAIT_INDEX(CHAO_KINDNESS)] < TRAIT_BAND) {
        kindness = 1;
    } else {
        kindness = 2;
    }
    if (traits->personality.preferences.traits[TRAIT_INDEX(CHAO_AGGRESSION)] <
        -TRAIT_BAND) {
        aggression = 0;
    } else if (traits->personality.preferences
                   .traits[TRAIT_INDEX(CHAO_AGGRESSION)] < TRAIT_BAND) {
        aggression = 1;
    } else {
        aggression = 2;
    }
    if (traits->personality.preferences.traits[TRAIT_INDEX(CHAO_CURIOSITY)] <
        -TRAIT_BAND) {
        curiosity = 0;
    } else if (traits->personality.preferences
                   .traits[TRAIT_INDEX(CHAO_CURIOSITY)] < TRAIT_BAND) {
        curiosity = 1;
    } else {
        curiosity = 2;
    }
    parameters->eye = default_eyes[aggression][kindness][curiosity];
    parameters->mouth = default_mouths[aggression][kindness][curiosity];
    if (gene->color[0]) {
        if (gene->color[1]) {
            parameters->color = gene->color[EXPRESSION_ALLELE()];
        } else {
            parameters->color = gene->color[0];
        }
    } else if (gene->color[1]) {
        parameters->color = gene->color[1];
    } else {
        parameters->color = 0;
    }
    if (gene->texture[0]) {
        if (gene->texture[1]) {
            parameters->texture = gene->texture[EXPRESSION_ALLELE()];
        } else {
            parameters->texture = gene->texture[0];
        }
    } else if (gene->texture[1]) {
        parameters->texture = gene->texture[1];
    } else {
        parameters->texture = 0;
    }
    if (gene->shiny[0] || gene->shiny[1]) {
        parameters->shiny = 1;
    } else {
        parameters->shiny = 0;
    }
}

/* this game's glyph for a Sonic Adventure character code, or 0 for a code
 * without one */
static char legacy_glyph(Uint8 code)
{
    char glyph;

    if (code == 0x20 || (code >= 0x60 && code <= 0x63) ||
        (code >= 0xbe && code <= 0xbf)) {
        glyph = 0x5f;
    } else if (code >= 0x21 && code <= 0x5f) {
        glyph = code - 0x20;
    } else if (code >= 0x64 && code <= 0xb6) {
        glyph = code + 0x3c;
    } else if (code >= 0xb7 && code <= 0xb8) {
        glyph = code + 0x3f;
    } else if (code == 0xb9) {
        glyph = 0x5e;
    } else if (code >= 0xba && code <= 0xbd) {
        glyph = code + 0x3e;
    } else if (code >= 0xc0 && code <= 0xc2) {
        glyph = code + 0x33;
    } else if (code >= 0xc3 && code <= 0xc4) {
        glyph = code + 0x39;
    } else {
        glyph = 0;
    }
    return glyph;
}

#define BOTH(pair, value) ((pair)[0] = (pair)[1] = (value))
#define IMPORT_SKILL(kind)                                                     \
    BOTH(gene->skill[(kind)], source->skill[(kind)] / chao_tuning.skill_step)
#define IMPORT_PERSONALITY(kind, value)                                        \
    BOTH(gene->personality[TRAIT_INDEX(kind)],                                 \
         ((value) / TRAIT_STEP) + NEUTRAL_ALLELE)
/* gene's color and textures for a Sonic Adventure Chao's finish. a jewel
 * from 0 to 3 sets the texture FIRST_JEWEL_TEXTURE plus legacy_chao.jewel.
 * any other jewel leaves the texture as it was */
#define BLACK_COLOR 1
#define GOLD_TEXTURE 1
#define SILVER_TEXTURE 2
#define FIRST_JEWEL_TEXTURE 8

void chao_import_legacy(const struct legacy_chao *source,
                        struct chao_save *destination)
{
    /* @hack spelling: save, a copy of destination, where destination would
     * do */
    struct chao_save *save = destination;
    struct chao_gene *gene = &destination->record.saved.gene;
    const Sint8 *from;
    char *to;

    memset(gene, 0, sizeof(*gene));
    memset(save, 0, sizeof(save->record.live));
    chao_new_gene(gene);
    save->record.saved.parameters.type = CHAO_EGG;
    save->record.saved.parameters.affection = source->affection;
    save->record.saved.parameters.reborn_egg = 1;
    save->record.saved.parameters.run_power = source->run_power;
    save->record.saved.parameters.fly_swim = source->fly_swim;
    save->record.saved.parameters.evolution = source->evolution;
    save->record.saved.parameters.reincarnations = source->generation + 1;
    from = source->name;
    /* @hack spelling: two characters an iteration, each pointer stepped by
     * + 1, where one character an iteration with ++ would do */
    for (to = save->record.saved.parameters.name;
         to < save->record.saved.parameters.name + sizeof(source->name);
         to = to + 1) {
        *to = legacy_glyph(*from);
        from = from + 1;
        to = to + 1;
        *to = legacy_glyph(*from);
        from = from + 1;
    }
    IMPORT_SKILL(CHAO_SWIM);
    IMPORT_SKILL(CHAO_FLY);
    IMPORT_SKILL(CHAO_RUN);
    IMPORT_SKILL(CHAO_POWER);
    if (source->finish_flags & LEGACY_BLACK) {
        BOTH(gene->color, BLACK_COLOR);
    }
    if (source->finish_flags & LEGACY_SILVER) {
        BOTH(gene->texture, SILVER_TEXTURE);
    }
    if (source->finish_flags & LEGACY_GOLD) {
        BOTH(gene->texture, GOLD_TEXTURE);
    }
    if (source->finish_flags & LEGACY_JEWEL) {
        switch (source->jewel) {
        case 0:
            BOTH(gene->texture, FIRST_JEWEL_TEXTURE);
            break;
        case 1:
            BOTH(gene->texture, FIRST_JEWEL_TEXTURE + 1);
            break;
        case 2:
            BOTH(gene->texture, FIRST_JEWEL_TEXTURE + 2);
            break;
        case 3:
            BOTH(gene->texture, FIRST_JEWEL_TEXTURE + 3);
            break;
        }
    }
    IMPORT_PERSONALITY(CHAO_CURIOSITY, source->curiosity);
    IMPORT_PERSONALITY(CHAO_KINDNESS, source->kindness);
    IMPORT_PERSONALITY(CHAO_AGGRESSION, source->aggression);
}
