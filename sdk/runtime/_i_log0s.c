#include <mathf.h>

static const float log10_e = 0.4342945F;

float _in_log10s(float x)
{
    return log10_e * _in_logs(x);
}
