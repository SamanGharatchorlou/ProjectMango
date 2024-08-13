#pragma once

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
