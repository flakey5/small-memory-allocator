#pragma once

#include <cstdint>
#include <vector>
#include <cassert>
#include "pool_allocator.hpp"

class SmallMemoryAllocator {
private:
    // TODO: putting these directly into std::vector copies them
    PoolAllocator pool8 = PoolAllocator(8, 64);
    PoolAllocator pool16 = PoolAllocator(16, 64);
    PoolAllocator pool32 = PoolAllocator(32, 64);
    PoolAllocator pool64 = PoolAllocator(64, 64);
    PoolAllocator pool128 = PoolAllocator(128, 64);
    PoolAllocator pool256 = PoolAllocator(256, 64);
    PoolAllocator pool512 = PoolAllocator(512, 64);

    std::vector<PoolAllocator*> pools {
        &pool8,
        &pool16,
        &pool32,
        &pool64,
        &pool128,
        &pool256,
        &pool512,
    };

    struct Header {
        uint8_t poolIndex;
        uint16_t quantity;
    };
    
public:
    template<typename T>
    T* alloc(uint16_t quantity = 1) {
        size_t size = (sizeof(T) * quantity) + sizeof(Header);

        size_t poolIndex = 0;
        void* block = nullptr;
        for (PoolAllocator* pool : this->pools) {
            if (size >= pool->getBlockSize() || !pool->hasFreeBlocks()) {
                continue;
            }

            block = pool->pop();
            if (block != nullptr) {
                break;
            }

            poolIndex++;
        }

        Header* header = reinterpret_cast<Header*>(block);
        header->poolIndex = poolIndex;
        header->quantity = quantity;

        T* values = reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(block) + sizeof(Header));
        for (size_t i = 0; i < quantity; i++) {
            T* value = &values[i];
            new(value) T;
        }

        return values;
    }

    template<typename T>
    void free(T* block) {
        Header* header = reinterpret_cast<Header*>(reinterpret_cast<uintptr_t>(block) - sizeof(Header));
        assert(header);
        
        for (size_t i = 0; i < header->quantity; i++) {
            T* value = &block[i];
            value->~T();
        }

        PoolAllocator* pool = this->pools[header->poolIndex];
        pool->push(header);
    }
};
