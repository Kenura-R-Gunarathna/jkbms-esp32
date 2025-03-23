# JKBMS - Standord

### Request

```
4E 57 0 13 0 0 0 0 6 3 0 0 0 0 0 0 68 0 0 1 29
```

### Response

```
4E 57 01 06 00 00 00 00 06 00 01 79 15 01 0E D2 02 0E D2 03 0E D2 04 0E D2 05 0E DB 06 0E DB 07 0E DB 80 00 1C 81 00 1C 82 00 1C 83 0A 62 84 00 00 85 5A 86 02 87 00 00 89 00 00 00 05 8A 00 07 8B 00 00 8C 00 03 8E 0B 7C 8F 07 B6 90 10 68 91 10 54 92 00 03 
93 0B 04 94 0B 22 95 00 03 96 01 2C 97 00 C8 98 01 2C 99 00 19 9A 00 1E 9B 0B B8 9C 00 0A 9D 01 9E 00 64 9F 00 50 A0 00 46 A1 00 3C A2 00 14 A3 00 46 A4 00 46 A5 FF EC A6 FF F6 A7 FF EC A8 FF F6 A9 07 AA 00 00 00 75 AB 01 AC 01 AD 03 CC AE 01 AF 01 B0 00 0A B1 14 B2 31 32 33 34 35 36 00 00 00 00 B3 00 B4 49 6E 70 75 74 20 55 73 B5 32 34 30 37 B6 00 00 08 19 B7 31 31 2E 58 41 5F 53 31 31 2E 35 31 5F 5F 5F B8 00 B9 00 00 00 75 BA 49 6E 70 75 74 20 55 73 65 72 64 61 4A 4B 5F 42 31 41 38 53 32 30 50 00 C0 01 00 00 00 00 68 00 00 4D 89
```

### Frame Format

A frame is the basic unit for transmitting information. It includes the start character, length, command word, transmission type, information field, end mark, and checksum. The following table describes the frame format used for communication. 

