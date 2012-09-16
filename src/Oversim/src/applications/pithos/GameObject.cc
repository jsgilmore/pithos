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

GameObject::GameObject(const std::string o_name, int64_t o_size, simtime_t o_creationTime, int o_ttl) : cOwnedObject(o_name.c_str())
{
	objectName = "Unspecified";
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
	ttl = other.ttl;
	objectName = other.objectName;
	creationTime = other.creationTime;
	group_address = other.group_address;
	value = other.value;

	return *this;
}

bool operator==(const GameObject& object1, const GameObject& object2)
{
	if (object1.size != object2.size)
		return false;

	if (object1.ttl != object2.ttl)
		return false;

	if (object1.value != object2.value)
		return false;

	if (object1.objectName != object2.objectName)
		return false;

	//A creation time difference of no more than one is used to account for inaccuracies when converting from the float string of the BinaryValue to a float variable
	if ((object1.creationTime - object2.creationTime) > 1)
		return false;
	if ((object2.creationTime - object1.creationTime) > 1)
		return false;

	return true;
}

bool operator!=(const GameObject& object1, const GameObject& object2)
{
	return !(object1 == object2);
}

bool operator<(const GameObject& object1, const GameObject& object2)
{
	return object1.getContentHash() < object2.getContentHash();
}

GameObject::GameObject(const BinaryValue& binval) : cOwnedObject("GameObject")
{
	operator=(binval);
}

GameObject& GameObject::operator=(const BinaryValue& binval)
{
	//If an unspecified BinaryValue was received, return an unspecified GameObject
	if (binval == BinaryValue::UNSPECIFIED_VALUE)
	{
		objectName = "Unspecified";
		size = 0;
		creationTime = SIMTIME_ZERO;
		ttl = 0;
		value = 0;

		return *this;
	}

	//If a malicious nodes alters the data in the DHT module, it sets the value to: "Modified Data"
	//TODO: Insert a check here, that checks if the simulation configuration is set to have malicious nodes. This check will drastically reduce the number of string comparisons performed.
	if (binval == BinaryValue("Modified Data"))
	{
		objectName = "Modified";
		size = 0;
		creationTime = SIMTIME_ZERO;
		ttl = 0;
		value = intuniform(0, 100000);

		return *this;
	}

	std::string buf;			//Have a buffer string
	std::stringstream ss;		//Create a string stream

	//This part tokenises the string value of BinaryValue in order to recreate the original GameObject from a given BinaryValue (string)
	ss << binval;						//Insert the string into a stream
	std::vector<std::string> tokens;	//Create vector to hold our words

	while (ss >> buf)
		tokens.push_back(buf);

	//After the string has been tokenised, we have to use the tokens to populate the variables
	objectName = tokens[0];
	size = atol((tokens[1]).c_str());		//TODO: I'm quite sure this long will be 64 bits in a 64 bit system, but unsure about 32 bit systems
	creationTime = atof((tokens[2]).c_str());
	ttl = atoi((tokens[3]).c_str());
	value = atoi((tokens[4]).c_str());

	return *this;
}

//The following functions are required by the cOwnedObject type from which this object inherits.
std::string GameObject::info()
{
	/**
	 * It's important that this function is kept up to date,
	 * since the BinaryValue and hash of the object is generated using it.
	 * Also keep the const function up to date with this one.
	 *
	 * Space delimiting is used by the C++ string streams to tokenise the input for reconstruction
	 *
	 * The info attribute is intentionally not present,
	 * since the hash of an object should not depend on whether it's a root, replica or overlay object
	 */

	std::stringstream out;
	out << objectName << " " << size << " " << creationTime << " " << ttl << " " <<value;
	return out.str();
}

std::string GameObject::info() const
{
	std::stringstream out;
	out << objectName << " " << size << " " << creationTime << " " << ttl << " " <<value;
	return out.str();
}

//This stream is basically a more descriptive string output of the contents of a game object
std::ostream& operator<<(std::ostream& stream, const GameObject go)
{
    return stream << /*This will state the node number and object number: */ go.objectName
					<< " Size: " << go.size
                  << " CreationTime: " << go.creationTime
                  << " TTL: " << go.ttl;
}

BinaryValue GameObject::getBinaryValue()
{
	return BinaryValue(info());
}

BinaryValue GameObject::getBinaryValue() const
{
	BinaryValue binval(info());

	return binval;
}

OverlayKey GameObject::getContentHash()
{
	return OverlayKey::sha1(getBinaryValue());
}

OverlayKey GameObject::getContentHash() const
{
	return OverlayKey::sha1(getBinaryValue());
}

OverlayKey GameObject::getNameHash()
{
	return OverlayKey::sha1(BinaryValue(objectName));
}

OverlayKey GameObject::getNameHash() const
{
	return OverlayKey::sha1(BinaryValue(objectName));
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

int64_t GameObject::getSize() const
{
	return size;
}

void GameObject::setSize(const int64_t &o_size)
{
	size = o_size;
}

int GameObject::getTTL()
{
	return ttl;
}

int GameObject::getTTL() const
{
	return ttl;
}

int GameObject::getValue()
{
	return value;
}

int GameObject::getValue() const
{
	return value;
}

void GameObject::setValue(const int &val)
{
	value = val;
}


void GameObject::setTTL(const int &o_ttl)
{
	ttl = o_ttl;
}

void GameObject::setObjectName(const std::string& o_Name)
{
	objectName = o_Name;
}

std::string GameObject::getObjectName()
{
	return objectName;
}

std::string GameObject::getObjectName() const
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

TransportAddress GameObject::getGroupAddress()
{
	return group_address;
}

TransportAddress GameObject::getGroupAddress() const
{
	return group_address;
}

void GameObject::setGroupAddress(const TransportAddress &gr_adr)
{
	group_address = gr_adr;
}

bool GameObject::isUnspecified()
{
	return (*this == GameObject::UNSPECIFIED_OBJECT);
}
