/*
 * vfd_controller.c
 *
 *  Created on: Nov 7, 2025
 *      Author: bahromiddin
 */

#include <stddef.h>  /* For NULL */
#include <stdint.h>
#include "vfd_controller.h"
#include "vfd_config.h"

/* Status Strings (full definition here) */
const char *status_strings[VFD_STATUS_COUNT] = {
    "None", "Forward Run", "Reverse Run", "Stop"
};

/* Issue Strings (full definition here) */
const char *issue_strings[VFD_ISSUE_COUNT] = {
    "No fault",
    "Over-current at constant speed", "Over-current at acceleration",
    "Over-current at deceleration", "Over-voltage at constant speed",
    "Over-voltage at acceleration", "Over-voltage at deceleration",
    "Module Fault", "Undervoltage", "Frequency inverter Overload",
    "Motor Overload", "Input Default Phase", "Output Default Phase",
    "External Fault", "Abnormal Communication",
    "Frequency inverter Overheat", "Hardware Fault of Frequency inverter",
    "Motor Earthing Short Circuit", "Motor Identification Error",
    "Motor Off-load", "PID Feedback Loss", "User-Defined Fault 1",
    "User-Defined Fault 2", "Accumulative Power-on Time Reached",
    "Accumulative Running Time Reached", "Encoder Fault",
    "Parameter Read-Write Abnormity", "Motor Overheat",
    "Larger Speed Deviation", "Motor Overspeed", "Initial Position Error",
    "Current Test Fault", "Contactor", "Abnormity of Current Test",
    "Fast Current-limiting Timeout", "Motor Switch at Running",
    "24V Power Fault", "Driving Power Supply Fault",
    "Buffer Resistance Fault"
};
/* Initialize VFD */
void vfd_init(vfd_context_t *ctx) {
    if (ctx == NULL || ctx == NULL) return;
    ctx->running_frequency = 0.0;
    ctx->reference_frequency = 0.0;
    ctx->output_current = 0.0;
    ctx->output_voltage = 0;
    ctx->bus_line_voltage = 0.0;
    ctx->status = VFD_NONE;
    ctx->issue = VFD_NO_FAULT;
}

/* Update VFD Data */
int vfd_update(vfd_context_t *ctx) {
    if (ctx == NULL) return VFD_ERROR_INVALID_PARAM;

    uint16_t monitoring_buf[5];  /* Max len */
    uint16_t status_val;
    uint16_t issue_val;
    vfd_issue err;

    /* Read Monitoring */
    err = ctx->mb_read_multi_reg(ctx->slave_address,
                                         VFD_MONITORING_ADDRESS,
                                         VFD_SET_FREQ_ADDRESS,
                                         monitoring_buf);
    if (err != VFD_SUCCESS) return VFD_ERROR_MODBUS_FAIL;

    ctx->running_frequency = monitoring_buf[0] *0.010F;
    ctx->reference_frequency = monitoring_buf[1] *0.01F;
    ctx->output_current = monitoring_buf[2] * 0.01F;
    ctx->output_voltage = monitoring_buf[3];
    ctx->bus_line_voltage = monitoring_buf[4]*0.1F;

    /* Read Status */

    err = ctx->mb_read_single_reg(ctx->slave_address,
                                          VFD_STATUS_ADDRESS,
                                          &status_val);
    if (err != VFD_SUCCESS) return VFD_ERROR_MODBUS_FAIL;
    ctx->status = (vfd_status)status_val;

//    /* Read Issue */
    err = ctx->mb_read_single_reg(ctx->slave_address,
                                          VFD_ISSUE_ADDRESS,
                                          &issue_val);
    if (err != VFD_SUCCESS) return VFD_ERROR_MODBUS_FAIL;
    ctx->issue = (vfd_issue)issue_val;

    return VFD_SUCCESS;
}

/* Set Running Frequency */
/* Set Running Frequency (% of max, 0.00~100.00) */
int vfd_set_running_freq(vfd_context_t *ctx, float speed) {
    if (ctx == NULL || speed < 0.0f || speed > 100.0f) return VFD_ERROR_OUT_OF_RANGE;
    uint16_t val = (uint16_t)(speed * 100.0f);  // Scale to 0~10000 (0.01% resolution)
    vfd_issue err = ctx->mb_write_single_reg(ctx->slave_address, VFD_SET_FREQ_ADDRESS, val);
    return (err == VFD_SUCCESS) ? VFD_SUCCESS : VFD_ERROR_MODBUS_FAIL;
}

/* Getters */
float vfd_get_running_freq(vfd_context_t *ctx) {
    return (ctx != NULL) ? ctx->running_frequency : 0.0f;
}

float vfd_get_reference_freq(vfd_context_t *ctx) {
    return (ctx != NULL) ? ctx->reference_frequency : 0.0f;
}

float vfd_get_output_current(vfd_context_t *ctx) {
    return (ctx != NULL) ? ctx->output_current : 0.0f;
}

uint16_t vfd_get_output_voltage(vfd_context_t *ctx) {
    return (ctx != NULL) ? ctx->output_voltage : 0;
}

float vfd_get_bus_line_voltage(vfd_context_t *ctx) {
    return (ctx != NULL) ? ctx->bus_line_voltage : 0.0f;
}

vfd_status vfd_get_status(vfd_context_t *ctx) {
    return (ctx != NULL) ? ctx->status : VFD_NONE;
}

vfd_issue vfd_get_issue(vfd_context_t *ctx) {
    return (ctx != NULL) ? ctx->issue : VFD_NO_FAULT;
}

const char* vfd_get_status_string(vfd_context_t *ctx) {
    if (ctx == NULL || ctx->status >= VFD_STATUS_COUNT) return "Unknown";
    return status_strings[ctx->status];
}

const char* vfd_get_issue_string(vfd_context_t *ctx) {
    if (ctx == NULL || ctx->issue >= VFD_ISSUE_COUNT) return "Unknown";
    return issue_strings[ctx->issue];
}
