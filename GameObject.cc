//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "GameObject.h"

//Register_Class(GameObject);

GameObject::GameObject(const char *name) : cOwnedObject(name)
{
	size = 0;
}

GameObject::GameObject(const GameObject& other) : cOwnedObject(other.getName())
{
	operator=(other);
}

GameObject::~GameObject()
{

}

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

	return *this;
}

std::string GameObject::info()
{
	std::stringstream out;
	out << "size = " << size;
	return out.str();
}

int64_t GameObject::getSize()
{
	return size;
}

void GameObject::setSize(int64_t o_size)
{
	size = o_size;
}
