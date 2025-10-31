# USBIPD Manual for Windows 11 and WSL2

## Overview

USBIPD-WIN allows you to share USB devices from Windows with WSL2 (Windows Subsystem for Linux). This is essential for using hardware programmers like AVR ISP mkII, JTAG ICE mkII, and STK500 with Linux-based tools running in WSL2.

## Prerequisites

- Windows 11 (or Windows 10 with recent updates)
- WSL2 installed and configured
- Administrator access on Windows

## Part 1: Installing USBIPD on Windows

### Method 1: Using winget (Recommended)

Open PowerShell or Command Prompt:
```powershell
winget install --interactive --exact dorssel.usbipd-win
```

### Method 2: Using MSI Installer

1. Download the latest release from: https://github.com/dorssel/usbipd-win/releases
2. Download the `.msi` file (e.g., `usbipd-win_x.x.x.msi`)
3. Run the installer
4. Follow the installation wizard
5. Restart your computer after installation

### Verify Installation

Open PowerShell and run:
```powershell
usbipd --version
```

You should see the version number (e.g., `4.0.0`).

## Part 2: Installing USB/IP Tools in WSL2

Open your WSL2 terminal and install the required tools:

### For Ubuntu/Debian:
```bash
sudo apt update
sudo apt install linux-tools-generic hwdata
sudo update-alternatives --install /usr/local/bin/usbip usbip /usr/lib/linux-tools/*-generic/usbip 20
```

### For other distributions:

Check your distribution's package manager for `usbip` or `linux-tools`.

## Part 3: Sharing USB Devices

### Step 1: List Available USB Devices (Windows)

Open PowerShell **as Administrator**:
```powershell
usbipd list
```

Output example:
```
BUSID  VID:PID    DEVICE                                            STATE
1-4    03eb:2104  Atmel Corp. AVR ISP mkII                         Not shared
2-3    0403:6001  FTDI FT232 USB-UART                              Not shared
3-1    1a86:7523  USB Serial Port (COM3)                           Not shared
```

Note the `BUSID` of the device you want to use (e.g., `1-4` for AVR ISP mkII).

### Step 2: Bind the USB Device (One-time setup)

Still in PowerShell **as Administrator**:
```powershell
usbipd bind --busid 1-4
```

Replace `1-4` with your device's BUSID.

This only needs to be done once per device. The binding persists across reboots.

### Step 3: Attach Device to WSL2

**Every time you want to use the device in WSL2**, run in PowerShell **as Administrator**:
```powershell
usbipd attach --wsl --busid 1-4
```

For automatic re-attachment after disconnect:
```powershell
usbipd attach --wsl --busid 1-4 --auto-attach
```

### Step 4: Verify Device in WSL2

In your WSL2 terminal:
```bash
lsusb
```

You should see your device listed, e.g.:
```
Bus 001 Device 002: ID 03eb:2104 Atmel Corp. AVR ISP mkII
```

### Step 5: Check Device Path

For serial devices (STK500, FTDI cables):
```bash
ls /dev/ttyUSB*
# or
ls /dev/ttyACM*
```

For USB programmers (AVR ISP mkII, JTAG ICE mkII):
```bash
lsusb -v | grep -i "atmel\|avr\|jtag"
```

## Part 4: Detaching Devices

When done, detach the device (makes it available to Windows again):

In PowerShell **as Administrator**:
```powershell
usbipd detach --busid 1-4
```

Or detach all devices:
```powershell
usbipd detach --all
```

## Part 5: Common Workflows

### Workflow 1: Using AVR ISP mkII

**Windows PowerShell (as Admin):**
```powershell
# One-time bind (if not done already)
usbipd bind --busid 1-4

# Attach to WSL2
usbipd attach --wsl --busid 1-4
```

**WSL2 Terminal:**
```bash
# Verify device
lsusb | grep Atmel

# Use with avr-prog script
./avr-prog avrisp program m328p firmware.hex

# When done, go back to PowerShell and detach
```

**Windows PowerShell:**
```powershell
usbipd detach --busid 1-4
```

### Workflow 2: Using STK500 (Serial Device)

**Windows PowerShell (as Admin):**
```powershell
# Bind (one-time)
usbipd bind --busid 2-3

# Attach
usbipd attach --wsl --busid 2-3
```

**WSL2 Terminal:**
```bash
# Find serial port
ls /dev/ttyUSB*
# Should show /dev/ttyUSB0 or similar

# Update avr-prog script with correct port
# Edit: STK500_PORT="/dev/ttyUSB0"

# Program
./avr-prog stk500 program m328p firmware.hex
```

### Workflow 3: Multiple Devices

You can attach multiple devices simultaneously:

**Windows PowerShell (as Admin):**
```powershell
# Attach AVR ISP mkII
usbipd attach --wsl --busid 1-4

# Attach FTDI serial cable
usbipd attach --wsl --busid 2-3

# Attach logic analyzer
usbipd attach --wsl --busid 3-1
```

## Part 6: Troubleshooting

### Issue: "Access denied" or "Administrator privileges required"

**Solution:** Always run PowerShell as Administrator for usbipd commands.

Right-click PowerShell → "Run as Administrator"

### Issue: Device not showing in WSL2 after attach

**Solution 1:** Check if usbip tools are installed in WSL2:
```bash
which usbip
```

If not found, install as per Part 2.

**Solution 2:** Restart WSL2:
```powershell
wsl --shutdown
wsl
```

Then re-attach the device.

### Issue: Device keeps disconnecting

