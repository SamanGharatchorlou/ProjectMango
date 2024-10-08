#include "pch.h"
#include "Property.h"


PropertyType::PropertyType(const BasicString& name) : type(getType(name)) { }


PropertyType::Type PropertyType::getType(const BasicString& name) const
{
	return findPropertyType(name);
}

BasicString PropertyType::string() const
{
	return findPropertyString(type);
}


PropertyType::Type findPropertyType(const BasicString& string)
{
#if DEBUG_MODE
	if (PropertyLookUpTable.count(string) == 0)
	{
		DebugPrint(Warning, "No property in the lookup table with string '%s'", string.c_str());
		return PropertyType::None;
	}
	else
#endif
		return PropertyLookUpTable.at(string);
}


BasicString findPropertyString(PropertyType::Type type)
{
	std::unordered_map<BasicString, PropertyType::Type>::const_iterator iter;
	for (iter = PropertyLookUpTable.begin(); iter != PropertyLookUpTable.end(); iter++)
	{
		if (type == iter->second)
			return iter->first;
	}

	DebugPrint(Warning, "No property in the lookup table with type '%d'", type);
	return "";
}



AttributeType::AttributeType(PropertyType::Type propertyType) : PropertyType(propertyType) { }
AttributeType::AttributeType(const BasicString& name) : PropertyType(name) { }


bool operator == (PropertyType propertyA, PropertyType propertyB)
{
	return propertyA.type == propertyB.type;
}

bool operator == (AttributeType attributeA, AttributeType attributeB)
{
	return attributeA.type == attributeB.type;
}