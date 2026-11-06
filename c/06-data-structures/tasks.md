# 06 — Data Structures in C

[← 05 — Data representation](../05-data-representation-bitwise/tasks.md) · [Roadmap](../../ROADMAP.md) · [Next: 07 — Binary files →](../07-binary-files-parsing/tasks.md)

> Time: about 20–25 hours · Prerequisites: C 03 (typedef, function pointers), C 04 (ownership, growable array), C 05 (bits) · You will build: a linked list, a hash map, a binary heap, and a binary search tree, each with tests.

## Learning objectives

By the end of this chapter you will be able to:
- Estimate the cost of an operation with Big-O notation.
- Implement a linked list, using a pointer-to-pointer to insert and remove without special cases.
- Implement a hash map with a good string hash and a growth policy.
- Implement a binary heap as an array and use it as a priority queue.
- Implement a binary search tree and explain when it becomes slow.
- Implement insertion sort, merge sort and binary search, and measure O(n²) against O(n log n).
- Test data structures with `assert` and invariant checks.

## Why it matters

Every program is data structures plus the code that walks them. Building them by hand once shows what the C++ standard library (B4) does for you, what each operation really costs, and how memory layout affects speed — which chapter 13 measures.

## Session plan

This chapter is long. Work through it in 7 sittings; each one ends with a stop marker.

| Session | Do |
|---------|----|
| 1 | Learn §1–3 |
| 2 | Learn §4–7 |
| 3 | Exercise 1 — Linked list toolkit |
| 4 | Exercise 2 — Hash map (string → int) |
| 5 | Exercise 3 — Priority queue and top-k |
| 6 | Exercise 4 — BST and the sorted-input trap |
| 7 | Exercise 5 — Sort race, then the quiz and recap |

---

## Learn

### 1. Big-O in one table

Big-O describes how the work grows with the number of elements `n`, ignoring constant factors.

| Big-O | Name | Example | `n` = 1,000,000 → about |
|-------|------|---------|-------------------------|
| O(1) | constant | array index, hash map lookup (average) | 1 step |
| O(log n) | logarithmic | binary search, heap push/pop, balanced tree | 20 steps |
| O(n) | linear | scan a list | 1,000,000 steps |
| O(n log n) | | good sorting (`qsort`) | 20,000,000 steps |
| O(n²) | quadratic | comparing every pair | 10¹² steps — too slow |

| Structure | Find | Insert | Remove | Memory layout |
|-----------|------|--------|--------|---------------|
| dynamic array (C 04) | O(n) | O(1) at end (amortised) | O(n) in middle | contiguous — cache-friendly |
| sorted array | O(log n) | O(n) | O(n) | contiguous |
| linked list | O(n) | O(1) at a known position | O(1) at a known position | scattered nodes |
| hash map | O(1) average | O(1) average | O(1) average | array of buckets |
| binary heap | O(1) for the min | O(log n) | O(log n) for the min | contiguous array |
| binary search tree | O(log n) if balanced, O(n) worst | same | same | scattered nodes |

### 2. Linked lists and the pointer-to-pointer trick

```text
head ──► [3 | next] ──► [7 | next] ──► [9 | NULL]
```

Removing a node normally needs a special case for the head. Walking with a **pointer to the `next` field** (`Node **link`) removes the special case: `*link` is either `head` or some node's `next`, and both are changed the same way.

