/* The startup words use section-relative addresses and absolute byte
 * counts. One output section keeps the sixteen groups together across
 * another relocation, including groups that contribute no bytes. */
#define MPV_GROUP(name)                            \
    . = ALIGN(4);                                  \
    __startof_##name = .;                          \
    *(name)                                        \
    __endof_##name = .;                            \
    __sizeof_##name = ABSOLUTE(. - __startof_##name);

SECTIONS
{
    .text.sofdec : ALIGN(4)
    {
        MPV_GROUP(PSGSFD00)
        MPV_GROUP(PSGSFD01)
        MPV_GROUP(PSGSFD02)
        MPV_GROUP(PSGSFD03)
        MPV_GROUP(PSGSFD04)
        MPV_GROUP(PSGSFD05)
        MPV_GROUP(PSGSFD06)
        MPV_GROUP(PSGSFD07)
        MPV_GROUP(PSGSFD08)
        MPV_GROUP(PSGSFD09)
        MPV_GROUP(PSGSFD10)
        MPV_GROUP(PSGSFD11)
        MPV_GROUP(PSGSFD12)
        MPV_GROUP(PSGSFD13)
        MPV_GROUP(PSGSFD14)
        MPV_GROUP(PSGSFD15)
    }
}
ASSERT(__sizeof_PSGSFD00 > 0 && __sizeof_PSGSFD02 > 0,
       "Sofdec decoder code missing from its instruction sections")
ASSERT(__sizeof_PSGSFD01 == 1582, "Sofdec decoder reservation has the wrong size")
