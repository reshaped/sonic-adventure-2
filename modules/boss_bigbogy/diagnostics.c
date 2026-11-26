/* the fight's debug strings, which no code reads. they keep their shipped
 * spellings. each starts on a word boundary, so its array rounds it up to a
 * multiple of 4 bytes */
struct boss_diagnostics {
    char jump[12];
    char damage[16];
    char damage_distance[20];
};
static const struct boss_diagnostics diagnostics = {
    "OBAKE JUMP",
    "DAMAGE %d  %d",
    "ChengeDist Damege%d",
};
