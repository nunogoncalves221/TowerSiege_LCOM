#include "rtc.h"
#include <minix/syslib.h> //para usar sys_inb e sys_outb
#define TODO return -1

#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71
#define RTC_REG_A 0x0A
#define RTC_REG_B 0x0B
#define RTC_REG_DAY 0x07
#define RTC_REG_MONTH 0x08
#define RTC_REG_YEAR 0x09
#define RTC_UIP_MSK (1 << 7)
#define RTC_DM_MSK (1 << 2)

//função para converter BCD em Binário
static int bdc_to_bin(uint8_t bcd) { 
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

//função principal para ler a data
int rtc_read_date(rtc_date *date) { 
    uint32_t regA, regB;
    uint32_t day, month, year;

    //esperar que o RTC não esteja a atualizar (UIP == 0)
    do {
        sys_outb(RTC_ADDR_REG, RTC_REG_A); //avisa que queremos ler o Registo A
        sys_inb(RTC_DATA_REG, &regA);      //lê o valor do Registo A
    } while (regA & RTC_UIP_MSK);          //fica preso no ciclo enquanto o bit 7 for 1

    //ler os valores do Dia, Mês e Ano dos respetivos registos
    if (sys_outb(RTC_ADDR_REG, RTC_REG_DAY)  == 0) sys_inb(RTC_DATA_REG, &day);
    if (sys_outb(RTC_ADDR_REG, RTC_REG_MONTH) == 0) sys_inb(RTC_DATA_REG, &month);
    if (sys_outb(RTC_ADDR_REG, RTC_REG_YEAR) == 0) sys_inb(RTC_DATA_REG, &year);

    //ler o Registo B para verificar se os dados estão em BCD
    sys_outb(RTC_ADDR_REG, RTC_REG_B);
    sys_inb(RTC_DATA_REG, &regB);

    //se o bit DM (Data Mode) for 0, os dados estão em BCD e temos de converter
    if (!(regB & RTC_DM_MSK)) { 
        day = bdc_to_bin(day);
        month = bdc_to_bin(month);
        year = bdc_to_bin(year);
    }

    //guardar os valores na estrutura usando o ponteiro
    date->day = (uint8_t) day;
    date->month = (uint8_t) month;
    date->year = (uint8_t) year;

    return 0;
 }
