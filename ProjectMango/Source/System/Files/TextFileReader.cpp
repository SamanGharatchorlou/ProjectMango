#include "pch.h"
#include "TextFileReader.h"


TextFileReader::TextFileReader(const BasicString& filePath)
{
	mFile.open(filePath.c_str());
	if (!mFile)
		DebugPrint(Warning, "Unable to open file '%s'", filePath.c_str());
}


TextFileReader::~TextFileReader()
{
	if (mFile)
		mFile.close();
}



void TextFileReader::readText(BasicString& outText)
{
	int length = fileLength();
	outText = BasicString("", length);

	char buffer[25]; // max word length
	while (mFile >> buffer)
	{
		outText.concat(" ");
		outText.concat(buffer);
	}

#if DEBUG_MODE
	if (outText.length() != length)
	{
		DebugPrint(Warning, "The file size %d does not match out out string length %d", length, outText.length());
	}
#endif
}


int TextFileReader::fileLength()
{
	mFile.seekg(0, mFile.end);
	int length = (int)mFile.tellg();

	mFile.seekg(0, mFile.beg);
	return length;
}