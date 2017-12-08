#include <Utils/StdAfx.h>

#include <Utils/Platform/FilePath.h>

#include <Windows.h>

FilePath::FilePath(const std::string& path)
{
	char buffer[1024];
	GetModuleFileNameA(NULL, buffer, 1024);
	std::string execPath(buffer, 1024);

	std::replace(execPath.begin(), execPath.end(), '\\', '/');
	size_t pos = execPath.find_last_of("\\/");
	std::string newpath = execPath.substr(0, pos);

	pos = newpath.find_last_of("RZE") + 2;
	newpath = execPath.substr(0, pos);
	
	mAbsolutePath = newpath + path;
	mRelativePath = path;
}

FilePath::~FilePath()
{
}

const std::string& FilePath::GetAbsolutePath() const
{
	return mAbsolutePath;
}

const std::string& FilePath::GetRelativePath() const
{
	// #TODO(Josh) for now until we develop this class better
	return mRelativePath;
}