```c
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int value;
    struct Node *next;
} Node;

/* Inserts value keeping the list sorted. Returns 0, or -1 if out of memory. */
int list_insert_sorted(Node **head, int value)
{
    Node **link = head;
    while (*link != NULL && (*link)->value < value) {
        link = &(*link)->next;               /* advance to the next `next` field */
    }
    Node *n = malloc(sizeof *n);
    if (n == NULL) {
        return -1;
    }
    n->value = value;
    n->next = *link;
    *link = n;                               /* works for the head and the middle alike */
    return 0;
}

/* Removes the first node with `value`. Returns 1 if removed, 0 if not found. */
int list_remove(Node **head, int value)
{
    for (Node **link = head; *link != NULL; link = &(*link)->next) {
        if ((*link)->value == value) {
            Node *dead = *link;
            *link = dead->next;              /* unlink */
            free(dead);
            return 1;
        }
    }
    return 0;
}

void list_free(Node **head)
{
    Node *n = *head;
    while (n != NULL) {
        Node *next = n->next;                /* read next BEFORE freeing n */
        free(n);
        n = next;
    }
    *head = NULL;
}

void list_print(const Node *n)
{
    for (; n != NULL; n = n->next) {
        printf("%d ", n->value);
    }
    printf("\n");
}

int main(void)
{
    Node *head = NULL;
    int input[] = {7, 3, 9, 1, 5};
    for (size_t i = 0; i < sizeof input / sizeof input[0]; i++) {
        if (list_insert_sorted(&head, input[i]) != 0) {
            list_free(&head);
            return 1;
        }
    }
    list_print(head);                        /* 1 3 5 7 9 */

    int removed_head = list_remove(&head, 1);
    int removed_tail = list_remove(&head, 9);
    int removed_none = list_remove(&head, 42);
    assert(removed_head == 1);               /* never put the call itself inside assert: */
    assert(removed_tail == 1);               /* release builds (NDEBUG) delete the whole line */
    assert(removed_none == 0);
    (void)removed_head;                      /* avoid "unused variable" warnings when */
    (void)removed_tail;                      /* NDEBUG removes the asserts */
    (void)removed_none;
    list_print(head);                        /* 3 5 7 */

    list_free(&head);
    assert(head == NULL);
    return 0;
}
```

`assert(condition)` from `<assert.h>` stops the program with the file and line if the condition is false. It is compiled out when `NDEBUG` is defined (release builds), so use it for **tests and invariants**, never for checking user input — and never put a call with side effects inside it, because the call disappears too.

Linked lists look attractive on paper, but their nodes are scattered across the heap, so walking one misses the CPU cache at almost every step. Chapter 13 measures this: for most workloads, a dynamic array wins even at "inserting in the middle".

### 3. Hash maps

A hash map stores key–value pairs in an array of **buckets**. A **hash function** turns the key into a number; `hash % capacity` picks the bucket.

```text
"cat" ── hash ──► 0x06745c07 ── % 8 ──► bucket 7
"pig" ── hash ──► 0x6840fb8f ── % 8 ──► bucket 7   ← collision!   (real FNV-1a values)
```

Two ways to handle **collisions**:
- **Chaining**: each bucket holds a linked list of entries.
- **Open addressing** (linear probing): if a bucket is taken, try the next one, and the next, until a free bucket appears.

A good string hash spreads keys evenly. FNV-1a is simple and good enough for this course:

```c
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* 32-bit FNV-1a: XOR in each byte, then multiply by the FNV prime. */
uint32_t fnv1a(const char *s)
{
    uint32_t h = 2166136261u;                /* offset basis */
    for (const unsigned char *p = (const unsigned char *)s; *p != '\0'; p++) {
        h ^= *p;
        h *= 16777619u;                      /* FNV prime; unsigned math wraps, which is intended */
    }
    return h;
}

int main(void)
{
    const char *keys[] = {"", "a", "foobar", "cat", "pig"};
    for (size_t i = 0; i < sizeof keys / sizeof keys[0]; i++) {
        uint32_t h = fnv1a(keys[i]);
        printf("%-8s hash=%08x bucket(of 8)=%u\n", keys[i], (unsigned)h, (unsigned)(h % 8));
    }
    return 0;
}
```

Known values to test against: `fnv1a("") = 811c9dc5`, `fnv1a("a") = e40c292c`, `fnv1a("foobar") = bf9cf968`.

The **load factor** is `count / capacity`. As it rises, collisions multiply. Grow the table (double the capacity and re-insert every entry) when the load factor passes about 0.7 for open addressing.

> **Session 1 of 7 complete: a good place to stop.** Check: close this page and explain in your own words: Big-O in one table; Linked lists and the pointer-to-pointer trick; Hash maps. Stuck on one? Re-read it before moving on.

### 4. Binary heap: a priority queue in an array

A **min-heap** is a tree where every parent is ≤ its children, stored in an array with no pointers:

