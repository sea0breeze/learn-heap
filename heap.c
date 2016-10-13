/*************************************************************************
	> File Name: heap.c
	> Author: seabreeze
	> Mail: 233@233.com
	> Created Time: Wed 12 Oct 2016 11:51:02 PM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include"heap.h"

#ifndef IS_32_BITS
#define SZ_SIZE 8
#define FASTBIN_MAX 0x80
#define SMALLBIN_MAX 0x400
#else 
#define SZ_SIZE 4
#define FASTBIN_MAX 0x40
#define SMALLBIN_MAX 0x200
#endif

void show_chunk(void *chunk_ptr, int free_flag)
{
    unsigned long int chunk_size = *(unsigned long int*)(chunk_ptr - SZ_SIZE);
    unsigned long int true_size = chunk_size & (~(unsigned long int)7);
    unsigned long int prev_chunk_size = *(unsigned long int*)(chunk_ptr - SZ_SIZE * 2);
    unsigned long int fd = *(unsigned long int*)(chunk_ptr);
    unsigned long int bk = *(unsigned long int*)(chunk_ptr + SZ_SIZE);
    unsigned long int fd_nextsize = *(unsigned long int*)(chunk_ptr + SZ_SIZE * 2);
    unsigned long int bk_nextsize = *(unsigned long int*)(chunk_ptr + SZ_SIZE * 3);
    puts("------------------------------------------");
    printf("Chunk position: \033[33m0x%lx\033[0m\n", (unsigned long)chunk_ptr - SZ_SIZE * 2);
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
        if (chunk_size <= FASTBIN_MAX) // This threshold is determined by the global varible "max fastbin size" in libc.
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
