// Author: ODrive Robotics Inc.
// License: MIT
// Documentation: https://docs.odriverobotics.com/v/latest/guides/arduino-can-guide.html
//
// This file has been modified from the Arduino original to work with
// STM32CubeIDE and the HAL library.

#include "ODrive/ODriveCAN.h"

// --- STM32CubeIDE Includes ---
#include "main.h"       // For HAL_GetTick()
#include <string.h>     // For memcpy()
#include <stdio.h>      // For printf()
// ---

bool ODriveCAN::clearErrors() {
    Clear_Errors_msg_t msg;
    return send(msg);
}

bool ODriveCAN::setPosition(float position, float velocity_feedforward, float torque_feedforward) {
    Set_Input_Pos_msg_t msg;

    msg.Input_Pos = position;
    msg.Vel_FF = velocity_feedforward;
    msg.Torque_FF = torque_feedforward;

    return send(msg);
}

bool ODriveCAN::setVelocity(float velocity, float torque_feedforward) {
    Set_Input_Vel_msg_t msg;

    msg.Input_Vel = velocity;
    msg.Input_Torque_FF = torque_feedforward;

    return send(msg);
}

bool ODriveCAN::setControllerMode(uint8_t control_mode, uint8_t input_mode) {
    Set_Controller_Mode_msg_t msg;

    msg.Control_Mode = control_mode;
    msg.Input_Mode = input_mode;

    return send(msg);
}

bool ODriveCAN::setTorque(float torque) {
    Set_Input_Torque_msg_t msg;

    msg.Input_Torque = torque;

    return send(msg);
}

bool ODriveCAN::setState(ODriveAxisState requested_state) {
    Set_Axis_State_msg_t msg;

    msg.Axis_Requested_State = (uint32_t)requested_state;

    return send(msg);
}

bool ODriveCAN::setLimits(float velocity_limit, float current_soft_max) {
    Set_Limits_msg_t msg;

    msg.Velocity_Limit = velocity_limit;
    msg.Current_Limit = current_soft_max;

    return send(msg);
}

bool ODriveCAN::setPosGain(float pos_gain) {
    Set_Pos_Gain_msg_t msg;

    msg.Pos_Gain = pos_gain;

    return send(msg);
}

bool ODriveCAN::setVelGains(float vel_gain, float vel_integrator_gain) {
    Set_Vel_Gains_msg_t msg;

    msg.Vel_Gain = vel_gain;
    msg.Vel_Integrator_Gain = vel_integrator_gain;

    return send(msg);
}

bool ODriveCAN::setAbsolutePosition(float abs_pos) {
    Set_Absolute_Position_msg_t msg;

    msg.Position = abs_pos;

    return send(msg);
}

bool ODriveCAN::setTrapezoidalVelLimit(float vel_limit) {
    Set_Traj_Vel_Limit_msg_t msg;

    msg.Traj_Vel_Limit = vel_limit;

    return send(msg);
}

bool ODriveCAN::setTrapezoidalAccelLimits(float accel_limit, float decel_limit) {
    Set_Traj_Accel_Limits_msg_t msg;

    msg.Traj_Accel_Limit = accel_limit;
    msg.Traj_Decel_Limit = decel_limit;

    return send(msg);
}

bool ODriveCAN::getCurrents(Get_Iq_msg_t& msg, uint16_t timeout_ms) {
    return request(msg, timeout_ms);
}

bool ODriveCAN::getTemperature(Get_Temperature_msg_t& msg, uint16_t timeout_ms) {
    return request(msg, timeout_ms);
}

bool ODriveCAN::getError(Get_Error_msg_t& msg, uint16_t timeout_ms) {
    return request(msg, timeout_ms);
}

bool ODriveCAN::getVersion(Get_Version_msg_t& msg, uint16_t timeout_ms) {
    return request(msg, timeout_ms);
}

bool ODriveCAN::getFeedback(Get_Encoder_Estimates_msg_t& msg, uint16_t timeout_ms) {
    return request(msg, timeout_ms);
}

