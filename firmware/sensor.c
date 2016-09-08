#include "pic24_all.h"
#include "i2c.h"

#define I2C_ID_MS5611    ((u8)0b11101110)
#define I2C_ID_MS5611_RST ((u8) 0x1E)
u8 I2C_ID_MS5607_C[7] = {0, 0b10100010, 0b10100100, 0b10100110, 0b10101000, 0b10101010, 0b10101100 };
#define I2C_ID_MS5611_CONVERT_PRESSURE_4096 ((u8)0x48)
#define I2C_ID_MS5611_CONVERT_TEMP_4096     ((u8)0x52)
#define I2C_ID_MS5611_READ_ADC              ((u8)0x00)

// MS5611 calibration coefficients
u16 COEFS[7];

s32 dT = 68;
// D2 typical value : 8077736
s16 sns_CompensateTemperature(u32 D2)	{
    // Difference between actual and reference temperature
    dT = D2 - ((u32)COEFS[5] << 8); // dT easily goes up to 250000     C5*256 = 8 256 000
    // Actual temperature (-40 unsigned long 85°C with 0.01°C resolution)
    s32 TEMP = 2000 + ((dT * (s64)COEFS[6]) >> 23);
    return (s16) (TEMP/100);
}

s32 sns_CompensatePressure(u32 D1)  {
//    sprintf(dbg, "%lu\t", D1);
//    uart_Print(dbg);
//    sprintf(dbg, "%ld\t", dT);
//    uart_Print(dbg);
    // Offset at actual temperature
    s64 OFF = ((s64)COEFS[2] << 16) + (((s64)COEFS[4] * dT) >> 7);
//    sprintf(dbg, "%lld\t", OFF);
//    uart_Print(dbg);
    
    // Sensitivity at actual temperature
    s64 SENS = ((s64)COEFS[1] << 15) + (((s64)COEFS[3] * dT) >> 8);
//    sprintf(dbg, "%lld\t", SENS);
//    uart_Print(dbg);
    // Temperature compensated pressure (10 to 1200mbar with 0.01mbar resolution)
    s32 P = (((D1 * SENS) >> 21 ) - OFF) >> 15;

    return P;
}

// Trigger a Pressure measurement and conversion
void sns_TriggerPressureSample(void) {
    i2c_Write1(I2C_ID_MS5611,I2C_ID_MS5611_CONVERT_PRESSURE_4096);
}

// Trigger a Temperature measurement and conversion
void sns_TriggerTemperatureSample(void) {
    i2c_Write1(I2C_ID_MS5611,I2C_ID_MS5611_CONVERT_TEMP_4096);
}

// Read the result of the conversion (temperature or pressure)
void sns_ReadADC(u32 * value) {
    u8 temp[3];
    i2c_RcvBuf(I2C_ID_MS5611,I2C_ID_MS5611_READ_ADC,3,temp);

    Nop();
    Nop();
    Nop();
    
    *value = 0X00000000 | temp[0];
    *value = *value << 8 | temp[1];
    *value = *value << 8| temp[2];
}

// Reset the sensor : load the calibration coefficients into the memory
void sns_Reset(void) {
    i2c_Write1(I2C_ID_MS5611, I2C_ID_MS5611_RST);
}

/**
 * call sns_Reset, wait, then call this
 * Read the calibration coefficient for the calculations
 */
void sns_Config(void) {
    u8 b[2];

    int i = 1;
    for (; i <= 6; i++) {
        i2c_RcvBuf(I2C_ID_MS5611, I2C_ID_MS5607_C[i], 2, b);
        COEFS[i] = ((u16) b[0] << 8) | (u16)b[1];
    }
}
