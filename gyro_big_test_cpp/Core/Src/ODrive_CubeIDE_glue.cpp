#include "ODrive_CubeIDE_glue.h"

// We need to know about the odrv0 object, which will be in main.cpp
extern ODriveCAN odrv0;

/**
 * @brief This is our implementation of the sendMsg function.
 * It takes the ODrive data and sends it using the STM32 HAL.
 */
bool sendMsg(CubeCANInterface& intf, uint32_t id, uint8_t length, const uint8_t* data) {
    CAN_TxHeaderTypeDef tx_header;
    tx_header.StdId = id;
    tx_header.IDE = CAN_ID_STD;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.DLC = length;
    tx_header.TransmitGlobalTime = DISABLE;

    uint32_t tx_mailbox; // This will be filled by the HAL

    // Call the HAL function to send the message
    if (HAL_CAN_AddTxMessage(intf.hcan, &tx_header, (uint8_t*)data, &tx_mailbox) != HAL_OK) {
        // Handle CAN send error
        Error_Handler();
        return false;
    }
    return true;
}

/**
 * @brief This is our implementation of the pumpEvents function.
 * It checks the CAN FIFO for new messages and passes them to the
 * ODrive library's onReceive method.
 */
void pumpEvents(CubeCANInterface& intf) {
    // Check if any messages are waiting in FIFO 0
    if (HAL_CAN_GetRxFifoFillLevel(intf.hcan, CAN_RX_FIFO0) > 0) {
        CAN_RxHeaderTypeDef rx_header;
        uint8_t rx_data[8];

        // Get the message
        if (HAL_CAN_GetRxMessage(intf.hcan, CAN_RX_FIFO0, &rx_header, rx_data) == HAL_OK) {

            // Pass the received message to the ODrive library
            // We use the global odrv0 object declared in main.cpp
            odrv0.onReceive(rx_header.StdId, rx_header.DLC, rx_data);
        }
    }
}
