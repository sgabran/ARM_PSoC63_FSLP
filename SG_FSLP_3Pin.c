// revision 2024-2-27-1

#include "SG_FSLP_3Pin.h"

cy_en_sar_status_t _fslp_status;

uint32_t FSLP_read_force(void)
{
    // Step 1 - Set up drive line voltages
    // R Output, 0
    Cy_GPIO_SetDrivemode(FSLPR_PORT, FSLPR_NUM, CY_GPIO_DM_STRONG_IN_OFF);
    Cy_GPIO_Write(FSLPR_PORT, FSLPR_NUM, 0);
    // Connect both DLs to 5V
    Cy_GPIO_SetDrivemode(FSLPDL2_PORT, FSLPDL2_NUM, CY_GPIO_DM_STRONG_IN_OFF);
    Cy_GPIO_Write(FSLPDL2_PORT, FSLPDL2_NUM, 1);

    // Step 2 - Wait for the voltage to stabilize
    CyDelayUs(FSLP_read_delay);

    // Step 3 - Take two measurements
    ADC_StartConvert();
    _fslp_status = 1;
    while(_fslp_status != CY_SAR_SUCCESS)
    {
        _fslp_status = Cy_SAR_IsEndConversion(SAR, CY_SAR_RETURN_STATUS);
    };
    return Cy_SAR_GetResult32(SAR, 0);
}

uint32_t FSLP_read_position(void)
{
    // Step 1 - Clear the charge on the sensor
    // Disconnect reference divider resistor
    Cy_GPIO_SetDrivemode(FSLPR_PORT, FSLPR_NUM, CY_GPIO_DM_HIGHZ);
    // Connect DL2 to ground
    Cy_GPIO_Write(FSLPDL2_PORT, FSLPDL2_NUM, 0);
    
    // Step 2 - Wait for the voltage to stabilize
    CyDelayUs(FSLP_read_delay);
    
    // Step 3 - Take two measurements
    ADC_StartConvert();
    _fslp_status = 1;
    while(_fslp_status != CY_SAR_SUCCESS)
    {
        _fslp_status = Cy_SAR_IsEndConversion(SAR, CY_SAR_RETURN_STATUS);
    };
    return Cy_SAR_GetResult32(SAR, 0);
}

//void FSLP_init(void)
//{
//    // DL2: GPIO Out
//    Cy_GPIO_SetDrivemode(FSLPDL2_PORT, FSLPDL2_NUM, CY_GPIO_DM_STRONG_IN_OFF);
//    Cy_GPIO_Write(FSLPDL2_PORT, FSLPDL2_NUM, 0);
//    // SL: Anlogue In
//    Cy_GPIO_SetDrivemode(FSLPSL_PORT, FSLPSL_NUM, CY_GPIO_DM_ANALOG);
//    Cy_GPIO_Write(FSLPSL_PORT, FSLPSL_NUM, 0);
//    // R: GPIO Out
//    Cy_GPIO_SetDrivemode(FSLPR_PORT, FSLPR_NUM, CY_GPIO_DM_STRONG_IN_OFF);
//    Cy_GPIO_Write(FSLPR_PORT, FSLPR_NUM, 0);
//}
