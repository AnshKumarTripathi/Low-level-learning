/* Solution: C 06 Exercise 4 — BST and the sorted-input trap. Attempt the exercise first.
 * Insert, contains, height and free are all iterative, so a degenerate tree of
 * 10,000 nodes cannot overflow the call stack.
 */
#include <stdio.h>
#include <stdlib.h>

typedef struct Tree {
    int key;
    struct Tree *left, *right;
} Tree;

/* Returns 1 if inserted, 0 if already present, -1 if out of memory. */
static int tree_insert(Tree **root, int key)
{
    Tree **link = root;
    while (*link != NULL) {
        if (key == (*link)->key) {
            return 0;
        }
        link = (key < (*link)->key) ? &(*link)->left : &(*link)->right;
    }
    Tree *n = malloc(sizeof *n);
    if (n == NULL) {
        return -1;
    }
    *n = (Tree){key, NULL, NULL};
    *link = n;
    return 1;
}

static int tree_contains(const Tree *t, int key)
{
    while (t != NULL) {
        if (key == t->key) {
            return 1;
        }
        t = (key < t->key) ? t->left : t->right;
    }
    return 0;
}

/* Height = number of nodes on the longest root-to-leaf path. Uses an explicit stack. */
static size_t tree_height(const Tree *root, size_t count)
{
    if (root == NULL) {
        return 0;
    }
    const Tree **nodes = malloc(count * sizeof *nodes);
    size_t *depth = malloc(count * sizeof *depth);
    size_t best = 0;
    if (nodes != NULL && depth != NULL) {
        size_t top = 0;
        nodes[top] = root;
        depth[top++] = 1;
        while (top > 0) {
            top--;
            const Tree *t = nodes[top];
            size_t d = depth[top];
            if (d > best) {
                best = d;
            }
            if (t->left) {
                nodes[top] = t->left;
                depth[top++] = d + 1;
            }
            if (t->right) {
                nodes[top] = t->right;
                depth[top++] = d + 1;
            }
        }
    }
    free(nodes);
    free(depth);
    return best;
}

/* Frees every node without recursion: rotate left children up until none remain. */
static void tree_free(Tree **root)
{
    Tree *t = *root;
    while (t != NULL) {
        if (t->left != NULL) {
            Tree *l = t->left;
            t->left = l->right;
            l->right = t;
            t = l;
        } else {
            Tree *right = t->right;
            free(t);
            t = right;
        }
    }
    *root = NULL;
}

static void print_in_order(const Tree *t, int limit)   /* recursion is fine for small trees */
{
    if (t == NULL || limit <= 0) {
        return;
    }
    print_in_order(t->left, limit);
    printf("%d ", t->key);
    print_in_order(t->right, limit);
}

static size_t build(Tree **root, int sorted)
{
    size_t inserted = 0;
    srand(7);
    for (int i = 0; i < 10000; i++) {
        int key = sorted ? i : (rand() % 32768) * 32768 + rand() % 32768;
        int r = tree_insert(root, key);
        if (r < 0) {
            fprintf(stderr, "error: out of memory\n");
            exit(1);
        }
        inserted += (size_t)r;
    }
    return inserted;
}

int main(void)
{
    Tree *small = NULL;
    const int keys[] = {5, 3, 8, 1, 4, 9};
    for (size_t i = 0; i < sizeof keys / sizeof keys[0]; i++) {
        tree_insert(&small, keys[i]);
    }
    printf("in-order: ");
    print_in_order(small, 1);
    printf("\ncontains 4: %d, contains 7: %d\n", tree_contains(small, 4), tree_contains(small, 7));
    tree_free(&small);

    Tree *random_tree = NULL;
    size_t n1 = build(&random_tree, 0);
    printf("random height=%zu (nodes=%zu)\n", tree_height(random_tree, n1), n1);
    tree_free(&random_tree);

    Tree *sorted_tree = NULL;
    size_t n2 = build(&sorted_tree, 1);
    printf("sorted height=%zu (nodes=%zu)\n", tree_height(sorted_tree, n2), n2);
    tree_free(&sorted_tree);
    return 0;
}
