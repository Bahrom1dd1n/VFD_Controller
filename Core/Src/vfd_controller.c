/*
 * vfd_controller.c
 *
 *  Created on: Nov 7, 2025
 *      Author: bahromiddin
 */

#include "vfd_controller.h"
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

/* Default Delixi Config */
const vfd_config_t delixi_default_config = {
    .monitoring_address = 0x9000,
    .freq_address = 0xA001,
    .status_address = 0xB000,
    .issue_address = 0xB001,
    .monitoring_data_len = 5,
    .status_data_len = 1,
    .issue_data_len = 1
};

/* Initialize VFD */
void vfd_init(vfd_t *vfd, vfd_context_t *ctx, vfd_config_t *cfg) {
    if (vfd == NULL || ctx == NULL || cfg == NULL) return;
    vfd->context = *ctx;
    vfd->config = *cfg;
    vfd->running_frequency = 0.0;
    vfd->reference_frequency = 0.0;
    vfd->output_current = 0.0;
    vfd->output_voltage = 0;
    vfd->bus_line_voltage = 0.0;
    vfd->status = DELIXI_NONE;
    vfd->issue = DELIXI_NO_FAULT;
}

/* Update VFD Data */
int vfd_update(vfd_t *vfd) {
    if (vfd == NULL) return VFD_ERROR_INVALID_PARAM;

    uint16_t monitoring_buf[5];  /* Max len */
    uint16_t status_val;
    uint16_t issue_val;
    int err;

    /* Read Monitoring */
    err = vfd->context.mb_read_multi_reg(vfd->context.slave_address,
                                         vfd->config.monitoring_address,
                                         vfd->config.monitoring_data_len,
                                         monitoring_buf);
    if (err != VFD_SUCCESS) return VFD_ERROR_MODBUS_FAIL;

    vfd->running_frequency = monitoring_buf[0] / 100.0f;
    vfd->reference_frequency = monitoring_buf[1] / 100.0f;
    vfd->output_current = monitoring_buf[2] / 100.0f;
    vfd->output_voltage = monitoring_buf[3];
    vfd->bus_line_voltage = monitoring_buf[4] / 10.0f;

    /* Read Status */
    err = vfd->context.mb_read_single_reg(vfd->context.slave_address,
                                          vfd->config.status_address,
                                          &status_val);
    if (err != VFD_SUCCESS) return VFD_ERROR_MODBUS_FAIL;
    vfd->status = (vfd_status_t)status_val;

    /* Read Issue */
    err = vfd->context.mb_read_single_reg(vfd->context.slave_address,
                                          vfd->config.issue_address,
                                          &issue_val);
    if (err != VFD_SUCCESS) return VFD_ERROR_MODBUS_FAIL;
    vfd->issue = (vfd_issue_t)issue_val;

    return VFD_SUCCESS;
}

/* Set Running Frequency */
int vfd_set_running_freq(vfd_t *vfd, float speed) {
    if (vfd == NULL || speed < 0.0f || speed > 100.0f) return VFD_ERROR_OUT_OF_RANGE;
    uint16_t val = (uint16_t)(speed * 100.0f);
    return vfd->context.mb_write_single_reg(vfd->context.slave_address, vfd->config.freq_address, val);
}

/* Getters */
float vfd_get_running_freq(vfd_t *vfd) {
    return (vfd != NULL) ? vfd->running_frequency : 0.0f;
}

float vfd_get_reference_freq(vfd_t *vfd) {
    return (vfd != NULL) ? vfd->reference_frequency : 0.0f;
}

float vfd_get_output_current(vfd_t *vfd) {
    return (vfd != NULL) ? vfd->output_current : 0.0f;
}

uint16_t vfd_get_output_voltage(vfd_t *vfd) {
    return (vfd != NULL) ? vfd->output_voltage : 0;
}

float vfd_get_bus_line_voltage(vfd_t *vfd) {
    return (vfd != NULL) ? vfd->bus_line_voltage : 0.0f;
}

vfd_status_t vfd_get_status(vfd_t *vfd) {
    return (vfd != NULL) ? vfd->status : DELIXI_NONE;
}

vfd_issue_t vfd_get_issue(vfd_t *vfd) {
    return (vfd != NULL) ? vfd->issue : DELIXI_NO_FAULT;
}

const char* vfd_get_status_string(vfd_t *vfd) {
    if (vfd == NULL || vfd->status >= DELIXI_STATUS_COUNT) return "Unknown";
    return status_strings[vfd->status];
}

const char* vfd_get_issue_string(vfd_t *vfd) {
    if (vfd == NULL || vfd->issue >= DELIXI_ISSUE_COUNT) return "Unknown";
    return issue_strings[vfd->issue];
}