```text
            1                 index:  0  1  2  3  4  5
          /   \               value: [1, 3, 2, 7, 4, 5]
         3     2
        / \   /               children of i: 2i+1 and 2i+2
       7   4 5                parent of i:   (i-1)/2
```

- **push**: put the new value at the end, then swap it with its parent while it is smaller ("sift up").
- **pop**: take the root, move the last element to the root, then swap it with its smaller child while it is larger ("sift down").

Both are O(log n), and the smallest element is always at index 0. Uses: schedulers, Dijkstra's shortest paths, "top k" problems, timers.

### 5. Binary search trees

Each node has a left subtree with smaller keys and a right subtree with larger keys, so a search goes left or right at each step: O(height). If keys arrive in random order, the height is about `2 log n`. If they arrive **sorted**, every node goes to the right and the tree becomes a linked list: O(n). Balanced trees (red-black, AVL — used by `std::map`) fix that by rotating nodes; they are beyond this chapter, but you will measure the problem in Exercise 4.

### 6. Testing data structures

- Write a `check()` function for each structure's invariants (heap order, sorted list, BST ordering, `count` matching the real number of entries) and call it after every operation in tests.
- Compare against a simple reference: run the same random operations on your hash map and on a plain array searched linearly; the answers must agree.
- Run tests under sanitizers (ASan on Linux/WSL) — data-structure code is where use-after-free and leaks hide.

### 7. Sorting and searching

`qsort` sorts for you, but knowing **how** sorting works explains the O(n²) vs O(n log n) row of the Big-O table.

**Insertion sort** — the way you sort playing cards in your hand. Take the next card and slide it left past every bigger card:

```text
[5 2 4 1]  take 2: slide past 5      → [2 5 4 1]
[2 5 4 1]  take 4: slide past 5      → [2 4 5 1]
[2 4 5 1]  take 1: slide past 5,4,2  → [1 2 4 5]
```

Each card may slide past all the cards before it, so the total work grows like n²: O(n²). It is fast on small or almost-sorted arrays, and real libraries use it for small pieces of bigger sorts.

**Merge sort** — recursion (C 00 Module E) at work. Split the array in half, sort each half (by calling yourself), then **merge** the two sorted halves by repeatedly taking the smaller front element:

```text
[5 2 4 1 3 6]
split   → [5 2 4]         [1 3 6]
sort    → [2 4 5]         [1 3 6]
merge   → 1 2 3 4 5 6     (compare fronts: 2 vs 1 → 1; 2 vs 3 → 2; 4 vs 3 → 3; …)
```

There are about log₂ n levels of splitting, and each level does n steps of merging: O(n log n). Merging needs a scratch buffer of n elements — a classic trade of memory for speed.

**Binary search** — on a **sorted** array, look at the middle; the key is either there, or in the left half, or in the right half. Each step halves the range, exactly like the guessing game in C 00 Module D: O(log n), so 20 steps find one value in a million.

```c
#include <stdio.h>

/* Returns the index of key in sorted a[0..n), or -1. */
long binary_search(const int *a, size_t n, int key)
{
    size_t lo = 0, hi = n;               /* the key can only be in a[lo..hi) */
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2; /* not (lo + hi) / 2: that can overflow */
        if (a[mid] < key) {
            lo = mid + 1;
        } else if (a[mid] > key) {
            hi = mid;
        } else {
            return (long)mid;
        }
    }
    return -1;
}

int main(void)
{
    int a[] = {2, 3, 5, 7, 11, 13, 17};
    size_t n = sizeof a / sizeof a[0];
    printf("%ld %ld %ld\n", binary_search(a, n, 11), binary_search(a, n, 2), binary_search(a, n, 4));
    return 0;
}
```

Output: `4 0 -1`. Binary search is famous for off-by-one bugs: keep one rule (here, "the key is in `a[lo..hi)`, `hi` excluded") and make every line respect it. The standard library has it too: `bsearch`, which takes the same comparator as `qsort`.

> **Session 2 of 7 complete: a good place to stop.** Check: close this page and explain in your own words: Binary heap: a priority queue in an array; Binary search trees; Testing data structures; Sorting and searching. Stuck on one? Re-read it before moving on.

