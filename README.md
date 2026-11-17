# ESP32-S3 Smart Home Automation

**An eight-channel home-automation controller using ESP RainMaker, Google Home integration, an IR remote and physical push buttons.**

The project was assembled and has been operated in a home by **Sai Chavan**, according to the author. This repository is a documented copy of the firmware supplied for review, **not a claim that its electrical installation or every hardware scenario has been independently tested**.

> **Important:** This is a public-sharing version of the author's working sketch. It retains the original control logic; the provisioning proof-of-possession (POP) is now a placeholder. **Do not flash the public copy before setting your own private POP and confirming your hardware.** Publishing this repository does not reprogram the already-installed controller.

## Project features

- Eight independently controllable, active-low relay outputs
- Eight corresponding physical push-button inputs with software debounce
- Eight NEC IR remote commands (specific command codes are in the sketch)
- Eight virtual switches exposed through **ESP RainMaker**
- **Google Home** control through the author's RainMaker account integration
- RainMaker Wi-Fi provisioning using **SoftAP** (not BLE in this sketch)
- RainMaker OTA, time-zone and schedule services enabled in firmware
- BOOT-button factory reset with an approximately three-second hold

**Not included in this version:** microphone/voice recognition on the ESP32, fan speed regulation or a local web dashboard. Google Home voice commands are provided by Google's ecosystem, not by an onboard microphone.

## Repository layout

```text
esp32-s3-home-automation/
├── firmware/
│   └── home_automation/
│       └── home_automation.ino     # Arduino IDE sketch; logic preserved
├── docs/
│   ├── GPIO_AND_CONTROLS.md        # GPIO map and IR assignments
│   ├── SETUP_AND_PROVISIONING.md   # Build and provisioning notes
│   ├── TECHNICAL_REVIEW.md         # Known behavior and limitations
│   └── SAFETY_AND_SECURITY.md      # Public code and mains safety notes
├── .gitignore
├── CHANGELOG.md
└── README.md
```

## Software and hardware context

- **Board family:** ESP32-S3 (exact dev-board model and flash configuration should be verified before flashing)
- **IDE:** Arduino IDE
- **Includes:** `Arduino.h`, `RMaker.h`, `WiFi.h`, `WiFiProv.h`, `IRremote.hpp`
- The author's **historical setup log** in the supplied project-plan PDF shows **Espressif Arduino-ESP32 core 2.0.7** and **IRremote 4.7.1**. Those are recorded working-context versions, not a current compatibility guarantee.
- **Serial Monitor:** 115200 baud
- **Partition scheme:** a RainMaker-compatible partition scheme sized for the board's flash

Review [setup and provisioning](docs/SETUP_AND_PROVISIONING.md) and [the original GPIO mapping](docs/GPIO_AND_CONTROLS.md) before building.

## How to open the firmware

1. Open `firmware/home_automation/home_automation.ino` with the Arduino IDE. The folder and file names match Arduino's sketch convention.
2. Select the correct ESP32-S3 board variant, port and compatible RainMaker partition scheme.
3. Install/select the Arduino-ESP32 board package containing the RainMaker APIs and the IRremote library.
4. Replace `REPLACE_WITH_PRIVATE_POP` in your **local** copy with a unique POP that is not committed to GitHub.
5. Check GPIOs, relay signal polarity, and input wiring using low-voltage test loads. **Do not interrupt or reflash a live household controller without an appropriate maintenance plan.**
6. Compile and test on a separate development board before considering a firmware update to the installed system.

The code calls `WiFiProv.beginProvision(...)` using SoftAP. The on-screen setup instructions are printed to the Serial Monitor. A 3-second hold on the BOOT button calls `RMakerFactoryReset(2)`; avoid this on an installed controller unless you intend to remove its provisioning state.

## Expected device assignments

The firmware names all eight RainMaker devices **Light 1** through **Light 8**. Room labels and groupings may be managed separately in Google Home. Hardware mapping is [documented here](docs/GPIO_AND_CONTROLS.md).

## Known operational caveats

- IR receiver processing begins after Wi-Fi becomes connected, and IR polling is paused whenever Wi-Fi is disconnected.
- Local button inputs are checked in the main loop; the reset handler can block the loop for about three seconds during a hold.
- Relays use active-low output logic. Exact relay module electrical specifications and switching behavior are **not** verified by this source.
- Startup output initialization, fixed IR command numbers, old core compatibility, and GPIO exposure on the particular ESP32-S3 board warrant separate testing before any deployment changes.

Details: [technical review](docs/TECHNICAL_REVIEW.md).

## Project author and links

**Sai Chavan** — Software & Embedded Systems Developer

- GitHub: https://github.com/sai0336
- LinkedIn: https://www.linkedin.com/in/sai-santosh-chavan/

## Publishing and licensing

This copy is **prepared for GitHub but has not been uploaded** as part of this review. No open-source license has been selected or added. You can choose licensing terms before publishing; third-party dependencies have their own licenses. Do not publish personal Wi-Fi credentials, real POP codes, cloud identifiers, private wiring photos or recovery details.

Project and documentation credits: Sai Chavan, 2026.
