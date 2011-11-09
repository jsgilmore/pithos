#include "GameObject.h"

//Register_Class(GameObject);

GameObject::GameObject(const GameObject& other) : cOwnedObject(other.getName())
{
	operator=(other);
}

GameObject::GameObject(const BinaryValue& binval) : cOwnedObject("")
{
	std::string buf;			// Have a buffer string
	std::stringstream ss;		//Create a string stream
	ss << binval;				// Insert the string into a stream

	std::vector<std::string> tokens; // Create vector to hold our words

	while (ss >> buf)
		tokens.push_back(buf);

}

GameObject::~GameObject()
{

}

//The following functions do not form part of a GameObject itself, but are required for the cOwnedObject type from which this object inherits.

GameObject *GameObject::dup() const
{
	return new GameObject(*this);
}

GameObject& GameObject::operator=(const GameObject& other)
{
	if (&other==this)
		return *this;
	cOwnedObject::operator=(other);


	size = other.size;
	type = other.type;
	strcpy(objectName, other.objectName);
	creationTime = other.creationTime;

	return *this;
}

std::string GameObject::info()
{
	//It's important that this function is kept up to date, since the BinaryValue of the object is generated using it
	//Also, space delimiting is used by the C++ string streams to tokenise the input for reconstruction
	std::stringstream out;
	out << objectName << " " << size << " " << type << " " << creationTime;
	return out.str();
}

//This is the real meat of the packet. The getter and setter methods for the different attributes.
int64_t GameObject::getSize()
{
	return size;
}

void GameObject::setSize(const int64_t &o_size)
{
	size = o_size;
}

int GameObject::getType()
{
	return type;
}

void GameObject::setType(const int &o_type)
{
	type = o_type;
}

void GameObject::setObjectName(const char *o_Name)
{
	strcpy(objectName, o_Name);
}

char * GameObject::getObjectName()
{
	return objectName;
}

void GameObject::setCreationTime(const simtime_t &time)
{
	creationTime = time;
}

simtime_t GameObject::getCreationTime()
{
	return creationTime;
}

BinaryValue GameObject::getBinaryValue()
{
	BinaryValue binval(info());

	return binval;
}
