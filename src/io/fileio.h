#pragma once

#include <memory>

// fileio.h
namespace cg::io
{
	struct CGFile
	{
		std::unique_ptr<char[]> data = nullptr;
		size_t size = 0ull;
	};

	CGFile ReadFile(const char* path);
}
