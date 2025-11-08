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

int set_running_freq(vfd_context_t*, float speed);
#endif /* INC_VFD_CONTROLLER_H_ */
