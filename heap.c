/*************************************************************************
	> File Name: heap.c
	> Author: seabreeze
	> Mail: 233@233.com
	> Created Time: Wed 12 Oct 2016 11:51:02 PM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include"heap.h"

#ifndef IS_32_BITS
#define SIZE_SZ 8
#define SMALLBIN_MAX 0x400
#else 
#define SIZE_SZ 4
#define SMALLBIN_MAX 0x200
#endif

struct chunk
{
    INTERNAL_SIZE_T      prev_size;   /* Size of previous chunk (if free).  */
    INTERNAL_SIZE_T      size;        /* Size in bytes, including overhead. */
      
    struct chunk* fd;                 /* double links -- used only if free. */
    struct chunk* bk;
      
    /* Only used for large blocks: pointer to next larger size.  */
    struct chunk* fd_nextsize;        /* double links -- used only if free. */
    struct chunk* bk_nextsize;
}

/*
 * MALLOC_ALIGNMENT is the minimum alignment for malloc'ed chunks.
 * It must be a power of two at least 2 * SIZE_SZ, even on machines
 * for which smaller alignments would suffice. It may be defined as
 * larger than this though. Note however that code and data structures
 * are optimized for the case of 8-byte alignment.
 */

#ifndef MALLOC_ALIGNMENT
# define MALLOC_ALIGNMENT       (2 * SIZE_SZ < __alignof__ (long double)  ? \
                  __alignof__ (long double) : 2 * SIZE_SZ)
#endif

/* The corresponding bit mask value */
#define MALLOC_ALIGN_MASK      (MALLOC_ALIGNMENT - 1)

/* The smallest possible chunk */

#define MIN_CHUNK_SIZE        (offsetof(struct chunk, fd_nextsize)) // 0x20 in 64-bit system.

/* The smallest size we can malloc is an aligned minimal chunk */

#define MINSIZE  \
   (unsigned long)(((MIN_CHUNK_SIZE+MALLOC_ALIGN_MASK) & ~MALLOC_ALIGN_MASK)) // 0x20 in 64-bit system.

/* pad request bytes into a usable size -- internal version 
   Only add SIZE_SZ * 1 here because the prev_size of next chunk is used. */

#define request2size(req)                                         \
   (((req) + SIZE_SZ + MALLOC_ALIGN_MASK < MINSIZE)  ?             \
    MINSIZE :                                                      \
    ((req) + SIZE_SZ + MALLOC_ALIGN_MASK) & ~MALLOC_ALIGN_MASK)

/* The maximum fastbin request size we support */
#define MAX_FAST_SIZE     (80 * SIZE_SZ / 4)

#define NFASTBINS  (fastbin_index (request2size (MAX_FAST_SIZE)) + 1)

void show_chunk(void *chunk_ptr, int free_flag)
{
    unsigned long int chunk_size = *(unsigned long int*)(chunk_ptr - SIZE_SZ);
    unsigned long int true_size = chunk_size & (~(unsigned long int)7);
    unsigned long int prev_chunk_size = *(unsigned long int*)(chunk_ptr - SIZE_SZ * 2);
    unsigned long int fd = *(unsigned long int*)(chunk_ptr);
    unsigned long int bk = *(unsigned long int*)(chunk_ptr + SIZE_SZ);
    unsigned long int fd_nextsize = *(unsigned long int*)(chunk_ptr + SIZE_SZ * 2);
    unsigned long int bk_nextsize = *(unsigned long int*)(chunk_ptr + SIZE_SZ * 3);
    puts("------------------------------------------");
    printf("Chunk position: \033[33m0x%lx\033[0m\n", (unsigned long)chunk_ptr - SIZE_SZ * 2);
    printf("Chunk size: \033[33m0x%lx\033[0m\n", true_size);

    puts("Show n_m_p flag:");
    printf("\tIn_use flag set? ");
    if (chunk_size & 1)
        puts("\033[32mYes\033[0m");
    else
    {
        puts("\033[31mNo\033[0m");
        printf("\tThe previous chunk size: \033[33m0x%lx\033[0m\n", prev_chunk_size);
    }
   
    printf("\tThe chunk mmaped? ");
    if (chunk_size & 2)
        puts("\033[32mYes\033[0m");
    else
        puts("\033[31mNo\033[0m");

    printf("\tThe chunk belongs to main_arena? ");
    if (chunk_size & 4)
        puts("\033[31mNo\033[0m");
    else 
        puts("\033[32mYes\033[0m");

    if (free_flag)
    {
        puts("Show Free Situation: ");
        if (chunk_size <= MAX_FAST_SIZE) // This threshold is determined by the global varible "max fastbin size" in libc.
            printf("\tFD(fastbin): \033[33m0x%lx\033[0m\n", fd);
        else
        {
            printf("\tFD(normalbin): \033[33m0x%lx\033[0m\n", fd);
            printf("\tBK(normalbin): \033[33m0x%lx\033[0m\n", bk);
        }
        if (chunk_size > SMALLBIN_MAX)
        {
            printf("\tIf this chunk is in large bins: \n");
            printf("\t\tFD_NEXTSIZE: \033[33m0x%lx\033[0m\n", fd_nextsize);
            printf("\t\tBK_NEXTSIZE: \033[33m0x%lx\033[0m\n", bk_nextsize);
        }
    }
    puts("------------------------------------------");
    puts("");
}

struct malloc_state
{
    pthread_mutex_t mutex;
    chunk *fastbinsY[NFASTBINS];
};

int main()
{
    void *a = malloc(0x20);
    void *b = malloc(0x20);
    show_chunk(a, 0);
    free(a);
    free(b);
    show_chunk(a, 1);
    show_chunk(b, 1);
    void *c = malloc(0x100);
    void *d = malloc(0x500);
    show_chunk(c, 0);
    show_chunk(d, 0);
    free(c);
    free(d);
    show_chunk(c, 1);
    show_chunk(d, 1);

    return 0;
}
