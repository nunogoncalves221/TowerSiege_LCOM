#include "rtc.h"
#include <minix/syslib.h>

/* Register Addresses and Masks */
#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71
#define RTC_REG_A 0x0A
#define RTC_REG_B 0x0B
#define RTC_REG_DAY 0x07
#define RTC_REG_MONTH 0x08
#define RTC_REG_YEAR 0x09
#define RTC_UIP_MSK (1 << 7)
#define RTC_DM_MSK (1 << 2)

/**
 * @brief Converts a BCD value to binary.
 * @param bcd BCD value to convert.
 * @return Binary representation of the value.
 */
static int bcd_to_bin(uint8_t bcd) { 
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

/**
 * @brief Reads the current date from the RTC.
 * @param date Pointer to rtc_date structure to be filled.
 * @return 0 on success, 1 on hardware/system call failure.
 */
int rtc_read_date(rtc_date *date) { 
    uint32_t regA, regB;
    uint32_t day, month, year;

    /* Wait until RTC is not updating (UIP == 0) */
    do {
        if (sys_outb(RTC_ADDR_REG, RTC_REG_A) != OK) return 1; /* Select Register A */
        if (sys_inb(RTC_DATA_REG, &regA) != OK) return 1;      /* Read Register A value */
    } while (regA & RTC_UIP_MSK);                             /* Loop while UIP bit is 1 */

    /* Read Register B to check data mode (BCD or Binary) */
    if (sys_outb(RTC_ADDR_REG, RTC_REG_B) != OK) return 1;
    if (sys_inb(RTC_DATA_REG, &regB) != OK) return 1;

    /* Read Day, Month, and Year from their respective registers */
    if (sys_outb(RTC_ADDR_REG, RTC_REG_DAY) != OK) return 1;
    if (sys_inb(RTC_DATA_REG, &day) != OK) return 1;

    if (sys_outb(RTC_ADDR_REG, RTC_REG_MONTH) != OK) return 1;
    if (sys_inb(RTC_DATA_REG, &month) != OK) return 1;

    if (sys_outb(RTC_ADDR_REG, RTC_REG_YEAR) != OK) return 1;
    if (sys_inb(RTC_DATA_REG, &year) != OK) return 1;

    /* If DM (Data Mode) bit is 0, data is in BCD and must be converted */
    if (!(regB & RTC_DM_MSK)) { 
        day = bcd_to_bin((uint8_t)day);
        month = bcd_to_bin((uint8_t)month);
        year = bcd_to_bin((uint8_t)year);
    }

    /* Store values in the structure using the pointer */
    date->day = (uint8_t) day;
    date->month = (uint8_t) month;
    date->year = (uint8_t) year;

    return 0; /* Success */
}