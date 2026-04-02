#ifndef INC_ICM20602_H_
#define INC_ICM20602_H_

#ifdef __cplusplus
extern "C" {
#endif

#define ICM20602_ADDRESS   0x68

#define REG_SLEEP_PLL 		0x6B
#define REG_CONFIG_GYRO 	0x1B
#define REG_CONFIG_ACC 		0x1C
#define REG_DATA_GYRO_Y		0x45
#define REG_DATA_ACC 		0x3B
#define REG_DLPF 			0x1A
#define REG_DLPF_ACC 		0x1D
#define REG_SAMPLE_DIV 		0x19
#define FIFO_OFF_REG		0x23
#define FIFO_DENIED_REG		0x6A

#define FS_GYRO_2000 	0x1A // 0x18 pro scale 2000 dps
#define FS_ACC_8G 		0x18 // scale 16g
#define WAKE_AND_PLL 	0x02
#define BANDWIDTH 		0x07 // bandwidth 92Hz
#define BANDWIDTH_ACC 	0x07 // puvodne byla 5
#define FS500 			0x01 // pro 500Hz =1 pro 100Hz =9 pro 1kHz =0
#define FIFO_OFF		0x00
#define FIFO			0x00

#define GYRO_SCALE      16.4
#define ACC_SCALE		2048.0

#define RAD2DEG 		57.295f
#define alpha 			0.98f

#define DIV_CON "CONNECTED\r\n"
#define DIV_DISCON "CONNECTION ERROR\r\n"
#define DIV_SLEEP "SLEEP MODE\r\n"
#define DIV_WAKE "WAKE MODE\r\n"
#define GYRO_CONFIG "GYRO CONFIGURED\r\n"
#define GYRO_ERR "GYRO ERROR\r\n"
#define DLPF_SET "DLPF SET\r\n"
#define DLPF_ERR "DLPF ERROR\r\n"
#define DLPF_SET_ACC "DLPF ACC SET\r\n"
#define DLPF_ERR_ACC "DLPF ACC ERROR\r\n"
#define SR_SET "SR SET\r\n"
#define SR_ERR "SR ERROR\r\n"
#define ACC_CONFIG "ACC CONFIGURED\r\n"
#define ACC_ERR "ACC ERROR\r\n"


#include <stdbool.h>
#include <main.h>

typedef struct{
	float angle;
	float gyro_integral_raw;
}CompFilter;


void Read_Gyro();
void Calibrate_GYRO();
void Read_Data_ACC();
void CF_Init(CompFilter *filter, float angle_init);
void CF_Update(CompFilter *filter, float gyro, float acc_angle, float start);
void ICM20602_Init();
void I2C_Bus_Recover();
void Print2Console(const char* msg1, const char* msg2, int ret);
void Read_data();


#ifdef __cplusplus
}
#endif

#endif /* INC_ICM20602_H_ */
