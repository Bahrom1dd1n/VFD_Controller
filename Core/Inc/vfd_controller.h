/*
 * vfd_controller.h
 *
 *  Created on: Nov 7, 2025
 *      Author: bahromiddin
 */

#ifndef INC_VFD_CONTROLLER_H_
#define INC_VFD_CONTROLLER_H_
#include <stdint.h>
#include "vfd_config.h"
/* VFD Context (Modbus Hooks) */
typedef struct {
	uint16_t slave_address;
	vfd_issue (*mb_write_single_reg)(uint16_t, uint16_t, uint16_t);
	vfd_issue (*mb_write_multi_reg)(uint16_t, uint16_t, uint16_t, uint16_t*);
	vfd_issue (*mb_read_single_reg)(uint16_t, uint16_t,uint16_t*);
	vfd_issue (*mb_read_multi_reg)(uint16_t, uint16_t, uint16_t, uint16_t*);
	float running_frequency; /* 0.01 Hz */
	float reference_frequency; /* 0.01 Hz */
	float output_current; /* 0.01 A */
	uint16_t output_voltage; /* 1 V */
	float bus_line_voltage; /* 0.1 V */
	vfd_status status;
	vfd_issue issue;

} vfd_context_t;


/* Functions */
void vfd_init(vfd_context_t *vfd);
int vfd_update(vfd_context_t *vfd);
int vfd_set_running_freq(vfd_context_t *vfd, float speed /* 0.00% ~ 100.00% */);
float vfd_get_running_freq(vfd_context_t *vfd);
float vfd_get_reference_freq(vfd_context_t *vfd);
float vfd_get_output_current(vfd_context_t *vfd);
uint16_t vfd_get_output_voltage(vfd_context_t *vfd);
float vfd_get_bus_line_voltage(vfd_context_t *vfd);

vfd_status vfd_get_status(vfd_context_t *vfd);
vfd_issue vfd_get_issue(vfd_context_t *vfd);
const char* vfd_get_status_string(vfd_context_t *vfd);
const char* vfd_get_issue_string(vfd_context_t *vfd);

#endif /* INC_VFD_CONTROLLER_H_ */
