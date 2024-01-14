#ifndef BMP390_H_
#define BMP390_H_

#ifdef __cplusplus
  extern "C" {
#endif
// #define FREERTOS_ENABLED true

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define BMP3_I2C_ADDR							0x77 //ITS 0x77 FOR BREADBOARD!!!, 0x76 ON SENSOR BOARD!!!

/**\name Macro to combine two 8 bit data's to form a 16 bit data */
#define BMP3_CONCAT_BYTES(msb, lsb)             (((uint16_t)msb << 8) | (uint16_t)lsb)

#define BMP3_GET_BITS(reg_data, bitname)        ((reg_data & (bitname##_MSK)) >> \
                                                 (bitname##_POS))

/* Macro variant to handle the bitname position if it is zero */
#define BMP3_GET_BITS_POS_0(reg_data, bitname)  (reg_data & (bitname##_MSK))

#define BMP3_GET_LSB(var)                       (uint8_t)(var & BMP3_SET_LOW_BYTE)
#define BMP3_GET_MSB(var)                       (uint8_t)((var & BMP3_SET_HIGH_BYTE) >> 8)

/**\name Register Address */
#define BMP3_REG_CHIP_ID                        UINT8_C(0x00)
#define BMP3_REG_ERR                            UINT8_C(0x02)
#define BMP3_REG_SENS_STATUS                    UINT8_C(0x03)
#define BMP3_REG_DATA							UINT8_C(0x04) //using
/* making these structs...
#define BMP3_REG_PRESSURE_DATA                  UINT8_C(0x04) //3bytes
#define BMP3_REG_TEMP_DATA						UINT8_C(0x07) //3bytes
#define BMP3_REG_SENSOR_TIME					UINT8_C(0x0C) //3 bytes
*/
#define BMP3_REG_EVENT                          UINT8_C(0x10)
#define BMP3_REG_INT_STATUS                     UINT8_C(0x11)
#define BMP3_REG_FIFO_LENGTH                    UINT8_C(0x12)
#define BMP3_REG_FIFO_DATA                      UINT8_C(0x14)
#define BMP3_REG_FIFO_WM                        UINT8_C(0x15)
#define BMP3_REG_FIFO_CONFIG_1                  UINT8_C(0x17)
#define BMP3_REG_FIFO_CONFIG_2                  UINT8_C(0x18)
#define BMP3_REG_INT_CTRL                       UINT8_C(0x19)
#define BMP3_REG_IF_CONF                        UINT8_C(0x1A)
#define BMP3_REG_PWR_CTRL                       UINT8_C(0x1B) //using (modes)
#define BMP3_REG_OSR                            UINT8_C(0X1C) //using (oversampling)
#define BMP3_REG_ODR                            UINT8_C(0x1D) //using (output data rate)
#define BMP3_REG_CONFIG                         UINT8_C(0x1F) //using (isr filter)
#define BMP3_REG_CALIB_DATA                     UINT8_C(0x31) //using (calibration data for data compensation)
#define BMP3_REG_CMD                            UINT8_C(0x7E)

/**\name Status macros */
#define BMP3_CMD_RDY                            UINT8_C(0x10)
#define BMP3_DRDY_PRESS                         UINT8_C(0x20)
#define BMP3_DRDY_TEMP                          UINT8_C(0x40)

/**\enables */
#define BMP3_ENABLE                             UINT8_C(0x01)
#define BMP3_DISABLE                            UINT8_C(0x00)

/**\name Power mode macros */
#define BMP3_MODE_SLEEP                         UINT8_C(0x00)
#define BMP3_MODE_FORCED                        UINT8_C(0x01)
#define BMP3_MODE_NORMAL                        UINT8_C(0x03)

/**\name Over sampling macros */
#define BMP3_NO_OVERSAMPLING                    UINT8_C(0x00)
#define BMP3_OVERSAMPLING_2X                    UINT8_C(0x01)
#define BMP3_OVERSAMPLING_4X                    UINT8_C(0x02)
#define BMP3_OVERSAMPLING_8X                    UINT8_C(0x03)
#define BMP3_OVERSAMPLING_16X                   UINT8_C(0x04)
#define BMP3_OVERSAMPLING_32X                   UINT8_C(0x05)

/**\name Filter setting macros */
#define BMP3_IIR_FILTER_DISABLE                 UINT8_C(0x00)
#define BMP3_IIR_FILTER_COEFF_1                 UINT8_C(0x01)
#define BMP3_IIR_FILTER_COEFF_3                 UINT8_C(0x02)
#define BMP3_IIR_FILTER_COEFF_7                 UINT8_C(0x03)
#define BMP3_IIR_FILTER_COEFF_15                UINT8_C(0x04)
#define BMP3_IIR_FILTER_COEFF_31                UINT8_C(0x05)
#define BMP3_IIR_FILTER_COEFF_63                UINT8_C(0x06)
#define BMP3_IIR_FILTER_COEFF_127               UINT8_C(0x07)

/**\name Odr setting macros */
#define BMP3_ODR_200_HZ                         UINT8_C(0x00)
#define BMP3_ODR_100_HZ                         UINT8_C(0x01)
#define BMP3_ODR_50_HZ                          UINT8_C(0x02)
#define BMP3_ODR_25_HZ                          UINT8_C(0x03)
#define BMP3_ODR_12_5_HZ                        UINT8_C(0x04)
#define BMP3_ODR_6_25_HZ                        UINT8_C(0x05)
#define BMP3_ODR_3_1_HZ                         UINT8_C(0x06)
#define BMP3_ODR_1_5_HZ                         UINT8_C(0x07)
#define BMP3_ODR_0_78_HZ                        UINT8_C(0x08)
#define BMP3_ODR_0_39_HZ                        UINT8_C(0x09)
#define BMP3_ODR_0_2_HZ                         UINT8_C(0x0A)
#define BMP3_ODR_0_1_HZ                         UINT8_C(0x0B)
#define BMP3_ODR_0_05_HZ                        UINT8_C(0x0C)
#define BMP3_ODR_0_02_HZ                        UINT8_C(0x0D)
#define BMP3_ODR_0_01_HZ                        UINT8_C(0x0E)
#define BMP3_ODR_0_006_HZ                       UINT8_C(0x0F)
#define BMP3_ODR_0_003_HZ                       UINT8_C(0x10)
#define BMP3_ODR_0_001_HZ                       UINT8_C(0x11)

/**\name Macros related to size */
#define BMP3_LEN_CALIB_DATA                     UINT8_C(21)
#define BMP3_LEN_P_AND_T_HEADER_DATA            UINT8_C(7)
#define BMP3_LEN_P_OR_T_HEADER_DATA             UINT8_C(4)
#define BMP3_LEN_P_T_DATA                       UINT8_C(6)
#define BMP3_LEN_GEN_SETT                       UINT8_C(7)
#define BMP3_LEN_P_DATA                         UINT8_C(3)
#define BMP3_LEN_T_DATA                         UINT8_C(3)
#define BMP3_LEN_SENSOR_TIME                    UINT8_C(3)
#define BMP3_FIFO_MAX_FRAMES                    UINT8_C(73)

typedef enum {
	BMP3_PRESSURE = UINT8_C(0x04),  // Default: hpa  **3 bytes each**
	BMP3_TEMPERATURE = UINT8_C(0x07),   // Default: C
	BMP3_SENSOR_TIME = UINT8_C(0x0C),      // Default: s
} bmp3_data;

typedef struct {
    /**
     * @ Trim Variables
     */
    uint16_t par_t1;
    uint16_t par_t2;
    int8_t par_t3;
    int16_t par_p1;
    int16_t par_p2;
    int8_t par_p3;
    int8_t par_p4;
    uint16_t par_p5;
    uint16_t par_p6;
    int8_t par_p7;
    int8_t par_p8;
    int16_t par_p9;
    int8_t par_p10;
    int8_t par_p11;
    int64_t t_lin;

} bmp3_reg_calib_data;

typedef struct {
    /**
     * @ Quantized Trim Variables
     */
	double par_t1;
	double par_t2;
	double par_t3;
	double par_p1;
	double par_p2;
	double par_p3;
	double par_p4;
	double par_p5;
	double par_p6;
	double par_p7;
	double par_p8;
	double par_p9;
	double par_p10;
	double par_p11;
	double t_lin;

} bmp3_quantized_calib_data;

typedef struct {
    /*! Quantized data */
    bmp3_quantized_calib_data quantized_calib_data;

    /*! Register data */
    bmp3_reg_calib_data reg_calib_data;

} bmp3_calib_data;

typedef struct {
	uint32_t temperature;
	uint32_t pressure;

} bmp3_uncomp_data;

typedef struct {
	double temperature;
	double pressure;

} comp_data;

void bmp390_writeData(uint8_t reg, uint8_t data);
void bmp390_readData(uint8_t reg, uint8_t *data, uint8_t len);
void bmp390_delay(int time);

void bmp390_setPage(uint8_t page);
void bmp390_setMode(uint8_t press_en, uint8_t temp_en, uint8_t mode);
void bmp390_setOversampling(uint8_t press_oversampling, uint8_t temp_oversampling);
void bmp390_setIIRCoeff(uint8_t iir_coeff);
void bmp390_setOutputDataRate(uint8_t data_rate);

double pow_bmp3(double base, uint8_t power);

//comp_data bmp390_getData();
comp_data bmp390_getData(bmp3_calib_data *calib);

bmp3_calib_data bmp390_getCalibData();
double bmp390_compensateTemp(bmp3_uncomp_data *uncomp_data, bmp3_calib_data *calib_data);
double bmp390_compensatePressure(bmp3_uncomp_data *uncomp_data, bmp3_calib_data *calib_data);

#ifdef __cplusplus
  }
#endif /* End of CPP guard */
#endif /* BMP3_DEFS_H_ */
