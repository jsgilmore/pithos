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

#ifndef OBJECTINFO_H_
#define OBJECTINFO_H_

#include <string>
#include <vector>
#include <TransportAddress.h>

/**
 * This class stores the information of a single object, including name,
 * size and a list of peers that store the object. The class is used by
 * super peers as part of a vector to keep track of which game objects
 * reside where in the group.
 *
 * @author John Gilmore
 */
class ObjectInfo
{
	private:
		std::string object_name;
		int size;
		std::vector<TransportAddress> location_list;
	public:
		ObjectInfo();
		virtual ~ObjectInfo();

		void setObjectName(const std::string &o_name);
		void setObjectName(char *o_name);
		std::string getObjectName();
		void addAddress(const TransportAddress &adr);
		void addAddress(const char* ip_str, int port);
		TransportAddress getAddress(const int &i);
		TransportAddress getRandAddress();
		void setSize(const int &siz);
		int getSize();
};

#endif /* OBJECTINFO_H_ */