---

## Exercises

## Exercise 1 — Linked list toolkit
### Task
Extend the list from Learn §2 with `list_reverse` (in place, no allocation), `list_length`, `list_find` (returns the node or `NULL`), and `list_remove_all(value)`. Write a test program that uses `assert` for every function, including on an empty list and a one-element list.

### Desired output
- The test program prints `all list tests passed` and exits 0.

### Hints
1. Reverse with three pointers: `prev`, `cur`, `next`. Draw the arrows on paper for a 3-node list first.
2. `list_remove_all` is the `list_remove` loop without the `return` — but do not advance `link` after removing, because `*link` is already the next node.
3. Run the tests under ASan (Linux/WSL) to prove there are no leaks.

> **Session 3 of 7 complete: a good place to stop.** Check: your program gives the desired output and handles the edge cases of this exercise.

## Exercise 2 — Hash map (string → int)
### Task
Implement an open-addressing hash map with FNV-1a and linear probing: `map_init`, `map_free`, `map_put`, `map_get`, `map_remove`. Keys are copied into the map (it owns them). Grow when the load factor would exceed 0.7.

### Desired output
- A word-count program: read words from stdin, count them in the map, and print `unique=<n> most=<word> count=<k>`.
- A stress test: 100,000 random puts, gets and removes compared against a slow reference; prints `stress test passed`.

### Hints
1. Each slot has a state: empty, used, or **deleted** ("tombstone"). A lookup must keep probing past tombstones, or keys inserted after a removed one become unreachable.
2. Probe with `i = (i + 1) & (capacity - 1)` and keep `capacity` a power of two.
3. When growing, re-insert every used entry into the new array; do not copy slots one-to-one.
4. Use `strdup`-style copying: `malloc(strlen(key) + 1)` and `memcpy`. `map_free` frees every key.

> **Session 4 of 7 complete: a good place to stop.** Check: your program gives the desired output and handles the edge cases of this exercise.

## Exercise 3 — Priority queue and top-k
### Task
Implement a min-heap of `int` on a growable array (`heap_push`, `heap_pop`, `heap_peek`, `heap_size`). Use it to print the `k` largest numbers from stdin in descending order, using only O(k) memory.

### Desired output
- For `k = 3` and input `5 1 9 3 7 2 8`: `top=9 8 7`.

### Hints
1. Keep a min-heap of at most `k` elements. For each new number larger than the heap's minimum, pop the minimum and push the new number.
2. At the end, pop everything into an array and print it in reverse.
3. Write `heap_check` that verifies `a[(i-1)/2] <= a[i]` for every `i > 0`.

> **Session 5 of 7 complete: a good place to stop.** Check: your program gives the desired output and handles the edge cases of this exercise.

## Exercise 4 — BST and the sorted-input trap
### Task
Implement a binary search tree of `int` keys with insert, contains, in-order print, height, and free. Insert 10,000 keys in random order, then 10,000 keys in sorted order, and print the height in each case.

### Desired output
- `random height=<h1>` (expect roughly 25–35)
- `sorted height=10000`

### Hints
1. Insert with the pointer-to-pointer trick from §2: `Node **link = &root;` then go left or right.
2. Write height iteratively or watch the recursion depth: recursing 10,000 levels deep may overflow the stack on some systems (another lesson).
3. Free the tree with a post-order traversal (children first, then the node).