**Solution 1:** Use `--auto-attach` flag:
```powershell
usbipd attach --wsl --busid 1-4 --auto-attach
```

**Solution 2:** Disable USB selective suspend in Windows:
- Control Panel → Power Options
- Change plan settings → Advanced
- USB settings → USB selective suspend → Disabled

**Solution 3:** Check USB cable quality - bad cables cause disconnects.

### Issue: "Error: no compatible device found"

**Solution:** Device might need specific drivers in WSL2. Check dmesg:
```bash
dmesg | tail -20
```

Look for errors related to your device.

### Issue: Permission denied when accessing /dev/ttyUSB0

**Solution:** Add your user to dialout group:
```bash
sudo usermod -a -G dialout $USER
```

Log out and log back into WSL2.

### Issue: BUSID changes after reboot

**Solution:** BUSID can change if you plug/unplug devices or reboot. Always check with `usbipd list` before attaching.

### Issue: "The device is already attached to another client"

**Solution:** Detach first:
```powershell
usbipd detach --busid 1-4
```

Then re-attach.

## Part 7: Persistent Setup (Advanced)

### Create Helper Scripts

**Windows side - `attach-avr.ps1`:**
```powershell
# Run as Administrator
$BUSID = "1-4"  # Change to your AVR ISP mkII bus ID

Write-Host "Attaching AVR ISP mkII to WSL2..."
usbipd attach --wsl --busid $BUSID --auto-attach

if ($?) {
    Write-Host "Success! Device attached."
} else {
    Write-Host "Failed to attach device. Check if it's plugged in."
}
```

**WSL2 side - `~/.bashrc` alias:**
```bash
# Add to ~/.bashrc
alias attach-avr='powershell.exe -Command "usbipd attach --wsl --busid 1-4 --auto-attach"'
alias detach-avr='powershell.exe -Command "usbipd detach --busid 1-4"'
alias list-usb='powershell.exe -Command "usbipd list"'
```

Reload bashrc:
```bash
source ~/.bashrc
```

Usage:
```bash
list-usb        # See available devices
attach-avr      # Attach AVR programmer
detach-avr      # Detach when done
```

### Auto-attach on WSL2 Startup

Create a startup script in WSL2:

**`~/bin/usb-startup.sh`:**
```bash
#!/bin/bash
# Auto-attach USB devices on WSL2 startup

echo "Waiting for USB devices..."
sleep 2

# Check if AVR ISP mkII is attached
if ! lsusb | grep -q "03eb:2104"; then
    echo "Attaching AVR ISP mkII..."
    powershell.exe -Command "usbipd attach --wsl --busid 1-4" > /dev/null 2>&1
fi

echo "USB setup complete."
```

Make executable:
```bash
chmod +x ~/bin/usb-startup.sh
```

Add to `~/.bashrc`:
```bash
# Auto-attach USB on terminal start
~/bin/usb-startup.sh
```

## Part 8: Quick Reference

### Essential Commands

**Windows (PowerShell as Admin):**
```powershell
# List devices
usbipd list

# Bind device (one-time)
usbipd bind --busid <BUSID>

# Attach to WSL2
usbipd attach --wsl --busid <BUSID>

# Attach with auto-reconnect
usbipd attach --wsl --busid <BUSID> --auto-attach

# Detach device
usbipd detach --busid <BUSID>

# Detach all
usbipd detach --all
```

**WSL2:**
```bash
# List USB devices
lsusb

# List serial ports
ls /dev/ttyUSB* /dev/ttyACM*

# Check USB device info
lsusb -v

# Check dmesg for USB events
dmesg | grep usb
```

## Part 9: Device-Specific Notes

### AVR ISP mkII (VID:PID 03eb:2104)

- Uses libusb, no serial port needed
- Access via `usb` in avrdude: `-P usb`
- No special permissions needed if properly attached

### JTAG ICE mkII (VID:PID 03eb:2103)

- Same as AVR ISP mkII
- Uses libusb
- Access via: `-P usb`

### STK500 (USB-to-Serial adapter)

- Creates `/dev/ttyUSB0` or `/dev/ttyACM0`
- Requires dialout group membership
- Access via: `-P /dev/ttyUSB0`

### FTDI Cables (VID:PID 0403:6001)

- Creates `/dev/ttyUSB0`
- Requires dialout group
- Common for serial communication

## Part 10: Alternative - Native Windows Tools

If USBIPD is problematic, consider:

1. **Use Windows-native avrdude:**
   - Download avrdude for Windows
   - Run in PowerShell/CMD instead of WSL2
   - Devices work directly without USB/IP

2. **Use Atmel Studio (Windows only):**
   - Full IDE with integrated programmer support
   - No WSL2 needed

3. **Dual boot Linux:**
   - Native USB support
   - No complexity of USB/IP
   - Best performance

## Resources

- USBIPD-WIN GitHub: https://github.com/dorssel/usbipd-win
- WSL2 Documentation: https://docs.microsoft.com/en-us/windows/wsl/
- USB/IP Protocol: http://usbip.sourceforge.net/

## Summary

**Initial Setup (once):**
1. Install usbipd-win on Windows
2. Install usbip tools in WSL2
3. Bind devices you want to share

**Every Session:**
1. Open PowerShell as Admin
2. Run `usbipd attach --wsl --busid X-X`
3. Use device in WSL2
4. Detach when done

**Tips:**
- Use `--auto-attach` for automatic reconnection
- Create helper scripts for frequent devices
- Always check `usbipd list` if BUSID changes

---

**With USBIPD properly configured, you can seamlessly use all your AVR programmers from WSL2!**