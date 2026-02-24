#ifndef INC_ICM20602_H_
#define INC_ICM20602_H_

#ifdef __cplusplus
extern "C" {
#endif

// ---- I2C address (AD0=0 -> 0x68, AD0=1 -> 0x69). HAL expects R/W-bit-shifted.
#define ICM20602_ADDRESS   0x68

// ---- Registers
#define REG_CONFIG          0x1A   // DLPF for gyro
//#define REG_GYRO_CONFIG     0x1B   // FS_SEL
//#define REG_ACCEL_CONFIG    0x1C
#define REG_WHO_AM_I        0x75
#define REG_GYRO_XOUT_H     0x43
#define REG_SLEEP_PLL 		0x6B
// ---- WHO_AM_I value for ICM-20602
#define REG_CONFIG_GYRO 	0x1B
#define REG_CONFIG_ACC 		0x1C
#define REG_DATA_GYRO 		0x43
#define REG_DATA_GYRO_Y		0x45
#define REG_DATA_ACC 		0x3B
#define REG_DLPF 			0x1A
#define REG_DLPF_ACC 		0x1D
#define REG_SAMPLE_DIV 		0x19
#define FIFO_OFF_REG		0x23
#define FIFO_DENIED_REG		0x6A

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

#define FS_GYRO_2000 	0x1A // 0x18 pro scale 2000 dps
#define FS_ACC_8G 		0x18 // scale 16g
#define WAKE_AND_PLL 	0x02
#define BANDWIDTH 		0x07 // bandwidth 92Hz
#define BANDWIDTH_ACC 	0x07 // puvodne byla 5
#define FS500 			0x01 // pro 500Hz =1 pro 100Hz =9 pro 1kHz =0
#define FIFO_OFF		0x00
#define FIFO			0x00

#include <stdbool.h>
#include <main.h>


// ---- Gyro FS options (FS_SEL bits 4:3)
typedef enum { GFS_250=0, GFS_500=1, GFS_1000=2, GFS_2000=3 } icm_gyro_fs_t;

typedef struct{
	float angle;
	float gyro_integral_raw;
}CompFilter;


// Return sensitivity (LSB per °/s) for chosen range
static inline float icm20602_gyro_sens(icm_gyro_fs_t fs) {
    switch (fs) {
        case GFS_250:  return 131.0f;
        case GFS_500:  return 65.5f;
        case GFS_1000: return 32.8f;
        default:       return 16.4f; // GFS_2000
    }
}


void Read_Gyro();
void Calibrate_GYRO();
void Read_Data_ACC();
void Combine_sensor();
void CF_Init(CompFilter *filter, float angle_init);
void CF_Update(CompFilter *filter, float gyro, float acc_angle);

HAL_StatusTypeDef ICM20602_Init(uint8_t ad0,
                                uint8_t dlpf_cfg,
                                uint8_t smpl_div,
                                icm_gyro_fs_t fs);
HAL_StatusTypeDef ICM20602_ReadGyroRaw(uint8_t ad0, int16_t *gx, int16_t *gy, int16_t *gz);
HAL_StatusTypeDef ICM20602_ReadGyroDPS(uint8_t ad0, icm_gyro_fs_t fs,
                                       float *gx_dps, float *gy_dps, float *gz_dps);
void Print2Console(const char* msg1, const char* msg2, int ret);

void I2C_Write(uint8_t devAddr, uint8_t *data, uint8_t size);
void I2C_Read(uint8_t devAddr, uint8_t reg, uint8_t *buffer, uint8_t size);
void ICM20602_INIT();
void Calibrate_Gyro_LL();
bool I2C_IsDeviceReady_LL(I2C_TypeDef *I2Cx,
                          uint8_t devAddr,
                          uint32_t timeout);
void LL_Read_Acc();
void LL_ReadY_Gyro();
void Quick_I2C_Reset(I2C_HandleTypeDef *hi2c);


#ifdef __cplusplus
}
#endif

#endif /* INC_ICM20602_H_ */