**Milestone:** after this chapter, build [M2 — `hashmap`](../../milestones/README.md#m2--hashmap-a-reusable-hash-map).

> **Session 6 of 7 complete: a good place to stop.** Check: your program gives the desired output and handles the edge cases of this exercise.

## Exercise 5 — Sort race
### Task
Implement `insertion_sort` and `merge_sort` for `int` arrays (merge sort allocates one scratch buffer and frees it), plus `binary_search` from Learn §7. Then race them:
- For n = 1,000, 10,000 and 100,000, fill an array with random values (`srand(42)` so runs repeat).
- Sort a copy with each algorithm and with `qsort`. Check that your results match `qsort`'s byte for byte (`memcmp`).
- Time each sort with `clock()`. Skip insertion sort at 100,000 and print `skipped`.
- Search every original value in the sorted array with `binary_search`; all must be found. Search one value that is not there; it must return `-1`.

### Desired output
One block per size, for example:

```text
n=10000
  insertion 0.0200 s ok
  merge     0.0020 s ok
  qsort     0.0030 s
  search    10000/10000 found, missing key -> -1
```

Your times will differ. Build with `-O2` for meaningful numbers.

### Hints
1. Insertion sort: use `size_t j` and test `j > 0` **before** reading `a[j - 1]`, so `j` never goes below zero.
2. Merge sort: write a helper `merge_sort_rec(a, tmp, lo, hi)` that sorts `a[lo..hi)`; the base case is fewer than 2 elements.
3. Predict before you run: if n grows 10×, an O(n²) sort gets about 100× slower and an O(n log n) one a bit over 10×. Do your numbers agree?
4. Stretch: sort an array that is **already sorted**. Which algorithm wins now, and why? (Compare Exercise 4's sorted-input trap.)

## Checkpoint quiz

1. What is the Big-O cost of finding a value in an unsorted linked list? In a hash map, on average?
2. Why does `Node **link` remove the special case for the head?
3. Why must `list_free` read `n->next` before calling `free(n)`?
4. What is a hash collision? Name two ways to handle it.
5. What is the load factor, and why grow the table when it gets high?
6. Why does open addressing need "deleted" markers?
7. In an array heap, where are the children of index `i`?
8. Why is a BST fed sorted keys slow?
9. Why should `assert` not be used to check user input?
10. Why can a dynamic array beat a linked list even for inserts in the middle?
11. Why is insertion sort O(n²) but merge sort O(n log n)?
12. What must be true of an array before binary search works on it?

## Recap

- Big-O tells you how work grows; memory layout decides the constant factor.
- Pointer-to-pointer walks make list insert/remove uniform.
- Hash maps: good hash, collision strategy, growth by load factor, tombstones for open addressing.
- Heaps give a priority queue in a plain array; BSTs need balancing to stay fast.
- Insertion sort is O(n²) but good for small inputs; merge sort is O(n log n) with an n-element buffer; binary search needs sorted data and is O(log n).
- Test with `assert`, invariant checks, reference comparisons and sanitizers.

## Spaced review

1. (C 04) Why must `realloc` go into a temporary pointer?
2. (C 03) What does a `qsort` comparator return?
3. (C 05) What does `x & (capacity - 1)` equal when `capacity` is a power of two?

## Answers

Quiz: 1. O(n); O(1). 2. `*link` is the head pointer itself on the first step and a node's `next` field later, so the same assignment works for both. 3. After `free(n)`, reading `n->next` is a use-after-free. 4. Two keys landing in the same bucket; chaining or open addressing (probing). 5. `count / capacity`; collisions and probe lengths grow quickly as it approaches 1. 6. Removing an entry outright would break the probe chain for keys inserted after it. 7. `2i + 1` and `2i + 2`. 8. Every key goes to the same side, so the tree degenerates into a list of height `n`. 9. It disappears in release builds (`NDEBUG`), and it aborts instead of reporting a helpful error. 10. Shifting contiguous memory is very fast and cache-friendly, while walking a list to the insertion point misses the cache at nearly every node. 11. Each insertion may shift every earlier element (n × n); merge sort has about log n levels of splitting, each merging n elements. 12. It must be sorted (by the same order the search compares with).

Spaced review: 1. If it fails it returns `NULL` and the old block must still be freed. 2. Negative, zero or positive for less, equal, greater. 3. `x % capacity`.

## References (concept-only)
- [cppreference — assert](https://en.cppreference.com/w/c/error/assert)
- [FNV hash (description and test values)](http://www.isthe.com/chongo/tech/comp/fnv/index.html)
- Optional books: *Algorithms* (Sedgewick & Wayne) for the data structures; *The Algorithm Design Manual* (Skiena) for choosing between them
- Optional practice: implement the same structures in C++ (B4) and compare with `std::unordered_map`, `std::priority_queue` and `std::map`
