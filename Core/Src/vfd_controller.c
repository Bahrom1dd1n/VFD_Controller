/*
 * ctx_controller.c
 *
 *  Created on: Nov 7, 2025
 *      Author: bahromiddin
 */

#include "ctx_controller.h"
#include <stddef.h>  /* For NULL */
#include <stdint.h>

/* Status Strings */
static const char* status_strings[DELIXI_STATUS_COUNT] = {
    "None",
    "Forward Run",
    "Reverse Run",
    "Stop"
};

/* Issue Strings */
static const char* issue_strings[DELIXI_ISSUE_COUNT] = {
    "No fault",
    "Over-current at constant speed",
    "Over-current at acceleration",
    "Over-current at deceleration",
    "Over-voltage at constant speed",
    "Over-voltage at acceleration",
    "Over-voltage at deceleration",
    "Module Fault",
    "Undervoltage",
    "Frequency inverter Overload",
    "Motor Overload",
    "Input Default Phase",
    "Output Default Phase",
    "External Fault",
    "Abnormal Communication",
    "Frequency inverter Overheat",
    "Hardware Fault of Frequency inverter",
    "Motor Earthing Short Circuit",
    "Motor Identification Error",
    "Motor Off-load",
    "PID Feedback Loss",
    "User-Defined Fault 1",
    "User-Defined Fault 2",
    "Accumulative Power-on Time Reached",
    "Accumulative Running Time Reached",
    "Encoder Fault",
    "Parameter Read-Write Abnormity",
    "Motor Overheat",
    "Larger Speed Deviation",
    "Motor Overspeed",
    "Initial Position Error",
    "Current Test Fault",
    "Contactor",
    "Abnormity of Current Test",
    "Fast Current-limiting Timeout",
    "Motor Switch at Running",
    "24V Power Fault",
    "Driving Power Supply Fault",
    "Buffer Resistance Fault"
};

/* Initialize VFD */
void ctx_init(ctx_context_t *ctx, ctx_config_t *cfg) {
    if (ctx == NULL || ctx == NULL || cfg == NULL) return;
    ctx->config = *cfg;
    ctx->running_frequency = 0.0;
    ctx->reference_frequency = 0.0;
    ctx->output_current = 0.0;
    ctx->output_voltage = 0;
    ctx->bus_line_voltage = 0.0;
//    ctx->status = DELIXI_NONE;
//    ctx->issue = DELIXI_NO_FAULT;
}

/* Update VFD Data */
int ctx_update(ctx_context_t *ctx) {
    if (ctx == NULL) return VFD_ERROR_INVALID_PARAM;

    uint16_t monitoring_buf[5];  /* Max len */
//    uint16_t status_val;
//    uint16_t issue_val;
    int err;

    /* Read Monitoring */
    err = ctx->mb_read_multi_reg(ctx->slave_address,
                                         ctx->config.monitoring_address,
                                         ctx->config.monitoring_data_len,
                                         monitoring_buf);
    if (err != VFD_SUCCESS) return VFD_ERROR_MODBUS_FAIL;

    ctx->running_frequency = monitoring_buf[0] *0.010F;
    ctx->reference_frequency = monitoring_buf[1] *0.01F;
    ctx->output_current = monitoring_buf[2] * 0.01F;
    ctx->output_voltage = monitoring_buf[3];
    ctx->bus_line_voltage = monitoring_buf[4]*0.1F;

    /* Read Status */
    err = ctx->mb_read_single_reg(ctx->slave_address,
                                          ctx->config.status_address,
                                          &status_val);
    if (err != VFD_SUCCESS) return VFD_ERROR_MODBUS_FAIL;
    ctx->status = (ctx_status_t)status_val;

//    /* Read Issue */
//    err = ctx->mb_read_single_reg(ctx->slave_address,
//                                          ctx->config.issue_address,
//                                          &issue_val);
//    if (err != VFD_SUCCESS) return VFD_ERROR_MODBUS_FAIL;
//    ctx->issue = (ctx_issue_t)issue_val;

    return VFD_SUCCESS;
}

/* Set Running Frequency */
int ctx_set_running_freq(ctx_context_t *ctx, float speed) {
    if (ctx == NULL || speed < 0.0f || speed > 100.0f) return VFD_ERROR_OUT_OF_RANGE;
    uint16_t val = (uint16_t)(speed * 100.0f);
    return ctx->mb_write_single_reg(ctx->slave_address, ctx->config.freq_address, val);
}

/* Getters */
float ctx_get_running_freq(ctx_t *ctx) {
    return (ctx != NULL) ? ctx->running_frequency : 0.0f;
}

float ctx_get_reference_freq(ctx_t *ctx) {
    return (ctx != NULL) ? ctx->reference_frequency : 0.0f;
}

float ctx_get_output_current(ctx_t *ctx) {
    return (ctx != NULL) ? ctx->output_current : 0.0f;
}

uint16_t ctx_get_output_voltage(ctx_t *ctx) {
    return (ctx != NULL) ? ctx->output_voltage : 0;
}

float ctx_get_bus_line_voltage(ctx_t *ctx) {
    return (ctx != NULL) ? ctx->bus_line_voltage : 0.0f;
}

ctx_status_t ctx_get_status(ctx_t *ctx) {
    return (ctx != NULL) ? ctx->status : DELIXI_NONE;
}

ctx_issue_t ctx_get_issue(ctx_t *ctx) {
    return (ctx != NULL) ? ctx->issue : DELIXI_NO_FAULT;
}
/*
const char* ctx_get_status_string(ctx_t *ctx) {
    if (ctx == NULL || ctx->status >= DELIXI_STATUS_COUNT) return "Unknown";
    return status_strings[ctx->status];
}

const char* ctx_get_issue_string(ctx_t *ctx) {
    if (ctx == NULL || ctx->issue >= DELIXI_ISSUE_COUNT) return "Unknown";
    return issue_strings[ctx->issue];
}*/
