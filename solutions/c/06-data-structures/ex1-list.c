/* Solution: C 06 Exercise 1 — Linked list toolkit with tests. Attempt the exercise first. */
#undef NDEBUG              /* this is a test program: its asserts must run in every build */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int value;
    struct Node *next;
} Node;

/* Returns 0, or -1 if out of memory. */
static int list_push_front(Node **head, int value)
{
    Node *n = malloc(sizeof *n);
    if (n == NULL) {
        return -1;
    }
    n->value = value;
    n->next = *head;
    *head = n;
    return 0;
}

static size_t list_length(const Node *n)
{
    size_t len = 0;
    for (; n != NULL; n = n->next) {
        len++;
    }
    return len;
}

static Node *list_find(Node *n, int value)
{
    for (; n != NULL; n = n->next) {
        if (n->value == value) {
            return n;
        }
    }
    return NULL;
}

/* Reverses in place by turning every `next` arrow around. */
static void list_reverse(Node **head)
{
    Node *prev = NULL;
    Node *cur = *head;
    while (cur != NULL) {
        Node *next = cur->next;    /* remember the rest of the list */
        cur->next = prev;          /* turn this arrow around */
        prev = cur;
        cur = next;
    }
    *head = prev;
}

/* Removes every node holding `value`. Returns how many were removed. */
static size_t list_remove_all(Node **head, int value)
{
    size_t removed = 0;
    Node **link = head;
    while (*link != NULL) {
        if ((*link)->value == value) {
            Node *dead = *link;
            *link = dead->next;    /* do not advance: *link is already the next node */
            free(dead);
            removed++;
        } else {
            link = &(*link)->next;
        }
    }
    return removed;
}

static void list_free(Node **head)
{
    Node *n = *head;
    while (n != NULL) {
        Node *next = n->next;
        free(n);
        n = next;
    }
    *head = NULL;
}

/* Builds a list from an array (values end up in the same order). */
static Node *list_from(const int *v, size_t n)
{
    Node *head = NULL;
    for (size_t i = n; i > 0; i--) {
        if (list_push_front(&head, v[i - 1]) != 0) {
            list_free(&head);
            return NULL;
        }
    }
    return head;
}

static int list_equals(const Node *n, const int *v, size_t len)
{
    for (size_t i = 0; i < len; i++, n = n->next) {
        if (n == NULL || n->value != v[i]) {
            return 0;
        }
    }
    return n == NULL;
}

int main(void)
{
    /* empty list */
    Node *empty = NULL;
    list_reverse(&empty);
    assert(empty == NULL);
    assert(list_length(empty) == 0);
    assert(list_find(empty, 1) == NULL);
    assert(list_remove_all(&empty, 1) == 0);

    /* one element */
    const int one[] = {7};
    Node *single = list_from(one, 1);
    assert(single != NULL);
    list_reverse(&single);
    assert(list_equals(single, one, 1));
    size_t removed_single = list_remove_all(&single, 7);
    assert(removed_single == 1 && single == NULL);

    /* several elements */
    const int start[] = {1, 2, 3, 2, 2, 4};
    Node *list = list_from(start, 6);
    assert(list != NULL);
    assert(list_length(list) == 6);
    assert(list_find(list, 3) != NULL && list_find(list, 3)->value == 3);
    assert(list_find(list, 9) == NULL);

    list_reverse(&list);
    const int reversed[] = {4, 2, 2, 3, 2, 1};
    assert(list_equals(list, reversed, 6));

    size_t removed = list_remove_all(&list, 2);    /* includes two neighbours in a row */
    const int without_2[] = {4, 3, 1};
    assert(removed == 3);
    assert(list_equals(list, without_2, 3));

    size_t removed_head = list_remove_all(&list, 4); /* the head */
    assert(removed_head == 1);
    assert(list_length(list) == 2);

    list_free(&list);
    assert(list == NULL);

    printf("all list tests passed\n");
    return 0;
}
