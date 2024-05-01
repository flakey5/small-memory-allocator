#include "pool_allocator.hpp"

#include <cassert>
#include <cstdio>

// this is cursed

PoolAllocator::PoolAllocator(size_t blockSize, size_t quantity) {    
    this->blockSize = blockSize;

    size_t unpaddedNodeSize = blockSize + 8;
    if (blockSize > 8) {
        // Need to align the next ptr with blockSize
        size_t nextPadding = (blockSize - (8 & (blockSize - 1))) & (blockSize - 1);
        this->nodeSize = unpaddedNodeSize + nextPadding;

        // Offset is the size of the ptr to the next node + it's padding
        this->nodeDataOffset = 8 + nextPadding;
    } else {
        // Need to align with the next ptr (8 bytes)
        size_t dataPadding = (8 - (unpaddedNodeSize & 7)) & 7;
        this->nodeSize = unpaddedNodeSize + dataPadding;

        // Offset is just the size of the ptr to the next node
        this->nodeDataOffset = 8;
    }

    this->data = new uint8_t[this->nodeSize * quantity];
    memset(this->data, 0, this->nodeSize * quantity);

    // Link each node together
    for (size_t i = 0; i < quantity - 1; i++) {
        uintptr_t node = reinterpret_cast<uintptr_t>(this->getNodeAt(i));
        void* data = reinterpret_cast<void*>(node + this->nodeDataOffset);
        this->push(data);
    }
}

PoolAllocator::~PoolAllocator() {
    uint8_t* data = reinterpret_cast<uint8_t*>(this->data);
    delete[] data;
}

void* PoolAllocator::getNodeAt(size_t index) {
    uint8_t* dataByBytes = reinterpret_cast<uint8_t*>(this->data);
    void* value = &dataByBytes[this->nodeSize * index];

    return value;
}

void* PoolAllocator::pop() {
    if (this->blocksAvailable == 0) {
        return nullptr;
    }

    this->blocksAvailable--;

    void* head = this->head;

    size_t nextPtr = *reinterpret_cast<size_t*>(head);        
    this->head = reinterpret_cast<void*>(nextPtr);

    if (this->head == nullptr) {
        this->tail = nullptr;
    }

    void* data = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(head) + this->nodeDataOffset);
    return data;
}

void PoolAllocator::push(void* value) {
    assert(
        value > &reinterpret_cast<uint8_t*>(this->data)[0] &&
        "PoolAllocator::push value is at a lower memory address than us"
    );
    // TODO: assert value isn't at a higher memory address that's outside of this->value

    // Grab the beginning of the node
    void* node = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(value) - this->nodeDataOffset);
    
    if (this->blocksAvailable == 0) {
        this->head = node;
        this->tail = node;
    } else {
        size_t* tailNextPtr = reinterpret_cast<size_t*>(this->tail);
        *tailNextPtr = reinterpret_cast<size_t>(node);

        this->tail = node;
    }

    this->blocksAvailable++;
}
