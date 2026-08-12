# Packet Sniffer

A minimal command-line packet sniffer written in C using [Npcap](https://npcap.com) (libpcap/WinPcap API). It lists available capture devices and performs live captures, decoding Ethernet, IP, TCP, UDP, and ICMP headers.

## Features

- **Live capture** on a specific interface with an optional BPF filter (`tcp or udp`).
- **Header decoding** for each captured packet:
  - Ethernet (source/destination MAC, EtherType)
  - IP (version, header length, source/destination address)
  - TCP / UDP (source/destination ports)
  - ICMP
- Prints each packet as `[size] src_ip -> dst_ip  PROTO  sport -> dport`.

## Files

| File | Description |
|------|-------------|
| `list_device.c` | Lists every capture interface Npcap can see. Run this first to find your NIC's GUID. |
| `main.c` | Opens a specific device, compiles a `tcp or udp` BPF filter, and captures/decodes packets until `Ctrl+C`. |

## Prerequisites (Windows)

1. **Install the Npcap runtime installer** (the `npcap-*.exe` download, *not* the SDK zip).
   - The SDK zip only contains headers and `.lib` files for compiling. Without the installer there is no `wpcap.dll`, and the `.exe` will fail to start (exit code 127 / "DLL not found").
   - At install time, tick "Install Npcap in WinPcap API-compatible Mode" if you want the classic WinPcap API.
2. **Download the Npcap SDK zip** and extract it to `C:\npcap-sdk-1.16`.
3. **A MinGW gcc** — e.g. MSYS2 UCRT64 (`C:\msys64\ucrt64\bin\gcc.exe`).

## Building

Build `list_device`:

```sh
gcc list_device.c -o list_device.exe \
    -IC:/npcap-sdk-1.16/Include \
    -LC:/npcap-sdk-1.16/Lib/x64 \
    -lwpcap
```

Build `main`:

```sh
gcc main.c -o main.exe \
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

1. Run `list_device.exe` from an **Administrator** prompt — otherwise it finds no interfaces.
2. It prints a numbered list of devices. Copy the GUID from the `\Device\NPF_{...}` name of your physical NIC (e.g. Wi-Fi or Ethernet) into the `device` variable in `main.c` (the GUID is machine-specific).
3. Run `main.exe` as Administrator. You should see decoded packets printed until you press `Ctrl+C`.

Example output:

```
Listening on device.. Press Ctrl+C to exit

[52 bytes] 192.168.1.10 -> 142.250.72.14  UDP  52428 -> 443
[64 bytes] 192.168.1.1 -> 192.168.1.10   TCP  443 -> 52428
```

## Notes

- On Wi-Fi, Npcap by default only captures traffic to/from your machine; monitor mode is required for full promiscuous capture on wireless.
- Filters can be tweaked by editing `filter_exp` in `main.c` (e.g. `"port 80"`, `"host 192.168.1.1"`).
