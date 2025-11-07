#ifndef _MMODBUS_CONFIG_H_
#define _MMODBUS_CONFIG_H_

#define _MMODBUS_RTU              1
#define _MMODBUS_ASCII            0 //  not implemented yet
#define _MMODBUS_USART            USART2
#define _MMODBUS_RXSIZE           64  
#define _MMODBUS_TXDMA            0
#if     _MMODBUS_TXDMA == 1
#define _MMODBUS_DMA              DMA1
#define _MMODBUS_DMASTREAM        LL_DMA_STREAM_5
#endif
#define _MMODBUS_CTRL_GPIO        USART2_DE_GPIO_Port
#define _MMODBUS_CTRL_PIN         USART2_DE_Pin


#if (_MMODBUS_RTU == 1) && (_MMODBUS_ASCII == 1)
#error please select _MMODBUS_RTU or _MMODBUS_ASCII
#endif
#endif
