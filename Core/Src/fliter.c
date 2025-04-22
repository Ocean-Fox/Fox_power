#include "fliter.h"
/*------һ�׵�ͨ�˲�------*/
void low_filter_init(LOWPASS_FILTER_STRUCT *p, float sample_f, float cutoff_f)
{
    float Tc; //ʱ�䳣����
    p->Fc = cutoff_f;
    p->Fs = sample_f;
    if (p->Fc <= 0.0f || p->Fs <= 0.0f)
    {
        p->Ka = 1.0f;
        p->Kb = 0.0f;
        return;
    }

    Tc = 1.0f / (PI2 * p->Fc);

    p->Ka = 1.0f / (1.0f + Tc * p->Fs);
    p->Kb = 1.0f - p->Ka;
    p->Input = 0.0f;
    p->Output = 0.0f;
}

CCMRAM void low_filter_calc(LOWPASS_FILTER_STRUCT *p)
{
    if (p->Output == p->Input)
        return;

    p->Output = p->Ka * p->Input + p->Kb * p->Output;
}
