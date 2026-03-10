#ifndef INC_LLCP_H_
#define INC_LLCP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define RECIEVE_MSG 42
#define IMU_MSG_ID  51 // obsahuje uhel z IMU, polohu motoru(uhel drona)
#define HEARTBEAT_MSG_ID 52
#define REFERENCE_ANGLE_ID 53 // payload bude obsahovat hodonu uhlu ktery ma gimbal sledovat
#define START 56
#define STOP 55

typedef struct __attribute__((__packed__))
{
  uint8_t  id;
  bool     is_running;
  uint16_t messages_received;
  bool     last_trigger;
  uint8_t  last_trigger_num;
}heartbeat_msg;

typedef struct __attribute__((__packed__))
{
	uint8_t  id;
	//uint32_t t_ms;
	uint8_t	 angle;
	uint8_t	 drone_angle;
}imu_msg;

typedef struct __attribute__((__packed__))
{
	uint8_t  id;
	//uint32_t t_ms;
	uint8_t    value;
}recieve_msg;

typedef struct __attribute__((__packed__))
{
	uint8_t  id;
}ACK_msg;

void send_heartbeat();
void send_imu(uint8_t id, uint8_t angle_data, uint8_t angle_drone);
void send_ACK(uint8_t id);
bool receive_message();
extern recieve_msg recieved_msg;

#ifdef __cplusplus
}
#endif

#endif /* INC_LLCP_H_ */
