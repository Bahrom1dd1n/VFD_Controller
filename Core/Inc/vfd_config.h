/*
 * vfd_config.h
 *
 *  Created on: Nov 10, 2025
 *      Author: bahromiddin
 */

#ifndef INC_VFD_CONFIG_H_
#define INC_VFD_CONFIG_H_


typedef struct {
    uint16_t monitoring_address;
    uint16_t freq_address;
    uint16_t status_address;
    uint16_t issue_address;
    uint16_t monitoring_data_len;
    uint16_t status_data_len;
    uint16_t issue_data_len;
} vfd_config_t;

typedef enum {
    VFD_NONE = 0,
    VFD_FORWARD_RUN,
    VFD_REVERSE_RUN,
    VFD_STOP,
    VFD_STATUS_COUNT
} vfd_status_t;


#endif /* INC_VFD_CONFIG_H_ */
