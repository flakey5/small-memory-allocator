# small-memory-allocator

## How it works

 * Multiple pool allocators are created, each with blocks double the size of the previous one (e.g. 8 bytes, 16 bytes, 32 bytes, ...).
 * When an allocation needs to be made, we search for the pool with the smallest block size that fits the allocation (e.g. for a uint16_t, the pool with 8 byte blocks will be used).

## Tradeoffs

This does waste space, however, it significantly reduces heap fragmentation.

Performance should be O(1) in the best case (the allocation fits into the smallest pool).
Under the hood, the pools are just linked lists. The nodes are allocated when the blocks are, so the only calls to
`new[]` and `delete[]` is in the pool's constructor and deconstructor.

## Building

Run `make`.
