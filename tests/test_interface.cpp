

#include <iostream>
#include <vector>
#include <array>
#include <random>
#include <algorithm>
#include <numeric>

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

int testBasic()
{
	constexpr size_t numElementsToAllocate{1024};
	constexpr size_t blockSize{32};
	constexpr size_t maxNumBlocks{numElementsToAllocate / 32};

	MemoryPool<Node> mempool{blockSize, maxNumBlocks};
	
	std::vector<Node*> allocatedPtrs; 
	allocatedPtrs.reserve(numElementsToAllocate);

	std::random_device rd;
    std::mt19937 gen(rd());

	std::array<size_t, numElementsToAllocate> freeOrder;
	std::iota(freeOrder.begin(), freeOrder.end(), 0); 

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

		std::shuffle(freeOrder.begin(), freeOrder.end(), gen);
		for (auto i : freeOrder)
		{
			mempool.free(allocatedPtrs[i]);
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
	return 0;
}