bool ODriveCAN::getBusVI(Get_Bus_Voltage_Current_msg_t& msg, uint16_t timeout_ms) {
    return request(msg, timeout_ms);
}

bool ODriveCAN::getPower(Get_Powers_msg_t& msg, uint16_t timeout_ms) {
    return request(msg, timeout_ms);
}

bool ODriveCAN::reset(ResetAction action) {
    Reboot_msg_t msg;
    msg.Action = action;

    return send(msg);
}

void ODriveCAN::onReceive(uint32_t id, uint8_t length, const uint8_t* data) {
if(false) // uncomment for debug
{
    // --- Replaced Serial with printf ---
    int byte_index = length - 1;
    printf("received:\r\n");
    printf("  id: 0x%lX\r\n", id);
    printf("  data: 0x");
    while (byte_index >= 0)
        printf("%02X", data[byte_index--]);
    printf("\r\n");
    // ---
}
    // Check that the message is meant for this node.
    if (node_id_ != (id >> ODriveCAN::kNodeIdShift))
        return;
    // If the message is requested, copy it in the request buffer and exit.
    if ((id & ODriveCAN::kCmdIdBits) == requested_msg_id_) {
        memcpy(buffer_, data, length);
        requested_msg_id_ = REQUEST_PENDING;
        return;
    };
    // Check if any of the registered callback handlers apply. Useful for cyclic
    // messages.
    switch (id & ODriveCAN::kCmdIdBits) {
        case Get_Encoder_Estimates_msg_t::cmd_id: {
            Get_Encoder_Estimates_msg_t estimates;
            estimates.decode_buf(data);
            if (feedback_callback_)
                feedback_callback_(estimates, feedback_user_data_);
            break;
        }
        case Get_Torques_msg_t::cmd_id: {
            Get_Torques_msg_t estimates;
            estimates.decode_buf(data);
            if (torques_callback_)
                torques_callback_(estimates, torques_user_data_);
            break;
        }
        case Heartbeat_msg_t::cmd_id: {
            Heartbeat_msg_t status;
            status.decode_buf(data);
            if (axis_state_callback_ != nullptr)
                axis_state_callback_(status, axis_state_user_data_);
            else
                printf("missing callback\r\n"); // --- Replaced Serial ---
            break;
        }
        case Get_Temperature_msg_t::cmd_id: {
            Get_Temperature_msg_t temperature;
            temperature.decode_buf(data);
            if (temperature_callback_)
                temperature_callback_(temperature, temperature_user_data_);
            break;
        }
        case Get_Bus_Voltage_Current_msg_t::cmd_id: {
            Get_Bus_Voltage_Current_msg_t bus_vi;
            bus_vi.decode_buf(data);
            if (busVI_callback_)
                busVI_callback_(bus_vi, busVI_user_data_);
            break;
        }
        case Get_Iq_msg_t::cmd_id: {
            Get_Iq_msg_t iq;
            iq.decode_buf(data);
            if (currents_callback_)
                currents_callback_(iq, currents_user_data_);
            break;
        }
        case Get_Error_msg_t::cmd_id: {
            Get_Error_msg_t error;
            error.decode_buf(data);
            if (error_callback_)
                error_callback_(error, error_user_data_);
            break;
        }
        default: {
            if (requested_msg_id_ == REQUEST_PENDING)
                return;
#ifdef DEBUG
            // --- Replaced Serial with printf ---
            printf("waiting for: 0x%X\r\n", requested_msg_id_);
#endif // DEBUG
        }
    }
}

/**
 * @brief This function is modified for STM32 HAL.
 * It uses HAL_GetTick() (milliseconds) instead of micros() (microseconds).
 */
bool ODriveCAN::awaitMsg(uint16_t timeout_ms) {
    // Use HAL_GetTick() which returns milliseconds
    uint32_t start_time = HAL_GetTick();

    while (requested_msg_id_ != REQUEST_PENDING) {
        can_intf_.pump_events(); // pump event loop while waiting

        // Check if (current_ms - start_ms) > timeout_ms
        if ((HAL_GetTick() - start_time) > timeout_ms) {
            return false; // Timeout
        }
    }
    return true; // Message received
}
