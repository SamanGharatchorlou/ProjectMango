#pragma once

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "XMLNode.h"

class XMLParser
{
public:
	XMLParser() { };
	XMLParser(const char* file) { parseXML(file); }
	~XMLParser();

	void reload(const char* file);
	void saveToFile(std::ofstream& file);

	void parseXML(const char* filePath);
	
	rapidxml::xml_document<>& getXML() { return xmlFile; }

	XMLNode rootNode() const;
	XMLNode rootChild(const char* label) const;

#if DEBUG_MODE
	BasicString path;
#endif

private:
	rapidxml::file<>* file = nullptr;
	rapidxml::xml_document<> xmlFile;
};
