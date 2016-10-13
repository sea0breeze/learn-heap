/*************************************************************************
	> File Name: heap.c
	> Author: seabreeze
	> Mail: 233@233.com
	> Created Time: Wed 12 Oct 2016 11:51:02 PM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>

void show_chunk(void *chunk_ptr, int free_flag)
{
    unsigned long int chunk_size = *(unsigned long int*)(chunk_ptr - 8);
    unsigned long int true_size = chunk_size & 0xfffffffffffffff8;
    unsigned long int prev_chunk_size = *(unsigned long int*)(chunk_ptr - 16);
    unsigned long int fd = *(unsigned long int*)(chunk_ptr);
    unsigned long int bk = *(unsigned long int*)(chunk_ptr + 8);
    puts("------------------------------------------");
    printf("Chunk position: 0x%lx\n", (unsigned long)chunk_ptr - 0x10);
    printf("Chunk size: 0x%lx\n", true_size);

    if (true_size > 0x80)
    {
        printf("The previous chunk in-use? ");
        if (chunk_size & 1)
            puts("Yes");
        else
        {
            puts("No");
            printf("The previous chunk size: 0x%lx\n", prev_chunk_size);
        }
    }

    if (free_flag)
    {
        if (chunk_size <= 0x80) // This threshold is determined by the global varible "max fastbin size" in libc.
            printf("FD(fastbin): 0x%lx\n", fd);
        else
        {
            printf("FD(normalbin): 0x%lx\n", fd);
            printf("BK(normalbin): 0x%lx\n", bk);
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

    return 0;
}
