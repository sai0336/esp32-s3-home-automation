# Arduino setup and provisioning notes

This guide describes **the supplied firmware**, which was reportedly working in the author's installation. It is not a request to reflash the working home system.

## 1. Sketch and board

Open `firmware/home_automation/home_automation.ino` in Arduino IDE. Confirm the exact ESP32-S3 module variant and actual flash size. The project-plan export contains an Arduino compilation log with `esp32:esp32:esp32s3`, Espressif **Arduino-ESP32 2.0.7**, and **IRremote 4.7.1**. It also shows a RainMaker partition scheme. Treat these as **historical environment records**, not proof that another machine or the newest package will build unchanged.

The sketch uses RainMaker (`RMaker.h`), Wi-Fi provisioning (`WiFiProv.h`) and IRremote (`IRremote.hpp`). Arduino-ESP32 includes the RainMaker/Wi-Fi support for compatible builds; IRremote may need to be installed separately.

## 2. Public POP placeholder

The GitHub-ready sketch contains:

```cpp
const char *pop = "REPLACE_WITH_PRIVATE_POP";
```

Before using the sketch on a **test device**, set a distinct non-public proof-of-possession string in your local copy. Don't publish that change. The Serial Monitor provisioning instructions now print the value of the `pop` variable automatically, so the code and instructions stay consistent.

**This does not change the POP already flashed to the installed controller.** The placeholder is not a security upgrade to a running board and should never be treated as a good production secret.

## 3. Provisioning flow in this version

1. The sketch registers eight devices with ESP RainMaker and enables OTA, time-zone and scheduling services.
2. It calls `WiFiProv.beginProvision(WIFI_PROV_SCHEME_SOFTAP, WIFI_PROV_SCHEME_HANDLER_NONE, WIFI_PROV_SECURITY_1, pop, service_name)`.
3. The provisioning SoftAP service is named `PROV_SMARTHOME`.
4. The sketch prints SoftAP and POP instructions on Serial at **115200 baud**.
5. The author reported successfully connecting the controller to ESP RainMaker and later using Google Home. Google Home association is managed through the RainMaker/Google account integration, not by an API key in this file.

Do not hold the BOOT button for three seconds during normal operation: the sketch calls `RMakerFactoryReset(2)`, not merely a temporary Wi-Fi reconnect.

## 4. Validation sequence — only on a safely isolated test setup

- Confirm your board variant, compatible core and partition layout.
- Compile without changing anything except the local private POP, if necessary.
- Exercise one channel at a time with suitable **low-voltage test loads** before connecting any real appliance circuits.
- Verify local button debounce, cloud state reporting, fixed NEC IR commands, power-on relay state, and restart behavior.
- Check provisioning and Google Home account association separately; these require real hardware and accounts.
- Keep the existing working firmware backed up and do not reflash your in-service device until testing and an appropriate maintenance plan are complete.

## Compatibility warnings

This exact sketch depends on APIs and enum names available to the author's historic Arduino-ESP32 build. They may differ in newer Arduino-ESP32 releases. Follow Espressif's current RainMaker documentation if choosing to port to a newer core, but keep that port on a **separate branch or firmware file** rather than silently changing this preserved version.

Official RainMaker documentation: https://docs.espressif.com/projects/arduino-esp32/en/latest/api/rainmaker.html
