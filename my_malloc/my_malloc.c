#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include <stdint.h>

struct arena_manager {
    void* start;
    void* current;
    size_t size;
    struct arena_manager* next;
};
struct header {
    bool used;
    void* start;
    size_t size;
    struct arena_manager* arena;
};

static struct arena_manager* head = NULL;

size_t align8(size_t bytes_amount) {
    if (!bytes_amount) {
        return 0;
    }
    size_t amount_off_8 = bytes_amount % 8;
    if (!amount_off_8) {
        return bytes_amount;
    }
    amount_off_8 = 8 - amount_off_8;
    return bytes_amount + amount_off_8;
}

void* my_malloc(size_t size) {
    struct header *header;
    size_t amount_mapped = 4096; //4KB page size I think || possibly change it so smth bigger than that can be allocated
    size = align8(size);
    if (!head) { // if no arenas path
        printf("getting head\n");
        char* pointer = mmap(NULL, amount_mapped, PROT_WRITE|PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (pointer == MAP_FAILED) {
            perror("mmap ");
            printf("%i\n", errno);
            return NULL;
        }
        // this sets up the first arena and gives the user the pointer cuz it does the header too
        struct arena_manager* arena = (struct arena_manager *)pointer;
        head = arena;
        arena->start = pointer + sizeof(struct arena_manager);
        arena->size = amount_mapped - sizeof(struct arena_manager);
        arena->current = arena->start;

        header = (struct header *)arena->current;

        header->used = true;
        header->start = (char *)arena->current + sizeof(struct header);
        header->size = size;
        arena->current = (char*)arena->current + sizeof(struct header) + size;
        return header->start;
    }
    struct arena_manager *arena = head;
    struct arena_manager* current = arena;
    struct arena_manager* arena_pointer = NULL;
    struct arena_manager* last_correct_arena = NULL;
    while (current) { // looks for space in existing arenas
        printf("checking arena %p remaining %ld\n",
       current,
       (char*)current->start + current->size - (char*)current->current);
        last_correct_arena = current;
        if ((char*)current->start + current->size - (char*)current->current >= size + sizeof(struct header)) {
            arena_pointer = current;
            break;
        }
        current = current->next;
    }

    if (!arena_pointer) { // makes a new arena if there isnt space in existing ones
        char* pointer = mmap(NULL, amount_mapped, PROT_WRITE|PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (pointer == MAP_FAILED) {
            perror("mmap ");
            printf("%i\n", errno);
            return NULL;
        }
        printf("[NEW] made an arena at %p of size %zu \n", pointer, amount_mapped);
        // this like connects the arena to the linked list and gives the pointer to the user
        arena = last_correct_arena;
        struct arena_manager* arena_new = (struct arena_manager *)pointer;
        arena->next = arena_new;
        arena_new->start = pointer + sizeof(struct arena_manager);
        arena_new->size = amount_mapped - sizeof(struct arena_manager);
        arena_new->current = arena_new->start;

        header = (struct header *)arena->current;

        header->arena = arena_new;

        header->used = true;
        header->start = pointer + sizeof(struct header) + sizeof(struct arena_manager);
        header->size = size;
        arena_new->current = (char*)arena_new->current + sizeof(struct header) + size;
        return header->start;
    }
    // this is if space is found in an arena just makes the structs and gives the pointer
    header = (struct header *)arena_pointer->current;

    header->used = true;
    header->start = (char *)arena_pointer->current + sizeof(struct header);
    header->arena = arena_pointer;
    header->size = size;
    arena_pointer->current = (char*)arena_pointer->current + sizeof(struct header) + size;
    return header->start;
}