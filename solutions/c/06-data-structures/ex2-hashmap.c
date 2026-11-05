/* Solution: C 06 Exercise 2 — Hash map (string -> int). Attempt the exercise first.
 * (Milestone M2 extends this into a reusable library with an iterator and benchmarks.)
 *
 * Usage: hashmap              word count: reads words from stdin
 *        hashmap --stress     100,000 random operations compared against a slow reference
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { SLOT_EMPTY, SLOT_USED, SLOT_DELETED };

typedef struct {
    char *key;          /* owned by the map */
    int value;
    unsigned char state;
} Slot;

typedef struct {
    Slot *slots;
    size_t capacity;    /* always a power of two */
    size_t count;       /* used slots */
    size_t tombstones;  /* deleted slots */
} Map;

static uint32_t fnv1a(const char *s)
{
    uint32_t h = 2166136261u;
    for (const unsigned char *p = (const unsigned char *)s; *p != '\0'; p++) {
        h ^= *p;
        h *= 16777619u;
    }
    return h;
}

static int map_init_cap(Map *m, size_t capacity)
{
    m->slots = calloc(capacity, sizeof *m->slots);   /* calloc: every slot starts SLOT_EMPTY */
    m->capacity = capacity;
    m->count = 0;
    m->tombstones = 0;
    return m->slots ? 0 : -1;
}

/* Returns 0, or -1 if out of memory. The caller must call map_free. */
int map_init(Map *m)
{
    return map_init_cap(m, 8);
}

void map_free(Map *m)
{
    for (size_t i = 0; i < m->capacity; i++) {
        if (m->slots[i].state == SLOT_USED) {
            free(m->slots[i].key);
        }
    }
    free(m->slots);
    m->slots = NULL;
}

/* Index of `key`'s slot if present, else SIZE_MAX. */
static size_t find_slot(const Map *m, const char *key)
{
    size_t mask = m->capacity - 1;
    for (size_t i = fnv1a(key) & mask, probes = 0; probes < m->capacity; i = (i + 1) & mask, probes++) {
        const Slot *s = &m->slots[i];
        if (s->state == SLOT_EMPTY) {
            return SIZE_MAX;                       /* a never-used slot ends the probe chain */
        }
        if (s->state == SLOT_USED && strcmp(s->key, key) == 0) {
            return i;
        }                                          /* SLOT_DELETED: keep probing */
    }
    return SIZE_MAX;
}

/* Inserts without checking for an existing key or growing. Takes ownership of `key`. */
static void insert_new(Map *m, char *key, int value)
{
    size_t mask = m->capacity - 1;
    size_t i = fnv1a(key) & mask;
    while (m->slots[i].state == SLOT_USED) {
        i = (i + 1) & mask;
    }
    if (m->slots[i].state == SLOT_DELETED) {
        m->tombstones--;
    }
    m->slots[i] = (Slot){key, value, SLOT_USED};
    m->count++;
}

static int grow(Map *m)
{
    Map bigger;
    if (m->capacity > SIZE_MAX / 2 / sizeof(Slot) || map_init_cap(&bigger, m->capacity * 2) != 0) {
        return -1;
    }
    for (size_t i = 0; i < m->capacity; i++) {     /* re-insert: positions depend on capacity */
        if (m->slots[i].state == SLOT_USED) {
            insert_new(&bigger, m->slots[i].key, m->slots[i].value);
        }
    }
    free(m->slots);                                /* keys moved over; free only the array */
    *m = bigger;
    return 0;
}

/* Sets key to value. Returns 0, or -1 if out of memory. */
int map_put(Map *m, const char *key, int value)
{
    size_t i = find_slot(m, key);
    if (i != SIZE_MAX) {
        m->slots[i].value = value;
        return 0;
    }
    if ((m->count + m->tombstones + 1) * 10 > m->capacity * 7 && grow(m) != 0) {   /* load > 0.7 */
        return -1;
    }
    size_t len = strlen(key);
    char *copy = malloc(len + 1);
    if (copy == NULL) {
        return -1;
    }
    memcpy(copy, key, len + 1);
    insert_new(m, copy, value);
    return 0;
}

/* Returns 1 and sets *out if found, 0 otherwise. */
int map_get(const Map *m, const char *key, int *out)
{
    size_t i = find_slot(m, key);
    if (i == SIZE_MAX) {
        return 0;
    }
    *out = m->slots[i].value;
    return 1;
}

/* Returns 1 if the key was removed, 0 if it was not there. */
int map_remove(Map *m, const char *key)
{
    size_t i = find_slot(m, key);
    if (i == SIZE_MAX) {
        return 0;
    }
    free(m->slots[i].key);
    m->slots[i].key = NULL;
    m->slots[i].state = SLOT_DELETED;             /* tombstone keeps later keys reachable */
    m->count--;
    m->tombstones++;
    return 1;
}

static int word_count(void)
{
    Map m;
    if (map_init(&m) != 0) {
        fprintf(stderr, "error: out of memory\n");
        return 1;
    }
    char word[128];
    while (scanf("%127s", word) == 1) {
        int n = 0;
        map_get(&m, word, &n);
        if (map_put(&m, word, n + 1) != 0) {
            fprintf(stderr, "error: out of memory\n");
            map_free(&m);
            return 1;
        }
    }
    const char *best = "";
    int best_n = 0;
    for (size_t i = 0; i < m.capacity; i++) {
        const Slot *s = &m.slots[i];
        if (s->state == SLOT_USED &&
            (s->value > best_n || (s->value == best_n && strcmp(s->key, best) < 0))) {
            best = s->key;                         /* ties: alphabetically first word */
            best_n = s->value;
        }
    }
    printf("unique=%zu most=%s count=%d\n", m.count, best, best_n);
    map_free(&m);
    return 0;
}

/* Slow reference: an array of keys 0..KEYS-1, searched directly. */
#define KEYS 500

static int stress(void)
{
    Map m;
    int ref_present[KEYS] = {0};
    int ref_value[KEYS] = {0};
    if (map_init(&m) != 0) {
        return 1;
    }
    unsigned seed = 2024;
    printf("seed=%u\n", seed);
    srand(seed);
    for (int op = 0; op < 100000; op++) {
        int k = rand() % KEYS;
        char key[16];
        snprintf(key, sizeof key, "k%d", k);
        int action = rand() % 3;
        if (action == 0) {
            int v = rand();
            if (map_put(&m, key, v) != 0) {
                map_free(&m);
                return 1;
            }
            ref_present[k] = 1;
            ref_value[k] = v;
        } else if (action == 1) {
            int got = 0;
            int found = map_get(&m, key, &got);
            if (found != ref_present[k] || (found && got != ref_value[k])) {
                printf("MISMATCH at op %d for %s\n", op, key);
                map_free(&m);
                return 1;
            }
        } else {
            if (map_remove(&m, key) != ref_present[k]) {
                printf("MISMATCH on remove at op %d for %s\n", op, key);
                map_free(&m);
                return 1;
            }
            ref_present[k] = 0;
        }
    }
    printf("stress test passed (count=%zu capacity=%zu)\n", m.count, m.capacity);
    map_free(&m);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc == 2 && strcmp(argv[1], "--stress") == 0) {
        return stress();
    }
    if (argc == 1) {
        return word_count();
    }
    fprintf(stderr, "usage: hashmap [--stress]\n");
    return 2;
}
