/*
 * vfd_controller.h
 *
 *  Created on: Nov 7, 2025
 *      Author: bahromiddin
 */

#ifndef INC_VFD_CONTROLLER_H_
#define INC_VFD_CONTROLLER_H_
#include <stdint.h>

typedef struct {
	uint16_t slave_address;
	void (*mb_write_single_reg)(uint16_t,uint16_t,uint16_t);
	void (*mb_write_multi_reg)(uint16_t,uint16_t,uint16_t,uint16_t*);
	uint16_t (*mb_read_single_reg)(uint16_t,uint16_t);
	void (*mb_read_multi_reg)(uint16_t,uint16_t,uint16_t,uint16_t*);
}vfd_context_t;

struct vfd_config{
	uint16_t monitoring_address;
	uint16_t freq_address;
	uint16_t status_address;
	uint16_t issue_address;

	uint16_t monitoring_data_len;
	uint16_t status_data_len;
	uint16_t issue_data_len;
};
/* Error Codes */
#define VFD_SUCCESS              0
#define VFD_ERROR_INVALID_PARAM -1
#define VFD_ERROR_MODBUS_FAIL   -2
#define VFD_ERROR_OUT_OF_RANGE  -3

/* Delixi Status Enum */
typedef enum {
    DELIXI_NONE = 0,
    DELIXI_FORWARD_RUN,
    DELIXI_REVERSE_RUN,
    DELIXI_STOP,
    DELIXI_STATUS_COUNT
} vfd_status_t;

/* Delixi Issue Enum */
typedef enum {
    DELIXI_NO_FAULT = 0,
    DELIXI_OVER_CURRENT_CONSTANT_SPEED,
    DELIXI_OVER_CURRENT_ACCELERATION,
    DELIXI_OVER_CURRENT_DECELERATION,
    DELIXI_OVER_VOLTAGE_CONSTANT_SPEED,
    DELIXI_OVER_VOLTAGE_ACCELERATION,
    DELIXI_OVER_VOLTAGE_DECELERATION,
    DELIXI_MODULE_FAULT,
    DELIXI_UNDERVOLTAGE,
    DELIXI_FREQUENCY_INVERTER_OVERLOAD,
    DELIXI_MOTOR_OVERLOAD,
    DELIXI_INPUT_DEFAULT_PHASE,
    DELIXI_OUTPUT_DEFAULT_PHASE,
    DELIXI_EXTERNAL_FAULT,
    DELIXI_ABNORMAL_COMMUNICATION,
    DELIXI_INVERTER_OVERHEAT,
    DELIXI_INVERTER_HARDWARE_FAULT,
    DELIXI_MOTOR_EARTHING_SHORT_CIRCUIT,
    DELIXI_MOTOR_IDENTIFICATION_ERROR,
    DELIXI_MOTOR_OFFLOAD,
    DELIXI_PID_FEEDBACK_LOSS,
    DELIXI_USER_DEFINED_FAULT_1,
    DELIXI_USER_DEFINED_FAULT_2,
    DELIXI_POWER_ON_TIME_REACHED,
    DELIXI_RUNNING_TIME_REACHED,
    DELIXI_ENCODER_FAULT,
    DELIXI_PARAM_RW_ABNORMITY,
    DELIXI_MOTOR_OVERHEAT,
    DELIXI_LARGE_SPEED_DEVIATION,
    DELIXI_MOTOR_OVERSPEED,
    DELIXI_INITIAL_POSITION_ERROR,
    DELIXI_CURRENT_TEST_FAULT,
    DELIXI_CONTACTOR,
    DELIXI_CURRENT_TEST_ABNORMITY,
    DELIXI_FAST_CURRENT_LIMIT_TIMEOUT,
    DELIXI_MOTOR_SWITCH_RUNNING,
    DELIXI_24V_POWER_FAULT,
    DELIXI_DRIVING_POWER_FAULT,
    DELIXI_BUFFER_RESISTANCE_FAULT,
    DELIXI_ISSUE_COUNT
} vfd_issue_t;

/* VFD Context (Modbus Hooks) */
typedef struct {
    uint16_t slave_address;
    int (*mb_write_single_reg)(uint16_t slave, uint16_t addr, uint16_t val);
    int (*mb_write_multi_reg)(uint16_t slave, uint16_t addr, uint16_t len, uint16_t* vals);
    int (*mb_read_single_reg)(uint16_t slave, uint16_t addr, uint16_t* val);
    int (*mb_read_multi_reg)(uint16_t slave, uint16_t addr, uint16_t len, uint16_t* buf);
} vfd_context_t;

/* VFD Config (Model-Specific) */
typedef struct {
    uint16_t monitoring_address;
    uint16_t freq_address;
    uint16_t status_address;
    uint16_t issue_address;
    uint16_t monitoring_data_len;
    uint16_t status_data_len;
    uint16_t issue_data_len;
} vfd_config_t;

/* VFD Instance (Holds Data) */
typedef struct {
    vfd_context_t context;
    vfd_config_t config;
    float running_frequency;    /* 0.01 Hz */
    float reference_frequency;  /* 0.01 Hz */
    float output_current;       /* 0.01 A */
    uint16_t output_voltage;    /* 1 V */
    float bus_line_voltage;     /* 0.1 V */
    vfd_status_t status;
    vfd_issue_t issue;
} vfd_t;

/* Default Config for Delixi VFD */
extern const vfd_config_t delixi_default_config;

/* Functions */
void vfd_init(vfd_t *vfd, vfd_context_t *ctx, vfd_config_t *cfg);
int vfd_update(vfd_t *vfd);
int vfd_set_running_freq(vfd_t *vfd, float speed /* 0.00% ~ 100.00% */);
float vfd_get_running_freq(vfd_t *vfd);
float vfd_get_reference_freq(vfd_t *vfd);
float vfd_get_output_current(vfd_t *vfd);
uint16_t vfd_get_output_voltage(vfd_t *vfd);
float vfd_get_bus_line_voltage(vfd_t *vfd);
vfd_status_t vfd_get_status(vfd_t *vfd);
vfd_issue_t vfd_get_issue(vfd_t *vfd);
const char* vfd_get_status_string(vfd_t *vfd);
const char* vfd_get_issue_string(vfd_t *vfd);

int set_running_freq(vfd_context_t*, float speed);
#endif /* INC_VFD_CONTROLLER_H_ */
