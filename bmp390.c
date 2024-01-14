#include "bmp390.h"
#include <string.h>
#include <math.h>
#include <stdio.h>


void bmp390_setPage(uint8_t page) { bmp390_writeData(BMP3_REG_CHIP_ID, page); }

void bmp390_setMode(uint8_t press_en, uint8_t temp_en, uint8_t mode){
	uint8_t mode_select = 0x17;
	bmp390_writeData(BMP3_REG_PWR_CTRL, mode_select);
}

void bmp390_setOversampling(uint8_t press_oversampling, uint8_t temp_oversampling) {
  uint8_t oversampling = (press_oversampling & 0x07) | ((temp_oversampling & 0x07) << 3);
  bmp390_writeData(BMP3_REG_OSR, oversampling);
}

void bmp390_setIIRCoeff(uint8_t iir_coeff){
	bmp390_writeData(BMP3_REG_CONFIG , iir_coeff);
}

void bmp390_setOutputDataRate(uint8_t data_rate){
	bmp390_writeData(BMP3_REG_ODR, data_rate);
}

double pow_bmp3(double base, uint8_t power){
	double pow_output = 1;

    while (power != 0)
    {
        pow_output = (float) base * pow_output;
        power--;
    }
    return pow_output;
}

comp_data bmp390_getData(bmp3_calib_data *calib) {
  //bmp390_setPage(0);
  uint8_t buff[6] = {0};    // 3 bytes each

  bmp3_uncomp_data uncomp_data = {.pressure = 0, .temperature = 0};
  comp_data data =  {.pressure = 0, .temperature = 0};

  bmp390_readData(BMP3_REG_DATA, buff, 6);

  uncomp_data.temperature = (uint32_t)(buff[5] << 16 | buff[4] << 8 | buff[3]); // MSB is in the 3rd byte!!!!
  data.temperature = bmp390_compensateTemp(&uncomp_data, calib);

  uncomp_data.pressure = (uint32_t)(buff[2] << 16 | buff[1] << 8 | buff[0]); // MSB is in the 3rd byte!!!!
  data.pressure = bmp390_compensatePressure(&uncomp_data, calib);

  return data;

}

bmp3_calib_data bmp390_getCalibData() {

	uint8_t buff[BMP3_LEN_CALIB_DATA] = {0};
    bmp390_readData(BMP3_REG_CALIB_DATA, buff, BMP3_LEN_CALIB_DATA);

    bmp3_calib_data x;

	x.reg_calib_data.par_t1 = BMP3_CONCAT_BYTES(buff[1], buff[0]);
	x.reg_calib_data.par_t2 = BMP3_CONCAT_BYTES(buff[3], buff[2]);
	x.reg_calib_data.par_t3 = (int8_t)buff[4];
	x.reg_calib_data.par_p1 = (int16_t)BMP3_CONCAT_BYTES(buff[6], buff[5]);
	x.reg_calib_data.par_p2 = (int16_t)BMP3_CONCAT_BYTES(buff[8], buff[7]);
	x.reg_calib_data.par_p3 = (int8_t)buff[9];
	x.reg_calib_data.par_p4 = (int8_t)buff[10];
	x.reg_calib_data.par_p5 = BMP3_CONCAT_BYTES(buff[12], buff[11]);
	x.reg_calib_data.par_p6 = BMP3_CONCAT_BYTES(buff[14], buff[13]);
	x.reg_calib_data.par_p7 = (int8_t)buff[15];
	x.reg_calib_data.par_p8 = (int8_t)buff[16];
	x.reg_calib_data.par_p9 = (int16_t)BMP3_CONCAT_BYTES(buff[18], buff[17]);
	x.reg_calib_data.par_p10 = (int8_t)buff[19];
	x.reg_calib_data.par_p11 = (int8_t)buff[20];

	x.quantized_calib_data.par_t1 = (double)x.reg_calib_data.par_t1 / 0.00390625f;
	x.quantized_calib_data.par_t2 = (double)x.reg_calib_data.par_t2 / 1073741824.0f;
	x.quantized_calib_data.par_t3 = (double)x.reg_calib_data.par_t3 / 281474976710656.0f;

	x.quantized_calib_data.par_p1 = ((double)x.reg_calib_data.par_p1 - 16384.0)/ 1048576.0f;
	x.quantized_calib_data.par_p2 = ((double)x.reg_calib_data.par_p2 - 16384.0)/ 536870912.0f;
	x.quantized_calib_data.par_p3 = (double)x.reg_calib_data.par_p3 / 4294967296.0f;
	x.quantized_calib_data.par_p4 = (double)x.reg_calib_data.par_p4 / 137438953472.0f;
	x.quantized_calib_data.par_p5 = (double)x.reg_calib_data.par_p5 / 0.125f;
	x.quantized_calib_data.par_p6 = (double)x.reg_calib_data.par_p6 / 64.0f;
	x.quantized_calib_data.par_p7 = (double)x.reg_calib_data.par_p7 / 256.0f;
	x.quantized_calib_data.par_p8 = (double)x.reg_calib_data.par_p8 / 32768.0f;
	x.quantized_calib_data.par_p9 = (double)x.reg_calib_data.par_p9 / 281474976710656.0f;
	x.quantized_calib_data.par_p10 = (double)x.reg_calib_data.par_p10 / 281474976710656.0f;
	x.quantized_calib_data.par_p11 = (double)x.reg_calib_data.par_p11 / 36893488147419103232.0f;

	return x;

}

