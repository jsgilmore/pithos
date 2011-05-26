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

#ifndef GO_H_
#define GO_H_

#include <omnetpp.h>

enum ObjectTypes {
    ROOT = 1,
    REPLICA = 2,
    OVERLAY = 3
};

class GameObject : public cOwnedObject
{
	private:
		char objectName[41];
		int64_t size;
		int type;
		simtime_t creationTime;
	public:
		GameObject(const char *name=NULL, int type=ROOT);
		GameObject(const GameObject& other);
		virtual ~GameObject();
		virtual GameObject *dup() const;
		GameObject& operator=(const GameObject& other);
		virtual std::string info();

		int64_t getSize();
		void setSize(int64_t o_size);
		int getType();
		void setType(int o_type);
		void setObjectName(char *o_Name);
		char *getObjectName();
		void setCreationTime(simtime_t time);
		simtime_t getCreationTime();
};

#endif /* GO_H_ */
