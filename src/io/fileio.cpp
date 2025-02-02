#include <cstdio>
#include "fileio.h"

// fileio.cpp
namespace cg::io
{
	CGFile ReadFile(const char* path)
	{
		FILE* file;
		fopen_s(&file, path, "rb");

		if (!file)
		{
			return { nullptr, 0 };
		}

		// Get file size
		fseek(file, 0, SEEK_END);
		size_t size = static_cast<size_t>(ftell(file));
		fseek(file, 0, SEEK_SET);

		auto buffer = std::make_unique<char[]>(size + 1);
		
		if (!buffer)
		{
			fclose(file);
			return { nullptr, 0 };
		}

		fread(buffer.get(), 1, size, file);
		buffer[size] = '\0';
		fclose(file);

		return { std::move(buffer), size };
	}
}
