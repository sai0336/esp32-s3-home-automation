# Technical review — preserved firmware

## What has been established

- The author reports that the ESP32-S3 controller has operated in their home for approximately two to three months with ESP RainMaker, Google Home, IR remote and push-button control.
- The source file contains eight relay pins, eight button inputs, an NEC IR decoder, RainMaker switch callbacks, SoftAP Wi-Fi provisioning, OTA/time-zone/schedule initialization and factory reset logic.
- The repository copy changes only the public POP placeholder and the Serial Monitor instruction line; **device-control logic is preserved**.
- No Wi-Fi password, RainMaker API token or Google account secret appears in the uploaded `.ino` file.

**Not independently verified:** compilation against installed libraries, runtime behavior on the user's actual board, Google Home cloud link, all relay channel responses, field installation safety, mains appliance load ratings or physical state of the installed enclosure.

## Existing behavior and limitations (not patched)

| Source location | Observation | Why it matters |
| --- | --- | --- |
| `loop()` | IR initializes only once when Wi-Fi connects, and is polled only while connected | IR commands won't be processed during Wi-Fi outages |
| `checkResetButton()` | A 30 × 100-ms blocking loop runs during a reset hold | Ordinary button processing and IR polling pause for up to about 3 seconds |
| `checkResetButton()` | A 3-second GPIO0 hold calls `RMakerFactoryReset(2)` | This is a factory reset, not just a Wi-Fi restart |
| `setup()` | `pinMode(..., OUTPUT)` precedes `digitalWrite(..., RELAY_OFF)` | Relay startup transitions should be tested for unwanted pulses |
| `toggleDevice()` | Reports state through `mySwitches[index]->updateAndReportParam(...)` | Cloud state synchronization during outages/reconnects is not independently verified |
| `updateIR()` | Uses fixed NEC command codes; ignores repeat frames | Different remotes or press patterns may behave differently |
| GPIO constants | Specific pins are baked into arrays | Verify the exact ESP32-S3 board pinout before changing hardware |
| `deviceNames[]` | Light 1 through Light 8 | Google Home room names/groups may differ from the firmware's fixed RainMaker device names |
| `RMaker.enableOTA(...)` | RainMaker OTA service enabled | OTA availability and behavior depends on provisioning/cloud and compatible runtime |

## Reasons not to "fix" the firmware blindly

The running installation reportedly works. Even minor changes to task scheduling, pin initialization, partition scheme or provisioning APIs could introduce regressions or reset a deployed controller. Improvements should be evaluated on separate hardware, with versioned backups and a rollback plan. The ZIP here is for code documentation and safe publication only.

## Historical context from supplied plan

The author's 57-page planning PDF records multiple RainMaker provisioning failures and later successful connection. It contains past wiring concerns, such as relay contacts sticking. Those historic observations **do not establish that those faults persist in the current installation**, nor do months of operation prove an electrical inspection was completed.
