#pragma once

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "XMLNode.h"

class XMLNode
{
public:
	explicit operator bool() const noexcept
	{
		return node != nullptr;
	}

	using Ptr = rapidxml::xml_node<>*;
	using Attribute = rapidxml::xml_attribute<>*;

public:
	explicit XMLNode(Ptr xmlNode) : node(xmlNode) { }

	XMLNode child() const { return XMLNode(node->first_node()); }
	XMLNode child(const char* label) const { return XMLNode(node->first_node(label)); }

	XMLNode next() const { return XMLNode(node->next_sibling()); }
	XMLNode next(const char* label) const { return XMLNode(node->next_sibling(label)); }

	Attribute attribute() const { return (node->first_attribute()); }
	Attribute attribute(const char* label) const { return node->first_attribute(label); }

	const char* name() const { return node->name(); }
	const char* value() const { return node->value(); }

	void setValue(const char* value) { node->value(value); }
	bool isEmpty() const { return node == nullptr; }

private:
	Ptr node;
};


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
