# GPIO mapping and control behavior

**Source:** uploaded `home_automation.ino`. These are the values actually present in the sketch, **not a universal ESP32-S3 wiring recommendation**.

| Channel | RainMaker name | Active-low relay GPIO | Button GPIO | NEC IR command |
| :-- | :-- | --: | --: | :-- |
| 1 | Light 1 | 4 | 1 | `0x92` |
| 2 | Light 2 | 5 | 2 | `0x93` |
| 3 | Light 3 | 6 | 8 | `0xCC` |
| 4 | Light 4 | 7 | 9 | `0x8E` |
| 5 | Light 5 | 15 | 10 | `0x8F` |
| 6 | Light 6 | 16 | 11 | `0xC8` |
| 7 | Light 7 | 17 | 12 | `0x8A` |
| 8 | Light 8 | 18 | 13 | `0x8B` |

| Other function | GPIO / configuration |
| --- | --- |
| IR receiver input | GPIO14 |
| BOOT / factory reset button | GPIO0, `INPUT_PULLUP` |
| Relay ON | `LOW` |
| Relay OFF | `HIGH` |
| Physical button press | `LOW` with `INPUT_PULLUP` |
| Button debounce | 50 ms |
| Serial Monitor | 115200 baud |

## Hardware notes

- On the **ESP32-S3**, GPIO6–11 are GPIO-capable; the classic ESP32's flash-pin restriction should not be copied here. Availability still depends on the exact module and dev-board design.
- **GPIO0 is a strapping pin**; holding it in a particular state while restarting can change boot behavior. This sketch also uses it for the software factory-reset button.
- GPIO0, GPIO3, GPIO45 and GPIO46 are the chip's strapping pins; the GPIO map above is not a certification of the full board schematic.
- Verify the relay board input-voltage requirements, active-low behavior, power supply rating, and switching/load ratings independently. These tables describe **signal GPIOs only**, not household mains power wiring.
- IR command numbers are mapped to the author's remote. A different NEC remote can use different command codes.

**Official board reference:** https://docs.espressif.com/projects/esp-idf/en/v5.5/esp32s3/api-reference/peripherals/gpio.html