double bmp390_compensateTemp(bmp3_uncomp_data *uncomp_data, bmp3_calib_data *calib_data)
{
    uint32_t uncomp_temp = uncomp_data->temperature;
    double partial_data1;
    double partial_data2;

    partial_data1 = (double)(uncomp_temp - calib_data->quantized_calib_data.par_t1);
    partial_data2 = (double)(partial_data1 * calib_data->quantized_calib_data.par_t2);

    /* Update the compensated temperature in calib structure since this is
     * needed for pressure calculation */
    calib_data->quantized_calib_data.t_lin = partial_data2 + (partial_data1 * partial_data1) *
                                             calib_data->quantized_calib_data.par_t3;

    /* Returns compensated temperature */
    return calib_data->quantized_calib_data.t_lin;
}



double bmp390_compensatePressure(bmp3_uncomp_data *uncomp_data, bmp3_calib_data *calib_data)
{

    bmp3_quantized_calib_data *quantized_calib_data = &calib_data->quantized_calib_data;
    uint32_t uncomp_press = uncomp_data->pressure;

    /* Variable to store the compensated pressure */
    double comp_press;


    /* Temporary variables used for compensation */
    double partial_data1;
    double partial_data2;
    double partial_data3;
    double partial_data4;
    double partial_out1;
    double partial_out2;

    /*partial_data1 = quantized_calib_data->par_p6 * quantized_calib_data->t_lin;
    partial_data2 = quantized_calib_data->par_p7 * pow_bmp3(quantized_calib_data->t_lin, 2);
    partial_data3 = quantized_calib_data->par_p8 * pow_bmp3(quantized_calib_data->t_lin, 3);
    partial_out1 = quantized_calib_data->par_p5 + partial_data1 + partial_data2 + partial_data3;

    partial_data1 = quantized_calib_data->par_p2 * quantized_calib_data->t_lin;
    partial_data2 = quantized_calib_data->par_p3 * (quantized_calib_data->t_lin* quantized_calib_data->t_lin);
    partial_data3 = quantized_calib_data->par_p4 * pow_bmp3(quantized_calib_data->t_lin, 3);
    partial_out2 = (double)(uncomp_press *
                   (quantized_calib_data->par_p1 + partial_data1 + partial_data2 + partial_data3));

    partial_data1 = (double)(uncomp_press * uncomp_press);
    partial_data2 = quantized_calib_data->par_p9 + quantized_calib_data->par_p10 * quantized_calib_data->t_lin;
    partial_data3 = partial_data1 * partial_data2;
    partial_data4 = (double)(partial_data3 + uncomp_press * uncomp_press * uncomp_press * quantized_calib_data->par_p11);

    comp_press = partial_out1 + partial_out2 + partial_data4;*/


    partial_data1 = quantized_calib_data->par_p6 * quantized_calib_data->t_lin;
    partial_data2 = quantized_calib_data->par_p7 * pow_bmp3(quantized_calib_data->t_lin, 2);
    partial_data3 = quantized_calib_data->par_p8 * pow_bmp3(quantized_calib_data->t_lin, 3);
    partial_out1 = quantized_calib_data->par_p5 + partial_data1 + partial_data2 + partial_data3;
    partial_data1 = quantized_calib_data->par_p2 * quantized_calib_data->t_lin;
    partial_data2 = quantized_calib_data->par_p3 * pow_bmp3(quantized_calib_data->t_lin, 2);
    partial_data3 = quantized_calib_data->par_p4 * pow_bmp3(quantized_calib_data->t_lin, 3);
    partial_out2 = uncomp_data->pressure *
                   (quantized_calib_data->par_p1 + partial_data1 + partial_data2 + partial_data3);
    partial_data1 = pow_bmp3((double)uncomp_data->pressure, 2);
    partial_data2 = quantized_calib_data->par_p9 + quantized_calib_data->par_p10 * quantized_calib_data->t_lin;
    partial_data3 = partial_data1 * partial_data2;
    partial_data4 = partial_data3 + pow_bmp3((double)uncomp_data->pressure, 3) * quantized_calib_data->par_p11;
    comp_press = partial_out1 + partial_out2 + partial_data4;

    return comp_press;
}




