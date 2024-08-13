#include "pch.h"
#include "XMLParser.h"

#include "rapidxml_print.hpp"


void XMLParser::parseXML(const char* filePath)
{
	ASSERT(fs::exists(filePath), "File path %s does not exist, cannot parse xml file", filePath);

	// todo: why am i new'ing here, do i need to??
	file = new rapidxml::file<>(filePath);
	xmlFile.parse<rapidxml::parse_no_data_nodes>(file->data());

#if DEBUG_CHECK
	path = filePath;
#endif
}

void XMLParser::saveToFile(std::ofstream& file)
{
	DebugPrint(Log, "Saveing xml to file %s", xmlFile.value());
	file << xmlFile;
}


XMLParser::~XMLParser()
{
	delete file;
}

void XMLParser::reload(const char* filePath)
{
	if (file)
	{
		delete file;
		file = nullptr;
	}

	file = new rapidxml::file<>(filePath);
	xmlFile.parse<rapidxml::parse_no_data_nodes>(file->data());

#if DEBUG_CHECK
	path = filePath;
#endif
}

XMLNode XMLParser::rootNode() const
{
	return XMLNode(xmlFile.first_node());
}


XMLNode XMLParser::rootChild(const char* label) const
{
#if DEBUG_CHECK
	XMLNode child = rootNode().child(label);
	if (child.isEmpty())
		DebugPrint(Log, "Parser at path '%s' has no child node labeled '%s'", path.c_str(), label);
	return child;
#endif

	return XMLNode(xmlFile.first_node()->first_node(label));
}