| Frame Unit              | Length | Remark                                                                 |
|-------------------------|--------|-------------------------------------------------------------------------|
| **STX**                 | 2      | Field is two bytes. The first byte is `0x4E` and the second byte is `0x57`. |
| **LENGTH**              | 2      | Two bytes, including all data bytes excluding the first two characters, including the checksum and length field itself. |
| **BMS Terminal Number** | 4      | 4-byte ID. The highest 8 bits are the management standby number, and the lower 24 bits are the terminal number. |
| **Command**             | 1      | Refer to the command word description. [Read More](#command-word-description) |
| **Word Frame Source**   | 1      | 1 byte. Relative to the sender and receiver: 0 = BMS, 1 = Bluetooth, 2 = GPS, 3 = PC host computer. |
| **Transport Type**      | 1      | 1 byte: 0 = request frame, 1 = response frame, 2 = active report. |
| **Frame Information Element** | N | Information field for BMS setting data identification code. |
| **Record Number**       | 4      | The upper 1 byte is a random code (reserved for encryption), and the lower 3 bytes are the record number/code. |
| **End Mark**            | 1      | One byte `0x68`. |
| **Checksum**            | 4      | The checksum is calculated as the cumulative sum of all bytes from the start character (`STX`) to the end mark (`0x68`). |

### Command Word Description 

One byte that defines the transmission function of this frame.

| Command Code | Command Item          | Remark                                                                 |
|--------------|-----------------------|-------------------------------------------------------------------------|
| `0x01`       | Activate Command      | Used to wake up the BMS from sleep mode.                               |
| `0x02`       | Write Instruction     | Configures BMS parameters.                                             |
| `0x03`       | Read Instruction      | Reads BMS identification code data.                                    |
| `0x05`       | Password Instruction  | Allows parameter modification only after the correct password is sent. |
| `0x06`       | Read All Data         | Reads all data from the identification code table in one operation.    |

### BMS Setting Data Identification Code

| R/W | Code   | Description                        | Data Type | Remark                                                                 |
|-----|--------|------------------------------------|-----------|-------------------------------------------------------------------------|
| R   | `0x79` | Single Battery Voltage            | 3*n HEX   | Voltage of each individual battery cell.                               |
| R   | `0x80` | Power Tube Temperature            | 2 HEX     | Temperature of the power tube, crucial for monitoring thermal performance. |
| R   | `0x81` | Battery Box Internal Temp         | 2 HEX     | Accurate measurement of internal temperature.                         |
| R   | `0x82` | Battery Temperature               | 2 HEX     | Accurate measurement of battery temperature.                          |
| R   | `0x83` | Total Battery Voltage             | 2 HEX     | Minimum unit 10mV.                                                    |
| R   | `0x84` | Current Data                      | 2 HEX     | Accuracy 10mA.                                                        |
| R   | `0x85` | Battery Remaining Capacity        | 1 HEX     | SOC, 0-100%.                                                          |
| R   | `0x86` | Number of Battery Temperature Sensors | 1 HEX  | Two battery temperature sensors.                                      |
| R   | `0x87` | Number of Battery Cycles          | 2 HEX     | Total number of charge/discharge cycles.                              |
| R   | `0x89` | Total Battery Cycle Capacity      | 4 HEX     | Total capacity over all cycles.                                       |
| R   | `0x8A` | Total Number of Battery Strings   | 2 HEX     | Number of battery strings in the pack.                                |
| R   | `0x8B` | Battery Warning Message           | 2 HEX     | Warning flags for various conditions.                                 |
| R   | `0x8C` | Battery Status Information        | 2 HEX     | Bit-coded status information.                                         |
| RW  | `0x8E` | Total Voltage Overvoltage Protection | 2 HEX  | Minimum unit 10mV.                                                    |
| RW  | `0x8F` | Total Voltage Undervoltage Protection | 2 HEX | Minimum unit 10mV.                                                    |
| RW  | `0x90` | Single Overvoltage Protection Voltage | 2 HEX | 1000–4500mV.                                                          |
| RW  | `0x91` | Cell Overvoltage Recovery Voltage | 2 HEX     | 1000–4500mV.                                                          |
| RW  | `0x92` | Single Overvoltage Protection Delay | 2 HEX   | 1–60 seconds.                                                         |
| RW  | `0x93` | Single Undervoltage Protection Voltage | 2 HEX | 1000–4500mV.                                                          |
| RW  | `0x94` | Single Undervoltage Recovery Voltage | 2 HEX  | 1000–4500mV.                                                          |
| RW  | `0x95` | Single Undervoltage Protection Delay | 2 HEX  | 1–60 seconds.                                                         |
| RW  | `0x96` | Cell Pressure Difference Protection Value | 2 HEX | 0–1000mV.                                                             |
| RW  | `0x97` | Discharge Overcurrent Protection Value | 2 HEX | 1–1000A.                                                              |
| RW  | `0x98` | Discharge Overcurrent Delay       | 2 HEX     | 1–60 seconds.                                                         |
| RW  | `0x99` | Charging Overcurrent Protection Value | 2 HEX | 1–1000A.                                                              |
| RW  | `0x9A` | Charge Overcurrent Delay          | 2 HEX     | 1–60 seconds.                                                         |
| RW  | `0x9B` | Balanced Starting Voltage         | 2 HEX     | 2000–4500mV.                                                          |
| RW  | `0x9C` | Balanced Opening Pressure Difference | 2 HEX  | 10–1000mV.                                                            |
| RW  | `0x9D` | Active Balance Switch            | 1 HEX     | 0 = off, 1 = on.                                                      |
| RW  | `0x9E` | Power Tube Temperature Protection Value | 2 HEX | 0–100°C.                                                              |
| RW  | `0x9F` | Power Tube Temperature Recovery Value | 2 HEX | 0–100°C.                                                              |
| RW  | `0xA0` | Temperature Protection Value in Battery Box | 2 HEX | 40–100°C.                                                             |
| RW  | `0xA1` | Temperature Recovery Value in Battery Box | 2 HEX | 40–100°C.                                                             |
| RW  | `0xA2` | Battery Temperature Difference Protection Value | 2 HEX | 5–20°C.                                                               |
| RW  | `0xA3` | Battery Charging High Temperature Protection Value | 2 HEX | 0–100°C.                                                              |
| RW  | `0xA4` | Battery Discharge High Temperature Protection Value | 2 HEX | 0–100°C.                                                              |
| RW  | `0xA5` | Charging Low Temperature Protection Value | 2 HEX | -45 to +25°C.                                                         |
| RW  | `0xA6` | Charging Low Temperature Protection Recovery Value | 2 HEX | -45 to +25°C.                                                         |
| RW  | `0xA7` | Discharge Low Temperature Protection Value | 2 HEX | -45 to +25°C.                                                         |
| RW  | `0xA8` | Discharge Low Temperature Protection Recovery Value | 2 HEX | -45 to +25°C.                                                         |
| RW  | `0xA9` | Battery String Setting           | 1 HEX     | 3–32.                                                                 |
| RW  | `0xAA` | Battery Capacity Setting         | 4 HEX     | Amp-hour (Ah).                                                        |
| RW  | `0xAB` | Charging MOS Tube Switch         | 1 HEX     | 0 = off, 1 = on.                                                      |
| RW  | `0xAC` | Discharge MOS Tube Switch        | 1 HEX     | 0 = off, 1 = on.                                                      |
| RW  | `0xAD` | Current Calibration              | 2 HEX     | 100mA–20000mA.                                                        |
| RW  | `0xAE` | Protection Board Address         | 1 HEX     | Reserved for cascading.                                               |
| RW  | `0xAF` | Battery Type                     | 1 HEX     | 0 = lithium iron phosphate, 1 = ternary, 2 = lithium titanate.        |
| RW  | `0xB0` | Sleep Waiting Time               | 2 HEX     | Seconds.                                                              |
| RW  | `0xB1` | Low Volume Alarm Value           | 1 HEX     | 0–80%.                                                                |
| RW  | `0xB2` | Modify Parameter Password        | 10 HEX    | Fixed password.                                                       |
| RW  | `0xB3` | Dedicated Charger Switch         | 1 HEX     | 0 = off, 1 = on.                                                      |
| RW  | `0xB4` | Device ID Code                   | 8 CHAR    | Example: `60300001`.                                                  |
| RW  | `0xB5` | Date of Manufacture              | 4 CHAR    | Example: `2004` (year and month).                                     |
| RW  | `0xB6` | System Working Hours             | 4 HEX     | Cleared at factory, unit: minutes.                                    |
| R   | `0xB7` | Software Version Number          | 15 CHAR   | Example: `NW_1_0_0_200428`.                                           |
| RW  | `0xB8` | Start Current Calibration        | 1 HEX     | 1 = start, 0 = close.                                                 |
| RW  | `0xB9` | Actual Battery Capacity          | 4 HEX     | Amp-hour (Ah).                                                        |
| RW  | `0xBA` | Manufacturer ID Naming           | 24 CHAR   | Manufacturer-specific.                                                |
| W   | `0xBB` | Restart the System               | 1 HEX     | 1 = restart.                                                          |
| W   | `0xBC` | Reset                            | 1 HEX     | 1 = restore factory parameters.                                       |
| W   | `0xBD` | Remote Upgrade Logo              | 1 HEX     | 1 = start upgrade.                                                    |
| W   | `0xBE` | Battery Low Voltage Turns Off GPS | 2 HEX    | Unit: mV.                                                             |
| W   | `0xBF` | Battery Low Voltage Recovery GPS | 2 HEX     | Unit: mV.                                                             |
| R   | `0xC0` | Protocol Version Number          | 1 HEX     | Default: `0x00`.                                                      |

1. All fields from 0x79 to 0xb9 marked with R or RW should be reported. For the old versions that have been shipped but not reported, try to upgrade. If it is inconvenient to upgrade, Contact our technical support, Tel: 13755639263/13480924112
2. 0xBA Manufacturer ID naming. This field is mainly used for battery swap cabinets. If there is a need for a battery swap cabinet, this field must be added.
### Note 1

The following describes the bit-level alarms and their meanings:

| Bit Position | Alarm Description                          | Status       |
|--------------|--------------------------------------------|--------------|
| **0**        | Low capacity alarm                        | `1` = Alarm, `0` = Normal |
| **1**        | MOS tube over-temperature alarm           | `1` = Alarm, `0` = Normal |
| **2**        | Charging over-voltage alarm               | `1` = Alarm, `0` = Normal |
| **3**        | Discharge undervoltage alarm              | `1` = Alarm, `0` = Normal |
| **4**        | Battery over-temperature alarm            | `1` = Alarm, `0` = Normal |
| **5**        | Charging overcurrent alarm                | `1` = Alarm, `0` = Normal |
| **6**        | Discharge overcurrent alarm               | `1` = Alarm, `0` = Normal |
| **7**        | Cell pressure difference alarm            | `1` = Alarm, `0` = Normal |
| **8**        | Over-temperature alarm in the battery box | `1` = Alarm, `0` = Normal |
| **9**        | Battery low-temperature alarm             | `1` = Alarm, `0` = Normal |
| **10**       | Monomer overvoltage alarm                 | `1` = Alarm, `0` = Normal |
| **11**       | Monomer undervoltage alarm                | `1` = Alarm, `0` = Normal |
| **12**       | 309_A protection                          | `1` = Alarm, `0` = Normal |
| **13**       | 309_B protection                          | `1` = Alarm, `0` = Normal |
| **14**       | Reserved                                  | -            |
| **15**       | Reserved                                  | -            |

#### Example:
- `0x0001`: Indicates a low capacity alarm.
- `0x0002`: Indicates a power board over-temperature alarm.
- `0x0003`: Indicates both a low capacity alarm and a power board over-temperature alarm.


### Note 2

The column format `"BT3072020120000200521001"` is structured as follows:

| Field               | Description                                                                 |
|---------------------|-----------------------------------------------------------------------------|
| **Product Name**    | `"BT"` for battery.                                                        |
| **Material System** | `1` = Iron-lithium, `2` = Manganese acid, `3` = Ternary.                   |
| **Voltage Level**   | `48` = 48V series, `60` = 60V series, `72` = 72V series.                   |
| **Capacity Grade**  | `20` = 20AH specification.                                                 |
| **Cycle Life**      | `04` = 400 cycles, `12` = 1200 cycles.                                     |
| **Manufacturer Code** | English code for the manufacturer. If less than 4 characters, pad with `0`. |
| **Production Year** | Last two digits of the production year (e.g., `19` for 2019).              |
| **Production Month**| `01` to `12` for January to December.                                      |
| **Production Date** | `01` to `31` for the day of the month.                                     |
| **Serial Number**   | Sequential number for the production batch on the given date.              |

#### Example:
- `"BT3072020120000200521001"` can be interpreted as:
    - **Product Name**: BT
    - **Material System**: Ternary
    - **Voltage Level**: 72V
    - **Capacity Grade**: 20AH
    - **Cycle Life**: 1200 cycles
    - **Manufacturer Code**: `0000`
    - **Production Year**: 2020
    - **Production Month**: December
    - **Production Date**: 20th
    - **Serial Number**: 001

### Reference

1. [mpp-solar/issues/112 - full table](https://github.com/jblance/mpp-solar/issues/112#issuecomment-850585642)
2. [Sample complete code](https://github.com/syssi/esphome-jk-bms/blob/main/components/jk_bms/jk_bms.cpp)
3. [BMS Communication details](https://off-grid-garage.com/bms-communication/)
4. [BMS Port details](https://user-images.githubusercontent.com/74740200/175492249-1b7c1cdc-b787-49f2-89d8-dc7130545e71.png)
5. [Official Docs](/docs/)