#include <StdAfx.h>
#include <EngineCore/Config/Config.h>

Config::Config()
{
}

Config::~Config()
{
}

bool Config::Empty() const
{
	const bool bFileEmpty = mINIFile.Empty();
	const bool bParserEmpty = mINIParser.IsEmpty();

	return bFileEmpty && bParserEmpty;
}

void Config::Read(const std::string& filePath)
{
	mINIFile.SetFilePath(filePath);
	mINIFile.Read();
	mINIFile.Close();

	if (!mINIFile.Empty())
	{
		mINIParser.LoadData(mINIFile.Content());
	}
	else
	{
		RZE_LOG_ARGS("Could not open file at path [%s]", mINIFile.GetPath().GetRelativePath().c_str());
	}
}
