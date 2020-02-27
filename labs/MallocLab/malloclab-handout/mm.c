/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
        /* Team name */
        "bzteam",
        /* First member's full name */
        "Bugen Zhao",
        /* First member's email address */
        "bugenzhao",
        /* Second member's full name (leave blank if none) */
        "",
        /* Second member's email address (leave blank if none) */
        ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define MAX(a, b)   ((a) > (b) ? (a) : (b))

#define WSIZE       4u
#define DSIZE       (WSIZE * 2)
#define CHUNKSIZE   (1u<<12u)

#define ALLOC   1u
#define FREE    0u

#define PACK(size, alloc) ((size)|(alloc))
#define GET(p)      (*(unsigned int*)(p))
#define PUT(p, val) (*(unsigned int*)(p) = (val))

#define GET_SIZE(p)     (GET(p) & ~0x7u)
#define GET_ALLOC(p)    (GET(p) & 0x1u)

#define HDRP(bp)    ((char *)(bp) - WSIZE)
#define FTRP(bp)    ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

// block pointer!
#define NEXT_BLKP(bp)   ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp)   ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

static char *heap_listp = NULL;

static void *extend_heap(size_t words);

static void *coalesce(void *bp);

static void *find_fit(size_t asize);

static void place(void *bp, size_t asize);

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void) {
    if ((heap_listp = mem_sbrk(4 * WSIZE)) == (void *) -1)
        return -1;

    PUT(heap_listp, 0);                                     // padding
    PUT(heap_listp + WSIZE * 1, PACK(DSIZE, ALLOC));    // prologue header
    PUT(heap_listp + WSIZE * 2, PACK(DSIZE, ALLOC));    // prologue footer
    PUT(heap_listp + WSIZE * 3, PACK(0u, ALLOC));   // epilogue header
    heap_listp += WSIZE * 2;

    if (extend_heap(CHUNKSIZE / WSIZE) == NULL)
        return -1;
    return 0;
}

/*
 * mm_malloc
 */
void *mm_malloc(size_t size) {
    size_t asize;
    size_t extendsize;
    char *bp;
    if (size == 0) return NULL;

    if (size <= DSIZE) asize = 2 * DSIZE;
    else asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);  // ceil

    bp = find_fit(asize);
    if (bp != NULL) {
        place(bp, asize);
        return bp;
    }

    extendsize = MAX(asize, CHUNKSIZE);
    bp = extend_heap(extendsize / WSIZE);
    if (bp != NULL) {
        place(bp, asize);
        return bp;
    }

    return NULL;
}

/*
 * mm_free
 */
void mm_free(void *ptr) {
    size_t size = GET_SIZE(HDRP(ptr));
    PUT(HDRP(ptr), PACK(size, FREE));
    PUT(FTRP(ptr), PACK(size, FREE));
    coalesce(ptr);
}

/*
 * mm_realloc
 */
void *mm_realloc(void *ptr, size_t size) {
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;

    if (size == 0) {
        mm_free(ptr);
        return NULL;
    }
    if (ptr == NULL) {
        return mm_malloc(size);
    }

    newptr = mm_malloc(size);
    if (newptr == NULL)
        return NULL;

    copySize = GET_SIZE(HDRP(ptr));
    if (size < copySize) copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}

void *extend_heap(size_t words) {
    char *bp;
    size_t size;

    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;   // dword alignment
    bp = mem_sbrk(size);
    if ((long) bp == -1) return NULL;

    PUT(HDRP(bp), PACK(size, FREE));    // replace epilogue block
    PUT(FTRP(bp), PACK(size, FREE));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0u, ALLOC));  // new epilogue

    return coalesce(bp);    // if the previous block ends with a free block?
}

void *coalesce(void *bp) {
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {                         // a, f, a
        return bp;
    } else if (prev_alloc && !next_alloc) {                 // a, f, f
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, FREE));
        PUT(FTRP(bp), PACK(size, FREE));
    } else if (!prev_alloc && next_alloc) {                 // f, f, a
        size += GET_SIZE(FTRP(PREV_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, FREE));
        PUT(FTRP(bp), PACK(size, FREE));
        bp = PREV_BLKP(bp);
    } else {                                                // f, f, f
        size += GET_SIZE(HDRP(NEXT_BLKP(bp))) + GET_SIZE(FTRP(PREV_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, FREE));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, FREE));
        bp = PREV_BLKP(bp);
    }

    return bp;
}

void *find_fit(size_t asize) {
    void *bp;
    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
        if (GET_ALLOC(HDRP(bp)) == FREE && GET_SIZE(HDRP(bp)) >= asize) return bp;
    }
    return NULL;
}

void place(void *bp, size_t asize) {
    size_t csize = GET_SIZE(HDRP(bp));
    if (csize - asize >= 2 * DSIZE) {
        PUT(HDRP(bp), PACK(asize, ALLOC));
        PUT(FTRP(bp), PACK(asize, ALLOC));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize - asize, FREE));
        PUT(FTRP(bp), PACK(csize - asize, FREE));
    } else {
        PUT(HDRP(bp), PACK(csize, ALLOC));
        PUT(FTRP(bp), PACK(csize, ALLOC));
    }
}