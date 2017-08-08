/* Tar Heels Allocator
kiweber mjlitz do hereby submit lab2 and swear to
have neither given nor recieved unauthothirized aid*/

/* Hard-code some system parameters */
#define SUPER_BLOCK_SIZE 4096
#define SUPER_BLOCK_MASK (~(SUPER_BLOCK_SIZE-1))
#define MIN_ALLOC 32 /* Smallest real allocation.  Round smaller mallocs up */
#define MAX_ALLOC 2048 /* Fail if anything bigger is attempted.
		        * Challenge: handle big allocations */
#define RESERVE_SUPERBLOCK_THRESHOLD 2

#define FREE_POISON 0xab
#define ALLOC_POISON 0xcd

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>

#define assert(cond) if (!(cond)) __asm__ __volatile__ ("int $3")
/*void printObject(void * p, int level);*/

/* Object: One return from malloc/input to free. */
struct __attribute__((packed)) object {/*We took out union, if this freaks out*/
    struct object *next; /*For free list (when not in use)*/
    char * raw;
};

/* Super block bookeeping; one per superblock.  "steal" the first
 * object to store this structure
 */
struct __attribute__((packed)) superblock_bookkeeping {
  struct superblock_bookkeeping * next; /* next super block*/
  struct object *free_list;
  /* Free count in this superblock*/
  uint8_t free_count; /* Max objects per superblock is 128-1, so a byte is sufficient*/
  uint8_t level;
  int bytes_per_object;
  int max_objects;
};

/* Superblock: a chunk of contiguous virtual memory.
 * Subdivide into allocations of same power-of-two size. */
struct __attribute__((packed)) superblock {
  struct superblock_bookkeeping bkeep;
  void *raw;
};


/* The structure for one pool of superblocks.
 * One of these per power-of-two */
struct superblock_pool {
  struct superblock_bookkeeping *next;
  uint64_t free_objects; /* Total number of free objects across all superblocks*/
  uint64_t whole_superblocks; /* Superblocks with all entries free*/
};

/* 10^5 -- 10^11 == 7 levels*/
#define LEVELS 7
static struct superblock_pool levels[LEVELS] = {{NULL, 0, 0},
						{NULL, 0, 0},
						{NULL, 0, 0},
						{NULL, 0, 0},
						{NULL, 0, 0},
						{NULL, 0, 0},
						{NULL, 0, 0}};


/*rounds the given number up to the nearest power of two*/
static inline int size2level (ssize_t size) {
        int iterations = 0;
	if (size > MAX_ALLOC)
		return -1;
        while (size > MIN_ALLOC) {
                size /= 2;
                iterations++;
        }
  return iterations;
}

static inline struct superblock_bookkeeping * alloc_super (int power) {
  void *page;
  struct superblock* sb;
  int free_objects = 0, bytes_per_object = 0, i = 0;
  char *cursor;

  page = mmap(NULL,SUPER_BLOCK_SIZE,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0);

  sb = (struct superblock*) page;
  sb->bkeep.next = levels[power].next;/*adds a pointer from this superblock to the former first of this size*/
  levels[power].next = &sb->bkeep;/*adds a pointer from the pool to this superblock*/
  levels[power].whole_superblocks++;/*superblock is empty, so it adds 1 to whole_superblocks*/
  sb->bkeep.level = power;/*sets bookkeeper level to power. Seems superfluous.*/
  sb->bkeep.free_list = NULL;/*nothing in list yet*/

  bytes_per_object = 32;

  for (; i < power ; i++)
    bytes_per_object *= 2;

  free_objects = SUPER_BLOCK_SIZE / bytes_per_object - 1;
  levels[power].free_objects += free_objects;
  sb->bkeep.free_count = free_objects;
  sb->bkeep.bytes_per_object = bytes_per_object;
  sb->bkeep.max_objects = free_objects;

  /* The following loop populates the free list with some atrocious
  pointer math.  You should not need to change this, provided that you
  correctly calculate free_objects.*/

  cursor = (char *) sb;
  /* skip the first object*/
  for (cursor += bytes_per_object; free_objects--; cursor += bytes_per_object) {/*advances cursor by size per object. */
    /* Place the object on the free list*/
    struct object* tmp = (struct object *) cursor;
    tmp->next = sb->bkeep.free_list;/*first address of temp holds pointer to next spot in list*/
    sb->bkeep.free_list = tmp;/*adds pointer from bkeep to start of linked list*/
  }

  return &sb->bkeep;
}

void *malloc(size_t size) {
  struct superblock_pool *pool;
  struct superblock_bookkeeping *bkeep;
  void *rv = NULL;
  int power = size2level(size);

  /* Check that the allocation isn't too big*/
  if (size > MAX_ALLOC) {
    errno = -ENOMEM;
    return NULL;
  }
  if (size <= 0)
    return NULL;

  pool = &levels[power]; /*pool points to proper sized pool*/

  if (!pool->free_objects) {/*if there are no free objects in the pool*/
    bkeep = alloc_super(power);/*make new superblock*/
  } else
    bkeep = pool->next;/*else bkeep points to bookkeeper for first superblock in pool*/

  while (bkeep != NULL) {
    if (bkeep->free_count) {/*if there are free objects in this superblock*/
      struct object *next = bkeep->free_list;
      /* Remove an object from the free list. */

      rv = next; /*rv is pointer to free space*/
      bkeep->free_list = next->next;/*pointer from bkeep to new start of list*/

      if ((bkeep->free_count) == bkeep->max_objects)
        levels[power].whole_superblocks --;

      bkeep->free_count --;
      levels[power].free_objects --;

      break;
    }else/*if there are not*/
      bkeep = bkeep->next;/*move on to next superblock*/
  }

  /* assert that rv doesn't end up being NULL at this point*/
  assert(rv != NULL);

  memset(rv, ALLOC_POISON, bkeep->bytes_per_object);

  return rv;
}

static inline
struct superblock_bookkeeping * obj2bkeep (void *ptr) {
  uint64_t addr = (uint64_t) ptr;
  addr &= SUPER_BLOCK_MASK;
  return (struct superblock_bookkeeping *) addr;
}

void free(void *ptr) {
  struct object *obj = (struct object *)ptr;
  struct superblock_bookkeeping *bkeep = obj2bkeep(ptr);
  struct superblock_bookkeeping *current, *parent;
  int x = bkeep->level;
  if (obj == NULL)
    return;

  memset(obj, FREE_POISON, bkeep->bytes_per_object);

  obj->next = bkeep->free_list; /*set old first free object as ptr's next object*/
  bkeep->free_list = obj; /*set bookkeeping's first free object to be the newly*/
  bkeep->free_count++;
  levels[x].free_objects++;
  /*if the free_count equals the maximum, we have a wholly free superblock*/

  if ((bkeep->free_count) == bkeep->max_objects)
     levels[x].whole_superblocks++;

  for (parent = levels[x].next, current = parent->next; levels[x].whole_superblocks > RESERVE_SUPERBLOCK_THRESHOLD; parent = current, current = current->next) {
    if (current->free_count == current->max_objects){
      parent->next = current->next;
      levels[x].whole_superblocks--;
      munmap(&current, SUPER_BLOCK_SIZE);
      break;
    }
  }
}

/* Do NOT touch this - this will catch any attempt to load this into a multi-threaded app*/
int pthread_create(void __attribute__((unused)) *x, ...) {
  exit(-ENOSYS);
}

