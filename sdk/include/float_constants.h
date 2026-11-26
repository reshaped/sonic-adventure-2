/* the single-precision constants of Hitachi's math library */
#ifndef FLOAT_CONSTANTS_H
#define FLOAT_CONSTANTS_H

/* C89 has no float literal for an infinity or for a NaN of a given payload.
 * these three are given as bits */
union float_constant {
    unsigned int bits;
    float value;
};

extern const union float_constant _IFNANS;
extern const union float_constant _IFINFS;
extern const union float_constant _NIFINFS;

/* PAIS is pi, 2PAIS twice pi, and HLFPAIS and QUOPAIS its half and
 * quarter. LOGE2S is the natural logarithm of 2. an N after the underscore
 * negates the constant: _NPAIS is -pi */
extern const float _IFZEROS;
extern const float _IFONES;
extern const float _HALFS;
extern const float _QUOTS;
extern const float _TWOS;
extern const float _FOURS;
extern const float _PAIS;
extern const float _2PAIS;
extern const float _HLFPAIS;
extern const float _QUOPAIS;
extern const float _LOGE2S;
extern const float _NIFZEROS;
extern const float _NIFONES;
extern const float _NHALFS;
extern const float _NQUOTS;
extern const float _NTWOS;
extern const float _NFOURS;
extern const float _NPAIS;
extern const float _N2PAIS;
extern const float _NHLFPAIS;
extern const float _NQUOPAIS;
extern const float _NLOGE2S;

#endif
