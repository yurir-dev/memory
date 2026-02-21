#pragma once

#include <memory>
#include <type_traits>
#include <stdexcept>
#include <cstddef>
#include <vector>

template<typename ObjType>
class MemoryPool
{
    union Slot
    {
        Slot* next;
        alignas(ObjType) char storage[sizeof(ObjType)];
    };
    struct MemoryBlock
    {
        MemoryBlock(size_t capacity)
        {
            _pool = std::make_unique<Slot[]>(capacity);
            for (size_t i = 0; i < capacity - 1; ++i)
            {
                _pool[i].next = &_pool[i + 1];
            }
            _pool[capacity - 1].next = nullptr;
        }
        Slot* getFirst() {return &_pool[0];}
        
        std::unique_ptr<Slot[]> _pool;
    };

public:
    MemoryPool(size_t blockCapacity, size_t maxNumBlocks)
    : _blockCapacity{blockCapacity}, _maxNumBlocks{maxNumBlocks}
    {
        _freePtr = allocateNewBlock(_blockCapacity);
    }
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;

    template <typename... Args >
    ObjType* allocate(Args... args)
    {
        if (_freePtr == nullptr)
        {
            if (_maxNumBlocks > _blocks.size())
            {
                _freePtr = allocateNewBlock(_blockCapacity);
            }
            else
            {
                throw std::runtime_error{"no more memory"};
            }
        }

        Slot* slot = _freePtr;
        _freePtr = _freePtr->next;

        return new (slot->storage) ObjType(std::forward<Args>(args)...);
    }
    void free(ObjType* objPtr)
    {
        if (!objPtr)
        {
            return;
        }

        objPtr->~ObjType();

        Slot* slot = reinterpret_cast<Slot*>(objPtr);
        slot->next = _freePtr;
        _freePtr = slot;
    }

private:
    Slot* allocateNewBlock(size_t blockCapacity)
    {
        auto& addedBlock = _blocks.emplace_back(blockCapacity);
        return addedBlock.getFirst();
    }

    size_t _blockCapacity{0};
    size_t _maxNumBlocks{0};
    std::vector<MemoryBlock> _blocks;
    Slot* _freePtr{nullptr};
};