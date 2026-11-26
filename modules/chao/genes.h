/* a Chao's genes, each a pair of alleles. an egg gets them, a hatchling takes
 * its traits from them, and a parent passes them on */
#ifndef CHAO_GENES_H
#define CHAO_GENES_H
#include <sg_xpt.h>

struct task;
struct chao_gene;
struct chao_profile;
struct chao_save;
struct legacy_chao;

/* give gene a new egg's alleles: its skills, life, personality and tastes at
 * random, and the rest 0. a hero egg's alignment alleles are 5, and a dark
 * egg's -5 */
void chao_new_gene(struct chao_gene *gene);
void chao_new_hero_gene(struct chao_gene *gene);
void chao_new_dark_gene(struct chao_gene *gene);
/* set gene's skill alleles from profile's stats. set one allele of each
 * personality trait, picked at random, from profile's traits */
void chao_learn_traits(struct chao_gene *gene,
                       const struct chao_profile *profile);
/* cross first's genes with second's into second, and make second an egg */
void chao_make_saved_egg(const struct chao_save *first,
                         struct chao_save *second);
void chao_cross_genes(const struct chao_gene *first,
                      const struct chao_gene *second, struct chao_gene *child);
/* cross the genes of two Chao into child, each Chao's as chao_learn_traits
 * leaves them. the Chao keep their own */
void chao_inherit_traits(const struct task *first, const struct task *second,
                         struct chao_gene *child);
/* set a hatching Chao's parameters and personality from its genes */
void chao_hatch_traits(struct task *chao);
/* make destination an egg of a Sonic Adventure Chao */
void chao_import_legacy(const struct legacy_chao *source,
                        struct chao_save *destination);

#endif
