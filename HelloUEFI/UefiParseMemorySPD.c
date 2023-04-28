#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/SmbusHc.h>
#include <Library/UefiLib.h>

EFI_STATUS FindDeviceSupportSMbus();
EFI_STATUS ParseSPDByte(IN UINT8 slave_address, IN UINT8 register_index, OUT UINT8 *value);
EFI_STATUS ParseSPDByte0to127(IN UINT8 *value);
VOID ParseCASLatency(IN UINT8 *value);

EFI_STATUS
UefiMain(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
) 
{
    // Detect devices support SMbus protocol
    EFI_STATUS Status = FindDeviceSupportSMbus();

    if (EFI_ERROR(Status)) {
        Print(L"Find Device Error: No such device or something wrong.\n");
        Print(L"\n- Failed -\n");
        return EFI_ABORTED;
    }

    // Get SPD 0~127 bytes information
    UINT8 offset;
    UINT8 count;
    UINT8 value[128];  // obtain SPD Configuration values
    UINT8 SlaveAddr = 0xA0;
    Print(L"   ");
    for (count = 0x00; count < 0x10; ++count) {
        Print(L"%02x ", count);
    }
    for (count = 0x00, offset = 0x00; offset < 0x80; ++offset) {
        if (offset % 0x10 == 0) {
            Print(L"\n");
            Print(L"%d0 ", count);
            ++count;
        }
        Status = ParseSPDByte(SlaveAddr, offset, value+offset);
        if (!EFI_ERROR(Status)) {
            Print(L"%02x ", value[offset]);  // print the value
        }
        else {
            // Print(L"Parse Error.\n");
            // Print(L"\n- Failed -\n");
            // return EFI_ABORTED;

            Print(L"Slave address: 0x%02x\n", SlaveAddr);
            if (SlaveAddr >= 0xC8) return EFI_ABORTED;
            count = 0;
            offset = 0;
            SlaveAddr = SlaveAddr + 2;
            continue;
        }
    }

    Status = ParseSPDByte0to127(value);

    Print(L"\n- Done -\n");
    return EFI_SUCCESS;
}

// Already detected in experiment. Just 1 device.
EFI_STATUS FindDeviceSupportSMbus()
{   
    EFI_STATUS Status;
    EFI_HANDLE *device_support_smbus = NULL;
    UINTN device_num, device_count;
    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiSmbusHcProtocolGuid,
        NULL,
        &device_num,
        &device_support_smbus
    );

    if (!EFI_ERROR(Status)) {
        Print(L"There is(are) %d device(s) support SMbus protocol.\n", device_num);
        for (device_count = 0; device_count < device_num; ++device_count) {
            Print(L"Handle[%d]: 0x%08x\n", device_count, device_support_smbus[device_count]);
        }
    }

    return Status;
}

EFI_STATUS ParseSPDByte(IN UINT8 slave_address, IN UINT8 register_index, OUT UINT8* value) 
{
    EFI_STATUS Status;
    STATIC EFI_SMBUS_HC_PROTOCOL *smbus_interface;
    
    Status = gBS->LocateProtocol(&gEfiSmbusHcProtocolGuid, NULL, &smbus_interface);
    
    EFI_SMBUS_DEVICE_ADDRESS device_adress;
    EFI_SMBUS_DEVICE_COMMAND index;
    UINTN length = 0x01;
    UINT8 buffer8;
    device_adress.SmbusDeviceAddress = (slave_address >> 1);  // 0xA0, 0xA2, 0xA4...  slave address
    index = register_index;  // 0x01, 0x02, 0x03, 0x04...  offset

    if (!EFI_ERROR (Status)) {
        Status = smbus_interface->Execute(
            smbus_interface, 
            device_adress, 
            index, 
            EfiSmbusReadByte, 
            FALSE, 
            &length,
            &buffer8
        );

        if (!EFI_ERROR(Status)) {
            *value = buffer8;
        }
    }

    return Status;
}

