# Packet Sniffer

Simple C examples using [Npcap](https://npcap.com) to list capture devices and open one for live capture.

- `list_device.c` — lists every capture interface Npcap can see.
- `main.c` — opens a specific device for capture. The device GUID is machine-specific, so run `list_device` first and edit `main.c` to match your NIC.

## Prerequisites (Windows)

1. **Install the Npcap runtime installer** (the `npcap-*.exe` download, *not* the SDK zip).
   - The SDK zip only contains headers and `.lib` files for compiling. Without the installer there is no `wpcap.dll`, and the `.exe` will fail to start (exit code 127 / "DLL not found").
   - At install time, tick "Install Npcap in WinPcap API-compatible Mode" if you want the classic WinPcap API.
2. **Download the Npcap SDK zip** and extract it to `C:\npcap-sdk-1.16`.
3. **A MinGW gcc** — e.g. MSYS2 UCRT64 (`C:\msys64\ucrt64\bin\gcc.exe`).

## Building `list_device`

```sh
gcc list_device.c -o list_device.exe \
    -IC:/npcap-sdk-1.16/Include \
    -LC:/npcap-sdk-1.16/Lib/x64 \
    -lwpcap
```

Important details:

- Use `Lib/x64`, **not** `Lib/` — the top-level `wpcap.lib`/`Packet.lib` are 32-bit and the 64-bit linker will skip them ("skipping incompatible ... cannot find -lwpcap").
- If you are on MSYS2 and gcc fails silently (compiles nothing, no error), prepend the UCRT64 bin directory to `PATH` so gcc loads the right DLLs:
  ```sh
  export PATH="/c/msys64/ucrt64/bin:$PATH"
  ```

## Running

- `list_device.exe` must be run from an **Administrator** prompt, or it will find no interfaces.
- It prints a numbered list of devices. For `main.c`, copy the GUID from the `\Device\NPF_{...}` name of your physical NIC (e.g. Wi-Fi or Ethernet) into the `device` variable.
- On Wi-Fi, Npcap by default only captures traffic to/from your machine; monitor mode is required for full promiscuous capture on wireless.
