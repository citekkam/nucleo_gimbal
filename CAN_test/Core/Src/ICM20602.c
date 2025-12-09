// icm20602.c / .h  — minimal STM32 HAL snippet
#include "main.h"
#include <string.h>
#include "ICM20602.h"
#include <stdint.h>
#include <math.h>
#include <stdio.h>

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart2;

#define RAD2DEG 57.295
#define alpha 0.95

char msg[64];
uint8_t data[6];
int16_t offset_x = 0;
int16_t offset_y = 0;
int16_t offset_z = 0;
float gyro_x;
float gyro_y;
float gyro_z;
float deg_XZ;
uint32_t prev_time = 0;
float degree;

void Print2Console(const char* msg1, const char* msg2, int ret)
{
	if (ret == HAL_OK)
	{
		HAL_UART_Transmit(&huart2, (const uint8_t*)msg1, strlen(msg1),100);
	}
	else
	{
		HAL_UART_Transmit(&huart2, (const uint8_t*)msg2, strlen(msg2),100);
	}
	//HAL_Delay(1000);
}

void Calibrate_GYRO()
{
	int16_t ret_count = 0;
	int32_t gyro_x_cal = 0;
	int32_t gyro_y_cal = 0;
	int32_t gyro_z_cal = 0;
	for (int i = 0; i < 1000; i++) {
		HAL_StatusTypeDef ret = HAL_I2C_Mem_Read(&hi2c1, (ICM20602_ADDRESS<<1) + 1, REG_DATA_GYRO, 1, data, 6, 100);
		gyro_x_cal += (int16_t)((data[0] << 8) + data[1]);
		gyro_y_cal += (int16_t)((data[2] << 8) + data[3]);
		gyro_z_cal += (int16_t)((data[4] << 8) + data[5]);
		if (ret == HAL_OK) {
			ret_count++;
		}
	}
	offset_x = (int16_t)(gyro_x_cal / 1000);
	offset_y = (int16_t)(gyro_y_cal / 1000);
	offset_z = (int16_t)(gyro_z_cal / 1000);


	/*
	printf("CALIBRACE\r\n");
	 */
	sprintf(msg, "%d from 1000, Y offset %d\r\n",ret_count, offset_y);
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
}

HAL_StatusTypeDef ICM20602_Init(uint8_t ad0,
                                uint8_t dlpf_cfg,
                                uint8_t smpl_div,
                                icm_gyro_fs_t fs)
{
    HAL_StatusTypeDef st;
    uint8_t v;

    HAL_StatusTypeDef ret = HAL_I2C_IsDeviceReady(&hi2c1, (ICM20602_ADDRESS<<1), 1, 1000);
    Print2Console(DIV_CON, DIV_DISCON, ret);
    //printf("DIV READY\r\n");


    uint8_t temp_data = WAKE_AND_PLL;
	ret = HAL_I2C_Mem_Write(&hi2c1, (ICM20602_ADDRESS<<1) + 0, REG_SLEEP_PLL, 1, &temp_data, 1, 100);
	Print2Console(DIV_WAKE, DIV_SLEEP, ret);
	//printf("WAKE\r\n");
	HAL_Delay(500);


	/*Set up the bandwidth on 176Hz, maybe later change on 92Hz*/
	temp_data = BANDWIDTH;
	ret = HAL_I2C_Mem_Write(&hi2c1, (ICM20602_ADDRESS<<1) + 0, REG_DLPF, 1, &temp_data, 1, 100);
	Print2Console(DLPF_SET, DLPF_ERR, ret);
	//printf("FILTER\r\n");

	temp_data = BANDWIDTH_ACC;
	ret = HAL_I2C_Mem_Write(&hi2c1, (ICM20602_ADDRESS<<1) + 0, REG_DLPF_ACC, 1, &temp_data, 1, 100);
	Print2Console(DLPF_SET_ACC, DLPF_ERR_ACC, ret);
	//printf("FILTER ACC\r\n");


	/*Set up Sample rate for now it is 500Hz*/
	temp_data = FS500;
	ret = HAL_I2C_Mem_Write(&hi2c1, (ICM20602_ADDRESS<<1) + 0, REG_SAMPLE_DIV, 1, &temp_data, 1, 100);
	Print2Console(SR_SET, SR_ERR, ret);
	//printf("SMP RATE\r\n");

	/*Gyroscope configuration*/
	temp_data = FS_GYRO_1000;
	ret = HAL_I2C_Mem_Write(&hi2c1, (ICM20602_ADDRESS<<1) + 0, REG_CONFIG_GYRO, 1, &temp_data, 1, 100);
	Print2Console(GYRO_CONFIG, GYRO_ERR, ret);
	//printf("GYRO\r\n");

	temp_data = FS_ACC_8G;
	ret = HAL_I2C_Mem_Write(&hi2c1, (ICM20602_ADDRESS<<1) + 0, REG_CONFIG_ACC, 1, &temp_data, 1, 100);
	Print2Console(ACC_CONFIG, ACC_ERR, ret);
	//printf("ACC\r\n");


	/*Count offset of gyroscope*/
	Calibrate_GYRO();
}

