#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "cachelab.h"

typedef struct node {
    int valid;
    unsigned tag;
    struct node* prev;
    struct node* next;
} CacheLine;

typedef struct {
    CacheLine* front;
    CacheLine* end;
    int length;
} CacheSet;

// typedef CacheSet* Cache;

void configureOpt(int argc, char* argv[], int* v, int* s, int* E, int* b, FILE** fp) {
    int opt;
    /* looping over arguments */
    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
        /* determine which argument it’s processing */
        switch (opt) {
            case 'h':
                printf("Usage: ./csim [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n");
                break;
            case 'v':
                *v = 1;
                break;
            case 's':
                *s = atoi(optarg);
                break;
            case 'E':
                *E = atoi(optarg);
                break;
            case 'b':
                *b = atoi(optarg);
                break;
            case 't':
                *fp = fopen(optarg, "r");
                break;
            default:
                printf("Wrong argument, use -h for help.\n");
                exit(1);
        }
    }
}

void removeNode(CacheSet* set, CacheLine* ptr) {
    CacheLine* prev = ptr->prev;
    CacheLine* next = ptr->next;

    if (prev != NULL) {
        prev->next = next;
    } else {
        set->front = next;
    }

    if (next != NULL) {
        next->prev = prev;
    } else {
        set->end = prev;
    }

    set->length--;
}

void addHead(CacheSet* set, CacheLine* ptr) {
    if (set->length == 0) {
        ptr->next = NULL;
        set->end = ptr;
    } else {
        ptr->next = set->front;
        set->front->prev = ptr;
    }

    ptr->prev = NULL;
    set->front = ptr;
    set->length++;
}

void query(CacheSet* cache, int v, int s, int E, int b, unsigned address, int* hits, int* misses, int* evictions) {
    unsigned tag = address >> (s + b);
    unsigned idx = (address >> b) & ((1 << s) - 1);
    CacheLine* ptr = cache[idx].front;
    while (ptr != NULL) {
        if (ptr->valid && (ptr->tag == tag)) {
            (*hits)++;
            if (v == 1) {
                printf(" hit");
            }
            // make current node orphan.
            removeNode(&cache[idx], ptr);
            // move current node to head.
            addHead(&cache[idx], ptr);
            return;
        }
        ptr = ptr->next;
    }

    (*misses)++;
    if (v == 1) {
        printf(" miss");
    }

    if (cache[idx].length < E) {
        ptr = (CacheLine*)malloc(sizeof(CacheLine));
        ptr->valid = 1;
        ptr->tag = tag;
        addHead(&cache[idx], ptr);
    } else {
        (*evictions)++;
        if (v == 1) {
            printf(" eviction");
        }

        ptr = cache[idx].end;
        cache[idx].end = ptr->prev;
        ptr->tag = tag;
        removeNode(&cache[idx], ptr);
        addHead(&cache[idx], ptr);
    }
}

void simulate(int v, int s, int E, int b, FILE* fp, int* hits, int* misses, int* evictions) {
    int S = (1 << s);
    CacheSet cache[S];
    for (int i = 0; i < S; i++) {
        cache[i].front = NULL;
        cache[i].end = NULL;
        cache[i].length = 0;
    }

    char identifier;
    unsigned address;
    int size;
    while (fscanf(fp, " %c %x,%d", &identifier, &address, &size) > 0) {
        if (v == 1) {
            printf("%c %x,%d", identifier, address, size);
        }

        switch (identifier) {
            case 'I':
                break;
            case 'L':
                query(cache, v, s, E, b, address, hits, misses, evictions);
                break;
            case 'M':
                query(cache, v, s, E, b, address, hits, misses, evictions);
            case 'S':
                query(cache, v, s, E, b, address, hits, misses, evictions);
                break;
            default:
                printf("Unknown identifier encountered.\n");
                exit(2);
        }

        if (v == 1) {
            printf("\n");
        }
    }

    fclose(fp);
    for (int i = 0; i < S; i++) {
        CacheLine* q;
        CacheLine* p = cache[i].front;
        while (p != NULL) {
            q = p;
            p = p->next;
            free(q);
        }
    }
}

int main(int argc, char* argv[]) {
    int verbose = 0;
    int s = 0;
    int E = 0;
    int b = 0;
    FILE* fp = NULL;
    int hits = 0;
    int misses = 0;
    int evictions = 0;
    configureOpt(argc, argv, &verbose, &s, &E, &b, &fp);
    simulate(verbose, s, E, b, fp, &hits, &misses, &evictions);
    printSummary(hits, misses, evictions);
    return 0;
}
