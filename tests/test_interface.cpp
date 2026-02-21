

#include <iostream>
#include <vector>

#include "memoryPool.h"

struct Node
{
	Node() = default;
	Node(size_t id, std::string name): _id{id}, _name{std::move(name)} {}

	size_t _id;
	std::string _name;
};

std::ostream& operator<<(std::ostream& os, const Node& obj)
{
	os << static_cast<const void*>(&obj) << " id: " << obj._id << ", name: " << obj._name << std::endl;
	return os;
}

int testBadUsage()
{
#if 0
	struct NoDefaultCtor {
		NoDefaultCtor(int){}
		void* ptr;
	};
	[[maybe_unused]] MemoryPool<NoDefaultCtor> mempool1{128, 1}; // compilation error

	struct Empty {};
	[[maybe_unused]] MemoryPool<Empty> mempool2{128, 1}; // compilation error
#endif
	return 0;
}

int testBasic()
{
	constexpr size_t numElementsToAllocate{1024};
	constexpr size_t blockSize{32};
	constexpr size_t maxNumBlocks{numElementsToAllocate / 32};

	MemoryPool<Node> mempool{blockSize, maxNumBlocks};
	
	std::vector<Node*> allocatedPtrs; 
	allocatedPtrs.reserve(numElementsToAllocate);

	size_t repeat{10};
	while(repeat-- > 0)
	{
		for (size_t i = 0 ; i < numElementsToAllocate ; i++)
		{
			auto* objPtr = mempool.allocate(i, "my name is " + std::to_string(i));
			//std::cout << *objPtr << std::endl;
			allocatedPtrs.push_back(objPtr);
		}

		try
		{
			mempool.allocate(1, "expect to fail to allocate");
			std::cerr << "FAILED, expected to fail to allocate." << std::endl;
			return __LINE__;
		}
		catch(const std::exception& e)
		{
			std::cout << "expected failed to allocate: " << e.what() << std::endl;
		}

		for (auto* ptr : allocatedPtrs)
		{
			mempool.free(ptr);
		}
		allocatedPtrs.clear();

		std::cout << __FILE__ << ':' << __LINE__ << " Successful test iteration" << std::endl;
	}

	return 0;
}
int main(int /*argc*/, char* /*argv*/ [])
{
	if (auto res = testBasic() ; res != 0)
	{
		return res;
	}
	if (auto res = testBadUsage() ; res != 0)
	{
		return res;
	}
	return 0;
}