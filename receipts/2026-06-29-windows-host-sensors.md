# Receipt — WiFi, Bluetooth, battery, memory streaming live into the mesh, native on Windows (2026-06-29)

**What happened:** the Windows-native `fkwu` now reads four more host world-sensors and streams them into the mesh
— **WiFi SSID + signal**, **Bluetooth radio + paired/near count**, **battery**, **memory load**. Same pattern as
the camera/mic carriers: afferent reads through the host's own OS API (`host-kernel.form` world-sensors VIA-HOST,
allowed), each degrading to an honest sentinel if the API is absent.

## Witnessed native on Windows 11 (`sense_sensors`, tag 222)

```
host-sensors  (Windows: wlanapi + bthprops + kernel32)
  wifi    where    ssid=IASVMS  signal=100
  bt      who      radio=1  paired/near=2
  power   vitality battery=255
  memory  vitality load=66
live sensors: 4
```

Real reads on this machine: the network `IASVMS` at full signal (wlanapi), a Bluetooth radio with 2 paired/near
devices (bthprops), 66% memory load (kernel32 `GlobalMemoryStatusEx`). `battery=255` is the honest `GetSystemPower
Status` value for *no battery present* (a desktop on AC) — the read succeeded; the value names the truth.

## Carriers + primitives

`runtime/fkwu-uni.c`, all `#if defined(_WIN32)` with non-Windows stubs:
- `sense_wifi_ssid` (216) / `sense_wifi_signal` (217) — `wlanapi` `WlanOpenHandle`/`WlanEnumInterfaces`/
  `WlanQueryInterface(current_connection)`; SSID + signal read by offset from `WLAN_CONNECTION_ATTRIBUTES`.
- `sense_bt_present` (218) / `sense_bt_count` (219) — `bthprops` `BluetoothFindFirstRadio` / `BluetoothFindFirst/
  NextDevice` (remembered+authenticated+connected, no live inquiry — fast, non-intrusive).
- `sense_power` (220) — `GetSystemPowerStatus`; `sense_mem` (221) — `GlobalMemoryStatusEx` (renamed C fn
  `fk_memload` to avoid the kernel's `fk_mem[]` array).
- `sense_sensors` (222) — the live report. All seven added to `flt-ops` (Form-callable). Build:
  `gcc -O2 -o fkwu.exe runtime/fkwu-uni.c -lws2_32 -lwinmm -lavicap32 -luser32 -lwlanapi -lbthprops`.

## Streamed into the mesh (`observe/host-sensors-mesh.fk`)

The honest, cross-witnessing mapping:
- **WiFi SSID → WHERE** — the network name is a place signature.
- **Bluetooth set → WHERE** — the set of nearby devices is *also* a place signature. So wifi and bt are **two
  independent witnesses of the same place**: when they agree, place confidence is high with **no oracle spent**
  (cross-witness-economy). `hsm-fused-where` fuses them on the BANDED WHERE plane → consensus place, summed trust,
  `agree=2`.
- **battery + memory → VITALITY** — the cell's own health, the confidence floor it carries (load < 80% + no-battery
  desktop = full vitality).

The SSID / bt-set content-address to a place band via `room-register` in production; here the witnessed session is
band 1 ("here"), named not faked.

## Honest floor

- **Witnessed (Windows):** all four sensors read live and reported; the SSID (`IASVMS`), bt radio + 2 devices, mem
  66% are real. The carriers link and run.
- **Mesh cell** composes the four-way organs (`mesh-join`, `mesh-sense-7w`) and encodes the witnessed invariants;
  running it four-way on Windows awaits the **seed** (`windows-flatten-reground`), like the other body cells.
- **WiFi signal offset** (`+576`) is read by byte-offset from the connection struct; SSID (`+520/+524`) is solid
  (the witnessed `IASVMS` confirms it). If a future Windows changes the layout, the offsets are the thing to re-check.
- **Bluetooth** uses remembered/connected devices (no live inquiry) — fast and non-intrusive; a live scan is a
  louder, opt-in rung not taken here.

## Reproduce

```
gcc -O2 -o fkwu.exe runtime/fkwu-uni.c -lws2_32 -lwinmm -lavicap32 -luser32 -lwlanapi -lbthprops
printf '1 0 1 222 0 0 0\n' > sensors.flat && ./fkwu.exe sensors.flat   # wifi / bt / power / memory, live
```