extern int iic_err;
int16_t count_limit = 0;
void Read_Gyro() {
	int16_t gyro_x_raw, gyro_y_raw, gyro_z_raw;
	HAL_StatusTypeDef ret = HAL_I2C_Mem_Read(&hi2c1, (ICM20602_ADDRESS<<1) + 1, REG_DATA_GYRO, 1, data, 6, 100);
	if (ret != HAL_OK) {
		iic_err += 1;
	}
	gyro_x_raw = (int16_t)((data[0] << 8) + data[1]);
	gyro_y_raw = (int16_t)((data[2] << 8) + data[3]);
	gyro_z_raw = (int16_t)((data[4] << 8) + data[5]);
	gyro_x_raw -= offset_x;
	gyro_y_raw -= offset_y;
	gyro_z_raw -= offset_z;
	gyro_x = ((float)gyro_x_raw/16.4);
	gyro_y = ((float)gyro_y_raw/16.4);
	gyro_z = ((float)gyro_z_raw/16.4);
	if (gyro_y > 1990)  {
		count_limit++;
	}
	/*
	sprintf(msg, "%d %d %d\r\n", (int)gyro_x, (int)gyro_y, (int)gyro_z);
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
	 *
	 */
}

void Read_Data_ACC()
{
	int16_t acc_x_raw, acc_y_raw, acc_z_raw;
 	float acc_x, acc_y, acc_z, rad_XZ;
 	HAL_StatusTypeDef ret = HAL_I2C_Mem_Read(&hi2c1, (ICM20602_ADDRESS<<1) + 1, REG_DATA_ACC, 1, data, 6, 100);
	if (ret != HAL_OK) {
		iic_err += 1;
	}
	acc_x_raw = (int16_t)((data[0] << 8) + data[1]);
	acc_y_raw = (int16_t)((data[2] << 8) + data[3]);
	acc_z_raw = (int16_t)((data[4] << 8) + data[5]);
	acc_x = ((float)acc_x_raw /4096.0);
	acc_y = ((float)acc_y_raw /4096.0);
	acc_z = ((float)acc_z_raw /4096.0);
	rad_XZ  = (atan2f(acc_x, acc_z));
	//rad_XZ = atan2f(-acc_x, sqrt(acc_y*acc_y + acc_z*acc_z));
	deg_XZ = (rad_XZ*RAD2DEG);

	/*
	 *
	sprintf(msg, "%d\r\n", iic_err);
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
	 */
}

/*CF = Complementary Filter*/
void CF_Init(CompFilter *filter, float angle_init) {
	filter->angle = angle_init;
}

void CF_Update(CompFilter *filter, float gyro, float acc_angle) {
	float now = HAL_GetTick();
	float dt = ((now - prev_time) / 1000.0); // 0.002
	//float dt = 0.002;
	prev_time = now;

	//filter->angle = filter->angle + gyro * dt;
	float gyro_angle = filter->angle + gyro * dt;

	filter->angle = alpha * gyro_angle + (1.0 - alpha) * acc_angle;
}
