#pragma once
#include <ostream>
#include <vector>
#include <iostream>
#include <cctype>


// RLE Compression/Decompression
struct RleData
{
	// Memory which stores either compressed or decompressed data
	// char* mData = nullptr;
	// Number of elements of type T that data is pointing to
	// int mSize;

    std::vector<char> mData;

	RleData() = default;


	~RleData() = default;

	// Delete other five
	RleData(const RleData& other) = delete;
	RleData(RleData&& other) = delete;
	RleData& operator=(const RleData& other) = delete;
	RleData& operator=(RleData&& other) = delete;

	// Compresses input data and stores it in mData
	void Compress(const char* input, size_t inSize);

	// Decompresses input data and stores it in mData
	void Decompress(const char* input, size_t inSize, size_t outSize);

	// Outputs mData
	friend std::ostream& operator<< (std::ostream& stream, const RleData& rhs);

	size_t MaxRunSize()
	{
		// Bitwise magic
		return 127;
	};


};

