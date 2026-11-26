#include <float_constants.h>

const union float_constant _IFNANS = {0x7f800001U};
const float _IFZEROS = 0.0F;
const union float_constant _IFINFS = {0x7f800000U};
const float _IFONES = 1.0F;
const float _HALFS = 0.5F;
const float _QUOTS = 0.25F;
const float _TWOS = 2.0F;
const float _FOURS = 4.0F;
/* the library's pi, 0x40490fdc, is one float above the float nearest pi */
const float _PAIS = 3.141593F;
const float _2PAIS = 6.2831855F;
const float _HLFPAIS = 1.5707964F;
const float _QUOPAIS = 0.7853982F;
const float _LOGE2S = 0.6931472F;

const float _NIFZEROS = -0.0F;
const union float_constant _NIFINFS = {0xff800000U};
const float _NIFONES = -1.0F;
const float _NHALFS = -0.5F;
const float _NQUOTS = -0.25F;
const float _NTWOS = -2.0F;
const float _NFOURS = -4.0F;
const float _NPAIS = -3.141593F;
const float _N2PAIS = -6.2831855F;
const float _NHLFPAIS = -1.5707964F;
const float _NQUOPAIS = -0.7853982F;
const float _NLOGE2S = -0.6931472F;
