#pragma once

#include <memory>

// fileio.h
namespace cg::io
{
	struct CGFile
	{
		std::unique_ptr<char[]> data;
		size_t size;
	};

	CGFile ReadFile(const char* path);
}
