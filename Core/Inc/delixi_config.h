/*
 * delixi_config.h
 *
 *  Created on: Nov 10, 2025
 *      Author: bahromiddin
 */

#ifndef INC_DELIXI_CONFIG_H_
#define INC_DELIXI_CONFIG_H_
#include "vfd_config.h"

/* VFD Config (Model-Specific) */
/* Default Delixi Config */
const vfd_config_t delixi_config = {
    .monitoring_address = 0x9000,
    .freq_address = 0xA001,
    .status_address = 0xB000,
    .issue_address = 0xB001,
    .monitoring_data_len = 5,
    .status_data_len = 1,
    .issue_data_len = 1
};



#endif /* INC_DELIXI_CONFIG_H_ */
