/**
 * @file high_scores.h
 * @brief Persistent high-score table with up to ten entries.
 *
 * Entries are stored on disk and loaded at startup. Each entry holds a
 * player name, a score, and the date the run was completed. The table is
 * kept sorted in descending score order.
 */

#ifndef HIGH_SCORES_H
#define HIGH_SCORES_H

#include <stdint.h>
#include <stdbool.h>
#include "rtc.h"

#define HS_MAX_ENTRIES  10
#define HS_MAX_NAME_LEN 8

/** @brief A single entry in the high-score table. */
typedef struct {
    char     name[HS_MAX_NAME_LEN + 1]; /**< Player name, null-terminated. */
    int      score;                     /**< Score achieved in that run. */
    rtc_date date;                      /**< Date the run was completed. */
} hs_entry_t;

/**
 * @brief Loads the high-score table from disk.
 *
 * Must be called once at application startup before any other hs_ function.
 * If the file does not exist, the table starts empty.
 */
void hs_load(void);

/**
 * @brief Writes the current high-score table to disk.
 *
 * Should be called after hs_insert() to persist the new entry.
 */
void hs_save(void);

/**
 * @brief Returns whether the given score would enter the top-ten table.
 *
 * @param score Score to test.
 * @return true if the table has fewer than ::HS_MAX_ENTRIES entries, or if
 *         @p score is strictly greater than the lowest existing entry.
 */
bool hs_qualifies(int score);

/**
 * @brief Inserts a new entry into the high-score table and re-sorts it.
 *
 * If the table is already full, the lowest-scoring entry is evicted.
 *
 * @param name  Player name (at most ::HS_MAX_NAME_LEN characters).
 * @param score Score achieved.
 * @param date  RTC date of the completed run.
 */
void hs_insert(const char *name, int score, rtc_date date);

/**
 * @brief Returns a pointer to the sorted array of high-score entries.
 *
 * @return Read-only pointer to the internal array of ::hs_entry_t.
 *         The array has at most ::HS_MAX_ENTRIES valid entries.
 */
const hs_entry_t *hs_get_entries(void);

/**
 * @brief Draws the high-score leaderboard screen.
 */
void hs_draw(void);

#endif /* HIGH_SCORES_H */
