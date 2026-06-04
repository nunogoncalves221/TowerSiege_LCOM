#ifndef HIGH_SCORES_H
#define HIGH_SCORES_H

#include <stdint.h>
#include <stdbool.h>
#include "rtc.h"

#define HS_MAX_ENTRIES  10
#define HS_MAX_NAME_LEN 15

typedef struct {
    char     name[HS_MAX_NAME_LEN + 1];
    int      score;
    rtc_date date;
} hs_entry_t;

void hs_load(void);
void hs_save(void);
bool hs_qualifies(int score);
void hs_insert(const char *name, int score, rtc_date date);
const hs_entry_t *hs_get_entries(void);
void hs_draw(void);

#endif
