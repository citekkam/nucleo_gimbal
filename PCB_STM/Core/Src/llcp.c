#include "llcp_lib.h"
#include "main.h"
#include <string.h>
#include "ICM20602.h"
#include "llcp.h"
#include <stdbool.h>
#include "usbd_cdc_if.h"

// Pokud nemáte LL hlavičky v main.h, je potřeba je přidat (např. #include "stm32g4xx_ll_usart.h")

extern UART_HandleTypeDef huart2;

#define TX_BUFFER_LEN 255
uint8_t tx_buffer[TX_BUFFER_LEN];


LLCP_Receiver_t llcp_receiver;

uint16_t num_msg_received = 0;

extern volatile uint8_t usbRxFlag;
extern uint16_t  usbRxBufLen;
extern uint8_t usbRxBuf[];

bool trigger_cmd = false;
uint8_t trigger_num = 0;

uint8_t recieved_value = 0;
bool got_new_servo_pos = false;

void send_heartbeat() {
  heartbeat_msg my_msg;
  uint16_t msg_len;

  // fill the message with data
  my_msg.id = HEARTBEAT_MSG_ID;
  my_msg.is_running = true;
  my_msg.messages_received = num_msg_received;
  my_msg.last_trigger = trigger_cmd;
  my_msg.last_trigger_num = trigger_num;

  // llcp_prepareMessage will fill your TX buffer
  msg_len = llcp_prepareMessage((uint8_t*)&my_msg, sizeof(my_msg), tx_buffer);

  CDC_Transmit_FS(tx_buffer, msg_len);
}

void send_imu(uint8_t id, int16_t angle_data) {
	imu_msg my_msg_imu;
	uint16_t msg_len;

	my_msg_imu.id = id;
	//my_msg_imsu.t_ms = 42;
	my_msg_imu.angle = angle_data;

	// llcp_prepareMessage will fill your TX buffer
	msg_len = llcp_prepareMessage((uint8_t*)&my_msg_imu, sizeof(my_msg_imu), tx_buffer);

	// Odeslání přes LL
	CDC_Transmit_FS(tx_buffer, msg_len);
}

void send_imu_raw(uint8_t id, int16_t ax,int16_t ay,int16_t az,int16_t gx, int16_t gy, int16_t gz) {
	imu_msg_raw my_msg_imu_raw;
	uint16_t msg_len;

	my_msg_imu_raw.id = id;
	//my_msg_imsu.t_ms = 42;
	my_msg_imu_raw.ax = ax;
	my_msg_imu_raw.ay = ay;
	my_msg_imu_raw.az = az;
	my_msg_imu_raw.gx = gx;
	my_msg_imu_raw.gy = gy;
	my_msg_imu_raw.gz = gz;

	// llcp_prepareMessage will fill your TX buffer
	msg_len = llcp_prepareMessage((uint8_t*)&my_msg_imu_raw, sizeof(my_msg_imu_raw), tx_buffer);

	// Odeslání přes LL
	CDC_Transmit_FS(tx_buffer, msg_len);
}

void send_plot_data(uint8_t id, int16_t angle_motor,int16_t angle_imu, int16_t reference_angle) {
	plot_data my_plot_data;
	uint16_t msg_len;

	my_plot_data.id = id;
	//my_msg_imsu.t_ms = 42;
	my_plot_data.angle_motor = angle_motor;
	my_plot_data.angle_imu = angle_imu;
	my_plot_data.reference_angle = reference_angle;


	// llcp_prepareMessage will fill your TX buffer
	msg_len = llcp_prepareMessage((uint8_t*)&my_plot_data, sizeof(my_plot_data), tx_buffer);

	// Odeslání přes LL
	CDC_Transmit_FS(tx_buffer, msg_len);
}

void send_ACK(uint8_t id)
{
	ACK_msg my_ACK_msg;
	uint16_t msg_len;

	char uart_tx_buf[128];
	uint16_t uart_tx_len = 0;

	my_ACK_msg.id = id;

	// llcp_prepareMessage will fill your TX buffer
	msg_len = llcp_prepareMessage((uint8_t*)&my_ACK_msg, sizeof(my_ACK_msg), tx_buffer);

	uart_tx_len += sprintf(uart_tx_buf + uart_tx_len, "ACK id=%d len=%d | ", id, msg_len);
	    for (uint16_t i = 0; i < msg_len; i++) {
	        uart_tx_len += sprintf(uart_tx_buf + uart_tx_len, "%d ", tx_buffer[i]);
	    }
	    uart_tx_len += sprintf(uart_tx_buf + uart_tx_len, "\r\n");

	    // Send via USART2
	    HAL_UART_Transmit(&huart2, (uint8_t*)uart_tx_buf, uart_tx_len, 100);

	CDC_Transmit_FS(tx_buffer, msg_len);
}


bool receive_message()
{
	bool got_valid_msg = false;
	LLCP_Message_t *llcp_message_ptr = NULL;

  // Použijeme while smyčku pro vyčtení všeho, co je aktuálně v bufferu UARTu.
  // Je to robustnější než IF, protože LL je rychlé a buffer může obsahovat více znaků.
	if (usbRxFlag) {

    // Přečtení bajtu přímo z registru
		for (uint32_t i = 0; i < usbRxBufLen; i++) {
			uint8_t ch = usbRxBuf[i];
			bool checksum_ok = false;

			if (llcp_processChar(ch, &llcp_receiver, &llcp_message_ptr, &checksum_ok)) {
				if (checksum_ok && llcp_message_ptr) {
					num_msg_received++;

					switch (llcp_message_ptr->payload[0]) {
						case HEARTBEAT_MSG_ID: {
						  got_valid_msg = true;
						  break;
						}
						case IMU_MSG_ID: {
						  got_valid_msg = true;
						  break;
						}
						case REFERENCE_ANGLE_ID: {
						  memcpy(&recieved_msg, llcp_message_ptr->payload, sizeof(recieve_msg));
						  got_valid_msg = true;
						  break;
						}

						case START: {
						  memcpy(&recieved_msg, llcp_message_ptr->payload, sizeof(recieve_msg));
						  got_valid_msg = true;
						  break;
						}
						case STOP: {
						  memcpy(&recieved_msg, llcp_message_ptr->payload, sizeof(recieve_msg));
						  got_valid_msg = true;
						  break;
						}
						case START_INFO_ID: {
						  memcpy(&recieved_msg, llcp_message_ptr->payload, sizeof(recieve_msg));
						  got_valid_msg = true;
						  break;
						}
						case STOP_INFO_ID: {
						  memcpy(&recieved_msg, llcp_message_ptr->payload, sizeof(recieve_msg));
						  got_valid_msg = true;
						  break;
						}


						default: break;
						}
				}
			}
		}
		usbRxFlag = 0;
	}

	return got_valid_msg;
}
