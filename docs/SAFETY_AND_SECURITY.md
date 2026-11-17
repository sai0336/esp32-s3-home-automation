# Safety, privacy, and public-repository rules

## Household electricity

This code may control mains-powered loads through relay modules. **Mains wiring can cause lethal electric shock and fire**. This repository is **not** an installation manual or electrical safety certification. Electrical installation, relay/contact selection, earthing, isolation, enclosure, strain relief and protective devices must be assessed by a qualified electrician under applicable local requirements. Avoid working on a live system. An already-running system isn't automatically proven safe.

A signal GPIO table is included in `GPIO_AND_CONTROLS.md`; no instructions for connecting 230 V live/neutral wires are provided. Do not reproduce unverified advice from the project-plan conversation about tapping stuck relays, installing generic inrush thermistors, or improvised parallel mains bypasses.

## Software security

- The example `pop` is **`REPLACE_WITH_PRIVATE_POP`**. It is a placeholder, **not a secure provisioning secret**.
- For a new device, set a unique private POP *locally*. Never commit it to GitHub or post it publicly.
- The original sketch was configured with a common, hardcoded POP. Editing a GitHub copy does not change what is already installed on the running board. Evaluate changes to the installed POP only with a deliberate service/reprovisioning plan.
- This firmware prints the currently configured POP to Serial during setup. Treat Serial logs and screenshots as potentially sensitive.
- Do not add Wi-Fi credentials, access tokens, cloud account IDs, device certificates, reset QR codes or other private identifiers to the repo.
- A long BOOT-button hold executes a factory reset. Document this for everyone with physical access.
- Google Home control requires the relevant cloud/account authorization outside of this source code.

## Scope of this public release

This is a **source publication candidate**, not a firmware update, production safety validation or independent cybersecurity audit. Nothing in this ZIP needs to be flashed onto the installed home controller.