EFI_STATUS ParseSPDByte0to127(IN UINT8 *value) {
    Print(L"\n\n==================== SPD General Configuration ====================\n");
    Print(L"===================================================================\n");
    // Parse Byte
    // 0
    Print(L"Byte 0 (0x000): Number of Bytes Used / Number of Bytes in SPD Device\n");
    Print(L"    SPD Bytes Used: ");
    switch (value[0] & 0x0f) {
    case 0x00:
        Print(L"Undefined\n");
        break;
    case 0x01:
        Print(L"128\n");
        break;
    case 0x02:
        Print(L"256\n");
        break;
    case 0x03:
        Print(L"384\n");
        break;
    case 0x04:
        Print(L"512\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    Print(L"    SPD Bytes Total: ");
    switch (value[0] & 0x70) {
    case 0x00:
        Print(L"Undefined\n");
        break;
    case 0x10:
        Print(L"256\n");
        break;
    case 0x20:
        Print(L"512\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    // 1
    Print(L"\nByte 1 (0x001): SPD Revision\n");
    if (value[1] < 9) {
        Print(L"    Pre-production: Revision 0.%d\n", value[1]);
    } 
    else if (value[1] < 255) {
        Print(L"    Production: Revision %x.%x\n", (value[1]&0xf0)>>4, value[1]&0x0f);
    }
    else {
        Print(L"    Undefined\n");
    }

    // 2
    Print(L"\nByte 2 (0x002): Key Byte / DRAM Device Type\n");
    Print(L"    SDRAM / Module Type: ");
    switch (value[2]) {
    case 0x01:
        Print(L"Fast Page Mode\n");
        break;
    case 0x02:
        Print(L"EOD\n");
        break;
    case 0x03:
        Print(L"Pipelined Nibble\n");
        break;
    case 0x04:
        Print(L"SDRAM\n");
        break;
    case 0x05:
        Print(L"ROM\n");
        break;
    case 0x06:
        Print(L"DDR SGRAM\n");
        break;
    case 0x07:
        Print(L"DDR SDRAM\n");
        break;
    case 0x08:
        Print(L"DDR2 SDRAM\n");
        break;
    case 0x09:
        Print(L"DDR2 SDRAM FB-DIMM\n");
        break;
    case 0x0A:
        Print(L"DDR2 SDRAM FB-DIMM PROBE\n");
        break;
    case 0x0B:
        Print(L"DDR3 SDRAM\n");
        break;
    case 0x0C:
        Print(L"DDR4 SDRAM\n");
        break;
    case 0x0E:
        Print(L"DDR4E SDRAM\n");
        break;
    case 0x0F:
        Print(L"LPDDR3 SDRAM\n");
        break;
    case 0x10:
        Print(L"LPDDR4 SDRAM\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    // 3
    Print(L"\nByte 3 (0x003): Key Byte / Module Type\n");
    Print(L"    Base Module Type: ");
    switch(value[3] & 0x0f) {
    case 0x00:
        Print(L"(Extended DIMM type)\n");
        break;
    case 0x01:
        Print(L"RDIMM\n");
        break;
    case 0x02:
        Print(L"UDIMM\n");
        break;
    case 0x03:
        Print(L"SO-DIMM\n");
        break;
    case 0x04:
        Print(L"LRDIMM\n");
        break;
    case 0x05:
        Print(L"Mini-RDIMM\n");
        break;
    case 0x06:
        Print(L"Mini-UDIMM\n");
        break;
    case 0x08:
        Print(L"72b-SO-RDIMM\n");
        break;
    case 0x09:
        Print(L"72b-SO-UDIMM\n");
        break;
    case 0x0C:
        Print(L"16b-SO-DIMM\n");
        break;
    case 0x0D:
        Print(L"32b-SO-DIMM\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    Print(L"    Hybrid Media: ");
    switch (value[3] & 0x70) {
    case 0x00:
        Print(L"Not hybrid\n");
        break;
    case 0x10:
        Print(L"NVDIMM Hybrid\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    Print(L"    Hybrid: ");
    switch (value[3] & 0x80) {
    case 0x00:
        Print(L"Not hybrid\n");
        break;
    case 0x80:
        Print(L"Hybrid module\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    // 4
    Print(L"\nByte 4 (0x004): SDRAM Density and Banks\n");
    Print(L"    Total SDRAM capacity per die (megabits): ");
    switch (value[4] &0x0f) {
    case 0x00:
        Print(L"256 Mb\n");
        break;
    case 0x01:
        Print(L"512 Mb\n");
        break;
    case 0x02:
        Print(L"1 Gb\n");
        break;
    case 0x03:
        Print(L"2 Gb\n");
        break;
    case 0x04:
        Print(L"4 Gb\n");
        break;
    case 0x05:
        Print(L"8 Gb\n");
        break;
    case 0x06:
        Print(L"16 Gb\n");
        break;
    case 0x07:
        Print(L"32 Gb\n");
        break;
    case 0x08:
        Print(L"12 Gb\n");
        break;
    case 0x09:
        Print(L"24 Gb\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    Print(L"    Bank: ");
    switch (value[4] & 0x30) {
    case 0x00:
        Print(L"4 banks\n");
        break;
    case 0x01:
        Print(L"8 banks\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    Print(L"    Bank Group: ");
    switch (value[4] & 0xC0) {
    case 0x00:
        Print(L"no bank groups\n");
        break;
    case 0x40:
        Print(L"2 bank groups\n");
        break;
    case 0x80:
        Print(L"4 bank groups\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    // 5
    Print(L"\nByte 5 (0x005): SDRAM Density and Banks\n");
    Print(L"    Column: ");
    switch (value[5] & 0x07) {
    case 0x00:
        Print(L"9\n");
        break;
    case 0x01:
        Print(L"10\n");
        break;
    case 0x02:
        Print(L"11");
        break;
    case 0x03:
        Print(L"12\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    Print(L"    Row: ");
    switch (value[5] & 0x78) {
    case 0x00:
        Print(L"12\n");
        break;
    case 0x08:
        Print(L"13\n");
        break;
    case 0x10:
        Print(L"14");
        break;
    case 0x18:
        Print(L"15\n");
        break;
    case 0x20:
        Print(L"16\n");
        break;
    case 0x28:
        Print(L"17\n");
        break;
    case 0x30:
        Print(L"18\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    // 6
    Print(L"\nByte 6 (0x006): Primary SDRAM Package Type\n");
    Print(L"    Signal Loading: ");
    switch (value[6] &0x03) {
    case 0x00:
        Print(L"Not specified\n");
        break;
    case 0x01:
        Print(L"Multi load stack\n");
        break;
    case 0x02:
        Print(L"Single load stack (3DS)\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    Print(L"    Die Count: ");
    switch (value[6] & 0x70) {
    case 0x00:
        Print(L"Single die\n");
        break;
    case 0x10:
        Print(L"2 die\n");
        break;
    case 0x20:
        Print(L"3 die\n");
        break;
    case 0x30:
        Print(L"4 die\n");
        break;
    case 0x40:
        Print(L"5 die\n");
        break;
    case 0x50:
        Print(L"6 die\n");
        break;
    case 0x60:
        Print(L"7 die\n");
        break;
    case 0x70:
        Print(L"8 die\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    Print(L"    Primary SDRAM Package Type: ");
    switch (value[6] & 0x80) {
    case 0x00:
        Print(L"Monolithic DRAM Device\n");
        break;
    case 0x80:
        Print(L"Non-Monolithic Device\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    // 7
    Print(L"\nByte 7 (0x007): SDRAM Optional Features\n");
    Print(L"    Maximum Activate Count (MAC): ");
    switch (value[7] & 0x0f) {
    case 0x00:
        Print(L"Untested MAC\n");
        break;
    case 0x01:
        Print(L"700 K\n");
        break;
    case 0x02:
        Print(L"600 K\n");
        break;
    case 0x03:
        Print(L"500 K\n");
        break;
    case 0x04:
        Print(L"400 K\n");
        break;
    case 0x05:
        Print(L"300 K\n");
        break;
    case 0x06:
        Print(L"200 K\n");
        break;
    case 0x08:
        Print(L"Unlimited MAC\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    Print(L"    Maximum Activate Window (tMAW): ");
    switch (value[7] & 0x30) {
    case 0x00:
        Print(L"8192 * tREFI\n");
        break;
    case 0x10:
        Print(L"4096 * tREFI\n");
        break;
    case 0x20:
        Print(L"2048 * tREFI\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    // 8
    Print(L"\nByte 8 (0x008): SDRAM Thermal and Refresh Options\n");
    Print(L"    (All Reserved)\n");

    // 9
    Print(L"\nByte 9 (0x009): Other SDRAM Optional Features\n");
    Print(L"    Soft PPR: ");
    switch (value[9] & 0x20) {
    case 0x00:
        Print(L"Soft PPR not supported\n");
        break;
    case 0x20:
        Print(L"Soft PPR supported\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    Print(L"    Post Package Repair (PPR): ");
    switch (value[9] & 0xC0) {
    case 0x00:
        Print(L"PPR not supported\n");
        break;
    case 0x40:
        Print(L"Post package repair supported, one row per bank group\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    // 10
    Print(L"\nByte 10 (0x00A): Secondary SDRAM Package Type\n");
    Print(L"    Signal Loading: ");
    switch (value[10] & 0x03) {
    case 0x00:
        Print(L"Not specified\n");
        break;
    case 0x01:
        Print(L"Multi load stack\n");
        break;
    case 0x02:
        Print(L"Single load stack (3DS)\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    Print(L"    DRAM Density Ratio: ");
    switch (value[10] & 0x0C) {
    case 0x00:
        Print(L"Rank 1 and 3 device densities are the same as rank 0 and 2 densities\n");
        break;
    case 0x04:
        Print(L"Rank 1 and 3 are one standard device density smaller than rank 0 and 2\n");
        break;
    case 0x08:
        Print(L"Rank 1 and 3 are two standard device densities smaller than rank 0 and 2\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    Print(L"    Die Count: ");
    switch (value[10] & 0x70) {
    case 0x00:
        Print(L"Single die\n");
        break;
    case 0x10:
        Print(L"2 die\n");
        break;
    case 0x20:
        Print(L"3 die\n");
        break;
    case 0x30:
        Print(L"4 die\n");
        break;
    case 0x40:
        Print(L"5 die\n");
        break;
    case 0x50:
        Print(L"6 die\n");
        break;
    case 0x60:
        Print(L"7 die\n");
        break;
    case 0x70:
        Print(L"8 die\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    Print(L"    SDRAM Package Type: ");
    switch (value[10] & 0x80) {
    case 0x00:
        Print(L"Monolithic DRAM Device\n");
        break;
    case 0x80:
        Print(L"Non-Monolithic Device\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    // 11
    Print(L"\nByte 11 (0x00B): Module Nominal Voltage, VDD\n");
    Print(L"    DRAM VDD 1.2 V: ");
    switch (value[11] & 0x01) {
    case 0x00:
        Print(L"not operable & ");
        break;
    case 0x01:
        Print(L"operable & ");
        break;
    default:
        Print(L"\n");
        break;
    }

    switch (value[11] & 0x02) {
    case 0x00:
        Print(L"not endurant\n");
        break;
    case 0x02:
        Print(L"endurant\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    // 12
    Print(L"\nByte 12 (0x00C):  Module Organization\n");
    Print(L"    SDRAM Device Width: ");
    switch (value[12] & 0x07) {
    case 0x00:
        Print(L"4 bits\n");
        break;
    case 0x01:
        Print(L"8 bits\n");
        break;
    case 0x02:
        Print(L"16 bits\n");
        break;
    case 0x03:
        Print(L"32 bits\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    Print(L"    Number of Package Ranks per DIMM: ");
    switch (value[12] & 0x38) {
    case 0x00:
        Print(L"1 Package Rank\n");
        break;
    case 0x08:
        Print(L"2 Package Rank\n");
        break;
    case 0x10:
        Print(L"3 Package Rank\n");
        break;
    case 0x18:
        Print(L"4 Package Rank\n");
        break;
    case 0x20:
        Print(L"5 Package Rank\n");
        break;
    case 0x28:
        Print(L"6 Package Rank\n");
        break;
    case 0x30:
        Print(L"7 Package Rank\n");
        break;
    case 0x38:
        Print(L"8 Package Rank\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    Print(L"    Rank Mix: ");
    switch (value[12] & 0x40) {
    case 0x00:
        Print(L"Symmetrical\n");
        break;
    case 0x40:
        Print(L"Asymmetrical\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    // 13
    Print(L"\nByte 13 (0x00D): Module Memory Bus Width\n");
    Print(L"    Primary bus width: ");
    switch (value[13] & 0x07) {
    case 0x00:
        Print(L"8 bits\n");
        break;
    case 0x01:
        Print(L"16 bits\n");
        break;
    case 0x02:
        Print(L"32 bits\n");
        break;
    case 0x03:
        Print(L"64 bits\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    Print(L"    Bus width extension: ");
    switch (value[13] & 0x18) {
    case 0x00:
        Print(L"0 bits (no extension)\n");
        break;
    case 0x08:
        Print(L"8 bits\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    // 14
    Print(L"\nByte 14 (0x00E): Module Thermal Sensor\n");
    Print(L"    Thermal Sensor: ");
    switch (value[14] & 0x80) {
    case 0x00:
        Print(L"Thermal sensor not incorporated onto this assembly\n");
        break;
    case 0x80:
        Print(L"Thermal sensor incorporated onto this assembly\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    // 15
    Print(L"\nByte 15 (0x00F): Extended Module Type\n");
    Print(L"    Extended Base Module Type: (All Reserved)\n");

    // 16
    Print(L"\nByte 16 (0x010): (All Reserved)\n");

    // 17
    Print(L"\nByte 17 (0x011): Timebases\n");
    Print(L"    Fine Timebase(FTB): ");
    switch (value[17] & 0x02) {
    case 0x00:
        Print(L"1 ps\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    Print(L"    Medium Timebase (MTB): ");
    switch (value[17] & 0x0C) {
    case 0x00:
        Print(L"125 ps\n");
        break;
    default:
        Print(L"\n");
        break;
    }

    // 18
    Print(L"\nByte 18 (0x012): SDRAM Minimum Cycle Time (tCKAVGmin)\n");
    Print(L"    tCKAVGmin MTB Units: %d\n", value[18]);

    // 19
    Print(L"\nByte 19 (0x013): SDRAM Maximum Cycle Time (tCKAVGmax)\n");
    Print(L"    tCKAVGmax MTB Units: %d\n", value[19]);

    // 20~23
    Print(L"\nByte 20~23 (0x014~0x017): CAS Latencies Supported\n");
    Print(L"    Actual CAS Latencies supported = ");
    ParseCASLatency(value);
    Print(L"\n");

    // 24
    Print(L"\nByte 24 (0x018): Minimum CAS Latency Time (tAAmin)\n");
    Print(L"    tAAmin MTB Unit: %d\n", value[24]);

    // 25
    Print(L"\nByte 25 (0x019): Minimum RAS to CAS Delay Time (tRCDmin)\n");
    Print(L"    tRCDmin MTB Unit: %d\n", value[25]);

    // 26
    Print(L"\nByte 26 (0x01A): Minimum Row Precharge Delay Time (tRPmin)\n");
    Print(L"    tRPmin MTB Unit: %d\n", value[26]);

    // 27
    Print(L"\nByte 27 (0x01B): Upper Nibbles for tRASmin and tRCmin\n");
    Print(L"    tRASmin Most Significant Nibble: (Bits 3~0)\n");
    Print(L"    tRCmin  Most Significant Nibble: (Bits 7~4)\n");

    // 28
    Print(L"\nByte 28 (0x01C): Minimum Active to Precharge Delay Time (tRASmin), Least Significant Byte\n");
    Print(L"    tRASmin MTB Units: %d\n", (0x000f & value[27]) << 8 | value[28] );  // 12bits

    // 29
    Print(L"\nByte 29 (0x01D): Minimum Active to Active/Refresh Delay Time (tRCmin), Least Significant Byte\n");
    Print(L"    tRCmin  MTB Units: %d\n", (0x00f0 & value[27]) << 4 | value[29] );  // 12bits

    // 30~31
    Print(L"\nByte 30~31 (0x01E~0x01F): Minimum Refresh Recovery Delay Time (tRFC1min)\n");
    Print(L"    tRFC1min: %d\n", (0x00ff & value[31]) << 8 |value[30] );

    // 32~33
    Print(L"\nByte 32~33 (0x020~0x021): Minimum Refresh Recovery Delay Time (tRFC2min)\n");
    Print(L"    tRFC2min: %d\n", (0x00ff & value[33]) << 8 |value[32] );

    // 34~35
    Print(L"\nByte 34~35 (0x022~0x023): Minimum Refresh Recovery Delay Time (tRFC3min)\n");
    Print(L"    tRFC3min: %d\n", (0x00ff & value[35]) << 8 |value[34] );

    // 36
    Print(L"\nByte 36 (0x024): Upper Nibble for tFAW\n");
    Print(L"    tFAW Most Significant Nibble: (Bits 3~0)\n");

    // 37
    Print(L"\nByte 37 (0x025): Minimum Four Activate Window Delay Time (tFAWmin), Least Significant Byte\n");
    Print(L"    tFAW MTB Units: %d\n", (0x000f & value[36]) << 8 | value[37] );

    // 38
    Print(L"\nByte 38 (0x026): Minimum Activate to Activate Delay Time (tRRD_Smin), different bank group\n");
    Print(L"    tRRD_Smin MTB Units: %d\n", value[38] );

    // 39
    Print(L"\nByte 39 (0x027): Minimum Activate to Activate Delay Time (tRRD_Lmin), same bank group\n");
    Print(L"    tRRD_Lmin MTB Units: %d\n", value[39] );

    // 40
    Print(L"\nByte 40 (0x028): Minimum CAS to CAS Delay Time (tCCD_Lmin), same bank group\n");
    Print(L"    tCCD_Lmin MTB Units: %d\n", value[40] );

    // 41
    Print(L"\nByte 41 (0x029): Upper Nibble for tWRmin\n");
    Print(L"    tWRmin Most Significant Nibble: (Bits 3~0)\n");

    // 42
    Print(L"\nByte 42 (0x02A): Minimum Write Recovery Time (tWRmin)\n");
    Print(L"    tFAW MTB Units: %d\n", (0x000f & value[41]) << 8 | value[42] );

    // 43
    Print(L"\nByte 43 (0x029): Upper Nibbles for tWTRmin\n");
    Print(L"    tWTR_Smin Most Significant Nibble: (Bits 3~0)\n");
    Print(L"    tWTR_Lmin Most Significant Nibble: (Bits 7~4)\n");

    // 44
    Print(L"\nByte 44 (0x02C): Minimum Write to Read Time (tWTR_Smin), different bank group\n");
    Print(L"    tWTR_Smin MTB Units: %d\n", (0x000f & value[43]) << 8 | value[44] );

    // 45
    Print(L"\nByte 45 (0x02D): Minimum Write to Read Time (tWTR_Lmin), same bank group\n");
    Print(L"    tWTR_Lmin MTB Units: %d\n", (0x00f0 & value[43]) << 4 | value[45] );

    // 46~59
    Print(L"\nByte 46~59 (0x02E~0x03B): Reserved, Base Configuration Section\n");

    // 60~77
    Print(L"\nBytes 60~77 (0x03C~0x04D): Connector to SDRAM Bit Mapping\n");
    Print(L"    (Too much too large)\n");

    // 78~116
    Print(L"\nBytes 78~116 (0x04E~0x074): Reserved, Base Configuration Section\n");

    // 117
    Print(L"\nByte 117 (0x075): Fine Offset for Minimum CAS to CAS Delay Time (tCCD_Lmin), same bank group\n");

    // 118
    Print(L"\nByte 118 (0x076): Fine Offset for Minimum Activate to Activate Delay Time (tRRD_Lmin), same bank group\n");

    // 119
    Print(L"\nByte 119 (0x077): Fine Offset for Minimum Activate to Activate Delay Time (tRRD_Smin), different bank group\n");

    // 120
    Print(L"\nByte 120 (0x078): Fine Offset for Minimum Active to Active/Refresh Delay Time (tRCmin)\n");

    // 121
    Print(L"\nByte 121 (0x079): Fine Offset for Minimum Row Precharge Delay Time (tRPmin)\n");

    // 122
    Print(L"\nByte 122 (0x07A): Fine Offset for Minimum RAS to CAS Delay Time (tRCDmin)\n");

    // 123
    Print(L"\nByte 123 (0x07B): Fine Offset for Minimum CAS Latency Time (tAAmin)\n");

    // 124
    Print(L"\nByte 124 (0x07C): Fine Offset for SDRAM Maximum Cycle Time (tCKAVGmax)\n");

    // 125
    Print(L"\nByte 125 (0x07D): Fine Offset for SDRAM Minimum Cycle Time (tCKAVGmin)\n");

    // 126~127
    Print(L"\nByte 126~127 (0x07E~0x07F): Cyclical Redundancy Code (CRC) for Base Configuration Section\n");
    Print(L"    CRC126: 0x%02x\n", value[126]);
    Print(L"    CRC127: 0x%02x\n", value[127]);

    return EFI_SUCCESS;
}

VOID ParseCASLatency(IN UINT8 *value) {
    // 20~23
    UINT8 count;
    UINT8 move_bits;
    UINT8 CL;

    if ((value[23] & 0x80) == 0x00) {
        CL = 7;
    }
    else {
        CL = 23;
    }

    // 20~22
    for (count = 0; count < 3; ++count) {
        if ((value[count+20] & 0xff) == 0x00) {
            continue;
        }

        for (move_bits = 0; move_bits < 8; ++move_bits) {
            if ( ((value[count+20] >> move_bits) & 0x01) == 0x00 ) {
                continue;
            }
            else {
                Print(L"%d ", CL + count*8 + move_bits);
            }
        }
    }

    // 23
    for (move_bits = 0; move_bits < 6; ++move_bits) {
            if ( ((value[count+20] >> move_bits) & 0x01) == 0x00 ) {
                continue;
            }
            else {
                Print(L"%d ", CL + count*8 + move_bits);
            }
    }

}
