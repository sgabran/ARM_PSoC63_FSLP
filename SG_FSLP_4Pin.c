// revision 2024-2-27-1

/*
    Use CY_GPIO_DM_STRONG for output pins
    R CY_GPIO_DM_PULLUP
    SL CY_GPIO_DM_ANALOG
    DL2 CY_GPIO_DM_ANALOG
    
    2 samples ave
    sequential sum
    
    Output: Unsigned
    Vref: Vdda
    Vneg: Vref
    S/E: 0 to 2*Vref
    
    Comments:
    1.
    Cy_GPIO_SetDrivemode(FSLPR_PORT, FSLPR_NUM, CY_GPIO_DM_HIGHZ); works
    Cy_GPIO_SetDrivemode(FSLPR_PORT, FSLPR_NUM, CY_GPIO_DM_PULLUP); doesn't work
    
    2.
    Similar: CY_GPIO_DM_STRONG or CY_GPIO_DM_STRONG_IN_OFF
    
    3.
    _fslp_status = 1;
    while(_fslp_status != CY_SAR_SUCCESS)
    {
        _fslp_status = Cy_SAR_IsEndConversion(SAR, CY_SAR_RETURN_STATUS);
    };
    ADC_StartConvert();
    CyDelayUs(3);
*/
    
#include "SG_FSLP_4Pin.h"

volatile uint32_t _read_DL2;
volatile uint32_t _read_SL;
volatile int32_t _read_SL_2;

cy_en_sar_status_t _fslp_status;

uint32_t FSLP_read_force(void)
{
    // Step 1 - Set up drive line voltages
    // DL1 Output, 1
    Cy_GPIO_Write(FSLPDL1_PORT, FSLPDL1_NUM, 1);
    
    // R Output, 0
    Cy_GPIO_SetDrivemode(FSLPR_PORT, FSLPR_NUM, CY_GPIO_DM_STRONG_IN_OFF);
    Cy_GPIO_Write(FSLPR_PORT, FSLPR_NUM, 0);
    
    // SL Input Analog
    Cy_GPIO_SetDrivemode(FSLPSL_PORT, FSLPSL_NUM, CY_GPIO_DM_ANALOG);
    // DL2 Input Analog
    Cy_GPIO_SetDrivemode(FSLPDL2_PORT, FSLPDL2_NUM, CY_GPIO_DM_ANALOG);
    
    // Step 2 - Wait for the voltage to stabilize
    CyDelayUs(FSLP_read_delay);
    
    // Step 3 - Take two measurements
    ADC_StartConvert();
    _fslp_status = 1;
    while(_fslp_status != CY_SAR_SUCCESS)
    {
        _fslp_status = Cy_SAR_IsEndConversion(SAR, CY_SAR_RETURN_STATUS);
    };
    _read_DL2 = Cy_SAR_GetResult32(SAR, 0);
    _read_SL = Cy_SAR_GetResult32(SAR, 1);
        
    // Step 4 - Calculate the pressure.
    if (_read_DL2 == _read_SL)
    {
        // Avoid dividing by zero, and return maximum reading
        return 32 * 1023;
    }
    return 32 * _read_SL / (_read_DL2 - _read_SL);
}

uint32_t FSLP_read_position(void)
{
    // Step 1 - Clear the charge on the sensor
    // SL Output, 0
    Cy_GPIO_SetDrivemode(FSLPSL_PORT, FSLPSL_NUM, CY_GPIO_DM_STRONG_IN_OFF);
    Cy_GPIO_Write(FSLPSL_PORT, FSLPSL_NUM, 0);
    
    // DL1 Output, 0
    Cy_GPIO_SetDrivemode(FSLPDL1_PORT, FSLPDL1_NUM, CY_GPIO_DM_STRONG_IN_OFF);
    Cy_GPIO_Write(FSLPDL1_PORT, FSLPDL1_NUM, 0);
    
    // DL2 Output, 0
    Cy_GPIO_SetDrivemode(FSLPDL2_PORT, FSLPDL2_NUM, CY_GPIO_DM_STRONG_IN_OFF);
    Cy_GPIO_Write(FSLPDL2_PORT, FSLPDL2_NUM, 0);
    
    // R Output, 0
    Cy_GPIO_SetDrivemode(FSLPR_PORT, FSLPR_NUM, CY_GPIO_DM_STRONG_IN_OFF);
    Cy_GPIO_Write(FSLPR_PORT, FSLPR_NUM, 0);
    
    // Step 2 - Set up appropriate drive line voltages
    Cy_GPIO_Write(FSLPDL1_PORT, FSLPDL1_NUM, 1);
    // R Input
    Cy_GPIO_SetDrivemode(FSLPR_PORT, FSLPR_NUM, CY_GPIO_DM_HIGHZ);
    // SL Input, analog
    Cy_GPIO_SetDrivemode(FSLPSL_PORT, FSLPSL_NUM, CY_GPIO_DM_ANALOG);
    
    // Step 3 - Wait for the voltage to stabilize
    CyDelayUs(FSLP_read_delay);
    
    // Step 4 - Take the measurement
    ADC_StartConvert();
    _fslp_status = 1;
    while(_fslp_status != CY_SAR_SUCCESS)
    {
        _fslp_status = Cy_SAR_IsEndConversion(SAR, CY_SAR_RETURN_STATUS);
    };
    _read_SL_2 = Cy_SAR_GetResult32(SAR, 1);
    return _read_SL_2;
}

void FSLP_init(void)
{
    // DL1: GPIO Out
    Cy_GPIO_SetDrivemode(FSLPDL1_PORT, FSLPDL1_NUM, CY_GPIO_DM_STRONG_IN_OFF);
    Cy_GPIO_Write(FSLPDL1_PORT, FSLPDL1_NUM, 0);
    // DL2: GPIO Out
    Cy_GPIO_SetDrivemode(FSLPDL2_PORT, FSLPDL2_NUM, CY_GPIO_DM_STRONG_IN_OFF);
    Cy_GPIO_Write(FSLPDL2_PORT, FSLPDL2_NUM, 0);
    //SL: Anlogue In
    Cy_GPIO_SetDrivemode(FSLPSL_PORT, FSLPSL_NUM, CY_GPIO_DM_ANALOG);
    Cy_GPIO_Write(FSLPSL_PORT, FSLPSL_NUM, 0);
    //R: GPIO Out
    Cy_GPIO_SetDrivemode(FSLPR_PORT, FSLPR_NUM, CY_GPIO_DM_STRONG_IN_OFF);
    Cy_GPIO_Write(FSLPR_PORT, FSLPR_NUM, 0);
}


// Scale the pressure reading to be from 0 to 255
int FSLP_scale_pressure(int32_t pressure)
{
    int adjusted_pressure = pressure * 8 / 10;
    if (adjusted_pressure > 255)
    {
        adjusted_pressure = 255;
    }
    return adjusted_pressure;
}

int FSLP_scale_position(int32_t position)
{
    return (int32_t)position * FSLP_POSITIONS / 1000;
}