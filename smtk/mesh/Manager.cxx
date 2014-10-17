//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================

#include "smtk/mesh/Manager.h"

#include "smtk/common/UUID.h"

#include <map>

namespace smtk {
namespace mesh {

//----------------------------------------------------------------------------
class Manager::InternalStorageImpl
{
public:
  typedef std::map< smtk::common::UUID, smtk::mesh::Collection > ContainerType;

  typedef ContainerType::value_type value_type;
  typedef ContainerType::mapped_type mapped_type;
  typedef ContainerType::key_type key_type;
  typedef ContainerType::const_iterator const_iterator;
  typedef ContainerType::iterator iterator;

  InternalStorageImpl():
    Collections()
  {
  }

  //----------------------------------------------------------------------------
  //Returns true when adding a new collection or a collection that already
  //exists.
  bool add(const smtk::common::UUID& uid,
           const smtk::mesh::Collection& collection)
  {
    const bool is_valid_u = !uid.isNull();
    const bool is_valid_c = collection.isValid();
    const bool not_already_added = this->Collections.count( uid ) == 0;
    const bool can_add = is_valid_u && is_valid_c && not_already_added;

    //do we need to worry about re-parenting?
    if(can_add)
      {
      //we just presume it was added, no need to check the result
      this->Collections.insert( value_type(uid,collection) );
      }

    return can_add || !not_already_added;
  }

  //----------------------------------------------------------------------------
  bool remove( const smtk::common::UUID& uid )
  {
    iterator to_remove = this->Collections.find( uid );
    if(to_remove != this->Collections.end())
      {
      to_remove->second.removeManagerConnection();
      this->Collections.erase( to_remove );
      return true;
      }
    return false;
  }

  //----------------------------------------------------------------------------
  const_iterator begin() const
    { return this->Collections.begin(); }
  iterator begin()
    { return this->Collections.begin(); }

  //----------------------------------------------------------------------------
  const_iterator end() const
    { return this->Collections.end(); }
  iterator end()
    { return this->Collections.end(); }

  //----------------------------------------------------------------------------
  const_iterator find( const smtk::common::UUID& uid) const
    {
    return this->Collections.find( uid );
    }

  //----------------------------------------------------------------------------
  bool has( const smtk::common::UUID& uid) const
    { return this->Collections.find( uid ) != this->Collections.end(); }

  //----------------------------------------------------------------------------
  std::size_t size() const
    { return this->Collections.size(); }

private:
  std::map< smtk::common::UUID, smtk::mesh::Collection > Collections;
};


//----------------------------------------------------------------------------
Manager::Manager():
  m_collector( new InternalStorageImpl() ),
  m_associator( new InternalStorageImpl() ),
  m_uuidGenerator()
{

}

//----------------------------------------------------------------------------
Manager::~Manager()
{ //needs to be in impl file
}

//----------------------------------------------------------------------------
smtk::common::UUID Manager::nextEntityId()
{
  //return the next random uuid
  return this->m_uuidGenerator.random();
}

//----------------------------------------------------------------------------
bool Manager::addCollection(const smtk::mesh::Collection& collection)
{
  //do we need to re-parent the collection?
  return this->m_collector->add(collection.entity(), collection);
}

//----------------------------------------------------------------------------
std::size_t Manager::numberOfCollections() const
{
  return this->m_collector->size();
}

//----------------------------------------------------------------------------
bool Manager::hasCollection( const smtk::mesh::Collection& collection ) const
{
  return this->m_collector->has( collection.entity() );
}

//----------------------------------------------------------------------------
Manager::const_iterator Manager::collectionBegin() const
{
  return this->m_collector->begin();
}

//----------------------------------------------------------------------------
Manager::const_iterator Manager::collectionEnd() const
{
  return this->m_collector->end();
}

//----------------------------------------------------------------------------
Manager::const_iterator Manager::findCollection( const smtk::common::UUID& collectionID ) const
{
  return this->m_collector->find(collectionID);
}

//----------------------------------------------------------------------------
smtk::mesh::Collection Manager::collection( const smtk::common::UUID& collectionID ) const
{
  const_iterator result = this->m_collector->find(collectionID);
  if(result == this->m_collector->end())
    { //returning end() result causes undefined behavior and will generally
      //cause a segfault when you query the item
    return smtk::mesh::Collection();
    }
  return result->second;
}

//----------------------------------------------------------------------------
bool Manager::removeCollection( const smtk::mesh::Collection& collection )
{
  return this->m_collector->remove( collection.entity() );
}

//----------------------------------------------------------------------------
std::size_t Manager::numberOfAssociations() const
{
  return this->m_associator->size();
}

//----------------------------------------------------------------------------
bool Manager::isAssociatedCollection( const smtk::mesh::Collection& collection )
{
  //this is complex as it is the inverse of searching by cursor
  return false;
}

//----------------------------------------------------------------------------
bool Manager::isAssociatedToACollection( const smtk::model::EntityRef& erf ) const
{
  return this->m_associator->has( erf.entity() );
}

//----------------------------------------------------------------------------
Manager::const_iterator Manager::associatedCollectionBegin() const
{
  return this->m_associator->begin();
}

//----------------------------------------------------------------------------
Manager::const_iterator Manager::associatedCollectionEnd() const
{
  return this->m_associator->end();
}

//----------------------------------------------------------------------------
Manager::const_iterator Manager::findAssociatedCollection( const smtk::model::EntityRef& erf ) const
{
  return this->m_associator->find( erf.entity() );
}

//----------------------------------------------------------------------------
smtk::mesh::Collection Manager::associatedCollection( const smtk::model::EntityRef& erf ) const
{
  const_iterator result = this->m_associator->find( erf.entity() );
  return result->second;
}

//----------------------------------------------------------------------------
bool Manager::addAssociation( const smtk::model::EntityRef& erf,
                               const smtk::mesh::Collection& collection)
{
  //do we need to re-parent the collection?
  this->m_collector->add(collection.entity(), collection);
  return this->m_associator->add( erf.entity(), collection );
}

//----------------------------------------------------------------------------
bool Manager::removeAssociation( const smtk::model::EntityRef& erf )
{
  return this->m_associator->remove( erf.entity() );
}


}
}