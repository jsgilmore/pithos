//
// Copyright (C) 2011 MIH Media lab, University of Stellenbosch
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

#include "GameObject.h"

// predefined GameObject
const GameObject GameObject::UNSPECIFIED_OBJECT;

GameObject::~GameObject()
{

}

GameObject::GameObject(const char *name, int o_type, int64_t o_size, simtime_t o_creationTime, int o_ttl) : cOwnedObject(name)
{
	type = o_type;
	size = o_size;
	creationTime = o_creationTime;
	ttl = o_ttl;
}

GameObject::GameObject(const GameObject& other) : cOwnedObject(other.getName())
{
	operator=(other);
}

GameObject& GameObject::operator=(const GameObject& other)
{
	if (&other==this)
		return *this;
	cOwnedObject::operator=(other);

	size = other.size;
	type = other.type;
	ttl = other.ttl;
	strcpy(objectName, other.objectName);
	creationTime = other.creationTime;

	return *this;
}

GameObject::GameObject(const BinaryValue& binval)
{
	operator=(binval);
}

GameObject& GameObject::operator=(const BinaryValue& binval)
{
	cOwnedObject("GameObject");

	std::string buf;			// Have a buffer string
	std::stringstream ss;		//Create a string stream

	//This part tokenises the string value of BinaryValue in order to repopulate GameObject
	ss << binval;				// Insert the string into a stream
	std::vector<std::string> tokens; // Create vector to hold our words

	while (ss >> buf)
		tokens.push_back(buf);

	//After the string has been tokenised, we have to use the tokens to populate the variables
	strcpy(objectName, (tokens[0]).c_str());
	size = atol((tokens[1]).c_str());		//I'm quite sure this long will be 64 bits in a 64 bit system, but unsure about 32 bit systems
	type = atoi((tokens[2]).c_str());
	creationTime = atof((tokens[3]).c_str());
	ttl = atoi((tokens[4]).c_str());

	return *this;
}

//The following functions are required by the cOwnedObject type from which this object inherits.
std::string GameObject::info()
{
	//It's important that this function is kept up to date, since the BinaryValue of the object is generated using it
	//Also, space delimiting is used by the C++ string streams to tokenise the input for reconstruction
	std::stringstream out;
	out << objectName << " " << size << " " << type << " " << creationTime << " " << ttl;
	return out.str();
}

//This stream is basically a more descriptive string output of the contents of a game object
std::ostream& operator<<(std::ostream& stream, const GameObject go)
{
    return stream << /*This will state the node number and object number: */ go.objectName
					<< " Size: " << go.size
					<< " Type: " << go.type
                  << " CreationTime: " << go.creationTime
                  << " TTL: " << go.ttl;
}

void GameObject::getHash(char hash_str[41])
{
	CSHA1 hash;

	for (int i = 0 ; i < 41 ; i++)	//The string has to be cleared for the OverlayKey constructor to correctly handle it.
		hash_str[i] = 0;

	//Create a hash of the game object's name
	hash.Update(((unsigned char *)info().c_str()), strlen(info().c_str()));
	hash.Final();
	hash.ReportHash(hash_str, CSHA1::REPORT_HEX);
}

OverlayKey GameObject::getHash()
{
	return OverlayKey::sha1(getBinaryValue());
}

GameObject *GameObject::dup() const
{
	return new GameObject(*this);
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

int GameObject::getTTL()
{
	return ttl;
}

int GameObject::getTTL() const
{
	return ttl;
}

void GameObject::setTTL(const int &o_ttl)
{
	ttl = o_ttl;
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

simtime_t GameObject::getCreationTime() const
{
	return creationTime;
}

BinaryValue GameObject::getBinaryValue()
{
	BinaryValue binval(info());

	return binval;
}
