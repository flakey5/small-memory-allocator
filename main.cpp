#include "small_memory_allocator.hpp"

#include <cstdio>

class Testing {
public:
    uint64_t a = 0;
    uint64_t b = 1;
    uint64_t c = 2;

    Testing() {
        printf("Testing::Testing\n");
    }

    ~Testing() {
        printf("Testing::~Testing\n");
    }
};

int main() {
    // create pools up to 512 bytes
    SmallMemoryAllocator allocator;

    int* a = allocator.alloc<int>();
    *a = 10;
    printf("a(%p) = %i\n", (void*)a, *a);

    Testing* b = allocator.alloc<Testing>();
    printf("testing=%p\n", (void*)b);
    printf("    a=%ld\n", b->a);
    printf("    b=%ld\n", b->b);
    printf("    c=%ld\n", b->c);
}
