#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

class PoolAllocator {
private:
    /**
     * All of the nodes
     * 
     * Node layout in memory:
     * 8 bytes - next node
     * n bytes - node data (n=blockSize)
     * 
     * You can think of it essentially like this:
     * `struct Node { Node* next; uint8_t data[blockSize]; };`
     */
    void* data;

    size_t blockSize;
    size_t nodeSize;
    size_t nodeDataOffset;

    size_t blocksAvailable = 0;
    void* head;
    void* tail;

    void* getNodeAt(size_t index);

public:
    /**
     * @param blockSize Size of the block allocated per node
     * @param quantity Amount of blocks to be allocated
     */
    PoolAllocator(size_t blockSize, size_t quantity);
    ~PoolAllocator();

    PoolAllocator(const PoolAllocator& other) = delete;
    PoolAllocator(PoolAllocator&&) = delete;

    inline size_t getBlockSize() const {
        return this->blockSize;
    }

    inline bool hasFreeBlocks() const {
        return this->blocksAvailable != 0;
    }

    void* pop();

    void push(void* value);
};
