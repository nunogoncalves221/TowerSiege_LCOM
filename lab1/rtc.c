#include "rtc.h"
#include <minix/syslib.h>

#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71
#define RTC_REG_A 0x0A
#define RTC_REG_B 0x0B
#define RTC_REG_DAY 0x07
#define RTC_REG_MONTH 0x08
#define RTC_REG_YEAR 0x09
#define RTC_UIP_MSK (1 << 7)  // Update In Progress bit
#define RTC_DM_MSK (1 << 2)   // Data Mode bit (BCD or binary)

// Convert BCD value to binary
static int bcd_to_bin(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

// Wait until RTC is not updating (UIP == 0)
static void wait_for_uip_clear() {
    uint32_t regA;
    do {
        sys_outb(RTC_ADDR_REG, RTC_REG_A);  // select register A
        sys_inb(RTC_DATA_REG, &regA);       // read register A
    } while (regA & RTC_UIP_MSK);           // loop while UIP bit is set
}

// Read the current date from the RTC and store it in the rtc_date structure
int rtc_read_date(rtc_date *date) {
    uint32_t regB, day, month, year;

    wait_for_uip_clear(); // ensure RTC is not updating

    // Read register B to check data mode
    sys_outb(RTC_ADDR_REG, RTC_REG_B);
    sys_inb(RTC_DATA_REG, &regB);

    // Read day, month, and year from RTC
    sys_outb(RTC_ADDR_REG, RTC_REG_DAY);
    sys_inb(RTC_DATA_REG, &day);

    sys_outb(RTC_ADDR_REG, RTC_REG_MONTH);
    sys_inb(RTC_DATA_REG, &month);

    sys_outb(RTC_ADDR_REG, RTC_REG_YEAR);
    sys_inb(RTC_DATA_REG, &year);

    // Convert from BCD to binary if necessary
    if (!(regB & RTC_DM_MSK)) {
        day   = bcd_to_bin(day);
        month = bcd_to_bin(month);
        year  = bcd_to_bin(year);
    }

    // Store the values in the rtc_date structure
    date->day   = (uint8_t)day;
    date->month = (uint8_t)month;
    date->year  = (uint8_t)year;

    return 0;  // success
}