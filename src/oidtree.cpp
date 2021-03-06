/**
* Project Clearwater - IMS in the Cloud
* Copyright (C) 2013 Metaswitch Networks Ltd
*
* This program is free software: you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation, either version 3 of the License, or (at your
* option) any later version, along with the "Special Exception" for use of
* the program along with SSL, set forth below. This program is distributed
* in the hope that it will be useful, but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR
* A PARTICULAR PURPOSE. See the GNU General Public License for more
* details. You should have received a copy of the GNU General Public
* License along with this program. If not, see
* <http://www.gnu.org/licenses/>.
*
* The author can be reached by email at clearwater@metaswitch.com or by
* post at Metaswitch Networks Ltd, 100 Church St, Enfield EN2 6BQ, UK
*
* Special Exception
* Metaswitch Networks Ltd grants you permission to copy, modify,
* propagate, and distribute a work formed by combining OpenSSL with The
* Software, or a work derivative of such a combination, even if such
* copying, modification, propagation, or distribution would otherwise
* violate the terms of the GPL. You must comply with the GPL in all
* respects for all of the code used other than OpenSSL.
* "OpenSSL" means OpenSSL toolkit software distributed by the OpenSSL
* Project and licensed under the OpenSSL Licenses, or a work based on such
* software and licensed under the OpenSSL Licenses.
* "OpenSSL Licenses" means the OpenSSL License and Original SSLeay License
* under which the OpenSSL Project distributes the OpenSSL toolkit software,
* as those licenses appear in the file LICENSE-OPENSSL.
*/

#include "oidtree.hpp"
#include <iostream>

static void dump_oidmap(OIDMap m);

bool OIDTree::get(OID requested_oid, int& output_result)
{
  _map_lock.lock();
  bool retval = false;
  OIDMap::iterator oid_location = _oidmap.find(requested_oid);
  if (oid_location == _oidmap.end())
  {
    retval = false;
  }
  else
  {
    output_result = oid_location->second;
    retval = true;
  }
  _map_lock.unlock();
  return retval;
}

bool OIDTree::get_next(OID requested_oid, OID& output_oid, int& output_result)
{
  _map_lock.lock();
  bool retval = false;
  OIDMap::iterator oid_location = _oidmap.upper_bound(requested_oid);

  if (oid_location == _oidmap.end())
  {
    retval = false;
  }
  else
  {
    output_oid = oid_location->first;
    output_result = oid_location->second;
    retval = true;
  }
  _map_lock.unlock();
  return retval;
}

void OIDTree::remove(OID key)
{
  _map_lock.lock();
  _oidmap.erase(key);
  _map_lock.unlock();
}

void OIDTree::remove_subtree(OID root_oid)
{
  _map_lock.lock();

  // Create a temporary copy, to avoid iterating over the map we're
  // deleting items from.
  OIDMap tmp_oidmap = _oidmap;
  for(OIDMap::iterator it = tmp_oidmap.begin();
      it != tmp_oidmap.end();
      ++it)
  {
    OID this_oid = it->first;
    if (root_oid.subtree_contains(this_oid))
    {
      _oidmap.erase(this_oid);
    }
  }
  _map_lock.unlock();
}

void OIDTree::replace_subtree(OID root_oid, OIDMap update)
{
  _map_lock.lock();
  remove_subtree(root_oid);
  _oidmap.insert(update.begin(), update.end());

  _map_lock.unlock();
}


void OIDTree::set(OID key, int value)
{
  _map_lock.lock();
  _oidmap[key] = value;
  _map_lock.unlock();
}

void OIDTree::dump()
{
  dump_oidmap(_oidmap);
}

static void dump_oidmap(OIDMap m) {
  for(OIDMap::iterator it = m.begin();
      it != m.end();
      ++it)
  {
    std::cerr << it->first.to_string() << " " << it->second << "\n";
  }
}
