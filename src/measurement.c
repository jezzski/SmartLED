#include "measurement.h"

void initOC(void)
{
    //should this take current level now?


    dac_output_enable(OC_LIMIT);

    dac_output_voltage(OC_LIMIT, 255); //test output VDD voltage
}