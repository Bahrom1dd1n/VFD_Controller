# VFD Controller Library

Hardware-agnostic VFD (Variable Frequency Drive) controller via Modbus RTU.

## Features

- Read monitoring data (frequency, current, voltage, bus voltage)
- Set running frequency (0-100%)
- Status and fault detection with human-readable strings
- Bring your own Modbus implementation

## Quick Start

### 1. Add to your project
```bash
git clone https://github.com/bahrom1dd1n/vfd-controller.git
# Copy src/ and include/ to your project
```

### 2. Dependencies
- **Modbus Library**: [master-modbus](https://github.com/nimaltd/master-modbus)
- **Hardware**: RS485 interface

### 3. Configure VFD Register Map
Edit `include/vfd_config.h` for your VFD model (default: Delixi):
```c
#define VFD_MONITORING_ADDRESS 0x9000  // Adjust for your VFD
#define VFD_SET_FREQ_ADDRESS   0xA001
#define VFD_STATUS_ADDRESS     0xB000
#define VFD_ISSUE_ADDRESS      0xB001
```

### 4. Implement Modbus Callbacks
```c
#include "vfd_controller.h"
#include "mmodbus.h"  // Your Modbus library

vfd_issue my_write_reg(uint16_t slave, uint16_t addr, uint16_t val) {
    int result = mmodbus_writeHoldingRegister16i(slave, addr, val);
    return (result == 0) ? VFD_SUCCESS : VFD_ERROR_MODBUS_FAIL;
}

vfd_issue my_read_multi_reg(uint16_t slave, uint16_t addr, 
                             uint16_t count, uint16_t *buf) {
    int result = mmodbus_readHoldingRegisters16i(slave, addr, count, buf);
    return (result == 0) ? VFD_SUCCESS : VFD_ERROR_MODBUS_FAIL;
}
```

### 5. Initialize and Control
```c
vfd_context_t vfd = {
    .slave_address = 16,
    .mb_write_single_reg = my_write_reg,
    .mb_read_single_reg = my_read_reg,
    .mb_read_multi_reg = my_read_multi_reg
};

vfd_init(&vfd);

// Set frequency to 50%
vfd_set_running_freq(&vfd, 50.0);

// Main loop
while (1) {
    vfd_update(&vfd);  // Read all registers
    
    float freq = vfd_get_running_freq(&vfd);
    float current = vfd_get_output_current(&vfd);
    
    if (vfd_get_issue(&vfd) != VFD_NO_FAULT) {
        printf("Fault: %s\n", vfd_get_issue_string(&vfd));
    }
    
    delay(100);
}
```

## API Reference

### Initialization
- `void vfd_init(vfd_context_t *ctx)` - Initialize VFD context

### Control
- `int vfd_set_running_freq(vfd_context_t *ctx, float speed)` - Set frequency (0.00-100.00%)
- `int vfd_update(vfd_context_t *ctx)` - Read all VFD registers (call periodically)

### Getters
- `float vfd_get_running_freq(vfd_context_t *ctx)` - Current frequency (Hz × 0.01)
- `float vfd_get_reference_freq(vfd_context_t *ctx)` - Target frequency
- `float vfd_get_output_current(vfd_context_t *ctx)` - Output current (A × 0.01)
- `uint16_t vfd_get_output_voltage(vfd_context_t *ctx)` - Output voltage (V)
- `float vfd_get_bus_line_voltage(vfd_context_t *ctx)` - Bus voltage (V × 0.1)
- `vfd_status vfd_get_status(vfd_context_t *ctx)` - Status enum
- `vfd_issue vfd_get_issue(vfd_context_t *ctx)` - Fault code enum
- `const char* vfd_get_status_string(vfd_context_t *ctx)` - Status as string
- `const char* vfd_get_issue_string(vfd_context_t *ctx)` - Fault as string

## Modbus Configuration

**Typical Settings** (adjust for your VFD):
- Baud rate: 9600
- Parity: None
- Stop bits: 2
- Data bits: 8

**Register Map** (Delixi default):
| Address | Access | Description | Resolution |
|---------|--------|-------------|------------|
| 0x9000 | Read | Running frequency | 0.01 Hz |
| 0x9001 | Read | Reference frequency | 0.01 Hz |
| 0x9002 | Read | Output current | 0.01 A |
| 0x9003 | Read | Output voltage | 1 V |
| 0x9004 | Read | Bus voltage | 0.1 V |
| 0xA001 | Write | Set frequency | 0.01% (0-10000) |
| 0xB000 | Read | Status | Enum |
| 0xB001 | Read | Fault code | Enum |

## Error Codes

| Code | Description |
|------|-------------|
| `VFD_SUCCESS` | Operation successful |
| `VFD_ERROR_INVALID_PARAM` | Invalid parameter |
| `VFD_ERROR_MODBUS_FAIL` | Modbus communication failed |
| `VFD_ERROR_OUT_OF_RANGE` | Value out of range |

## Status Codes

- `VFD_NONE` - No status
- `VFD_FORWARD_RUN` - Running forward
- `VFD_REVERSE_RUN` - Running reverse
- `VFD_STOP` - Stopped

## Fault Codes

39 predefined fault codes including:
- Over-current/voltage conditions
- Motor overload/overheat
- Communication errors
- Hardware faults
- See `vfd_config.h` for complete list

## Example in main.c(In STM32CubeIde)

```c
// Initialize Modbus with 2.5ms timeout
mmodbus_init(2500);

// Setup VFD context with callbacks
vfd_context_t ctx = {
    .slave_address = 16,
    .mb_write_single_reg = my_mb_write_single_reg,
    .mb_read_multi_reg = my_mb_read_multi_reg
};

vfd_init(&ctx);

// Control loop: ramp frequency 0-99% every 500ms
while (1) {
    vfd_update(&ctx);  // Read VFD data
    
    if (HAL_GetTick() - last_time > 500) {
        vfd_set_running_freq(&ctx, freq);
        freq += step;
        if (freq >= 99) step = -1;
        if (freq < 5) step = 1;
        last_time = HAL_GetTick();
    }
}
```

## Porting to Other VFDs

1. Check your VFD's Modbus register map documentation
2. Update addresses in `vfd_config.h`
3. Adjust scaling factors in `vfd_controller.c` if needed
4. Update fault/status enums if different

## Hardware Setup

```
MCU (UART TX/RX) <---> RS485 Transceiver <---> VFD (A/B terminals)
                       (e.g., MAX485)
```

**Wiring**:
- RS485 A/B → VFD Modbus A/B terminals
- Ground → Ground

## License

MIT License - Use freely in commercial/personal projects.

## Contributing

1. Fork the repository
2. Create feature branch (`git checkout -b feature/new-vfd-support`)
3. Commit changes (`git commit -m 'Add support for XYZ VFD'`)
4. Push to branch (`git push origin feature/new-vfd-support`)
5. Open Pull Request

## Support

- **Issues**: [GitHub Issues](https://github.com/bahrom1dd1n/vfd-controller/issues)
- **Discussions**: [GitHub Discussions](https://github.com/bahrom1dd1n/vfd-controller/discussions)

## Credits

- Modbus library: [master-modbus by nimaltd](https://github.com/nimaltd/master-modbus)
