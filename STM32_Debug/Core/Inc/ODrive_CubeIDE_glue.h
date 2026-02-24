/*
 * ODrive_CubeIDE_glue.h
 *
 *  Created on: Nov 16, 2025
 *      Author: kamil
 */

#ifndef INC_ODRIVE_CUBEIDE_GLUE_H_
#define INC_ODRIVE_CUBEIDE_GLUE_H_

#include "main.h"       // For HAL types (CAN_HandleTypeDef)
#include "ODrive/ODriveCAN.h" // The library we just added

// This struct will hold a pointer to our HAL CAN Handle
struct CubeCANInterface {
    CAN_HandleTypeDef* hcan;
};

// We must provide implementations for these two functions
// that the ODriveCAN.h wrapper macro will use.
bool sendMsg(CubeCANInterface& intf, uint32_t id, uint8_t length, const uint8_t* data);
void pumpEvents(CubeCANInterface& intf);

CREATE_CAN_INTF_WRAPPER(CubeCANInterface);

#endif /* INC_ODRIVE_CUBEIDE_GLUE_H_ */
