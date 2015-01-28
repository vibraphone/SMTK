//=============================================================================
//
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//
//=============================================================================

#include "smtk/mesh/moab/Interface.h"
#include "smtk/mesh/moab/CellTypeToType.h"
#include "smtk/mesh/moab/Tags.h"
#include "smtk/mesh/moab/Functors.h"

#include "smtk/mesh/MeshSet.h"
#include "smtk/mesh/QueryTypes.h"
#include "smtk/mesh/TypeSet.h"

#include <algorithm>
#include <cstring>
#include <set>
#include <vector>

namespace smtk {
namespace mesh {
namespace moab {

//----------------------------------------------------------------------------
std::size_t numMeshes(smtk::mesh::Handle handle,
                      const smtk::mesh::moab::InterfacePtr& iface)
{
  int num_ents = 0;
  iface->get_number_entities_by_type( handle, ::moab::MBENTITYSET, num_ents);
  return static_cast<std::size_t>(num_ents);
}

//----------------------------------------------------------------------------
bool create_meshset(smtk::mesh::HandleRange cells,
                    smtk::mesh::Handle& meshHandle,
                    const smtk::mesh::moab::InterfacePtr& iface)
{
  const unsigned int options = 0;
  ::moab::ErrorCode rval = iface->create_meshset( options , meshHandle );
  if(rval == ::moab::MB_SUCCESS)
    {
    iface->add_entities( meshHandle, cells );
    }
   return (rval == ::moab::MB_SUCCESS);
}

//----------------------------------------------------------------------------
smtk::mesh::HandleRange get_meshsets(smtk::mesh::Handle handle,
                                     const smtk::mesh::moab::InterfacePtr& iface)

{
  ::moab::Range range;
  iface->get_entities_by_type(handle, ::moab::MBENTITYSET, range);
  return range;
}

//----------------------------------------------------------------------------
smtk::mesh::HandleRange get_meshsets(smtk::mesh::Handle handle,
                                     int dimension,
                                     const smtk::mesh::moab::InterfacePtr& iface)

{
  ::moab::Range all_meshes_with_dim_tag;
  ::moab::Range meshes_of_proper_dim;

  //construct a dim tag that matches the dimension coming in
  tag::QueryDimTag dimTag(dimension, iface);

  // get all the entities of that type in the mesh
  iface->get_entities_by_type_and_tag(handle,
                                      ::moab::MBENTITYSET,
                                      dimTag.moabTag(),
                                      NULL,
                                      1,
                                      all_meshes_with_dim_tag);

  typedef ::moab::Range::const_iterator iterator;
  for(iterator i=all_meshes_with_dim_tag.begin();
      i != all_meshes_with_dim_tag.end(); ++i)
    {
    int value = 0;
    iface->tag_get_data(dimTag.moabTagAsRef(), &(*i), 1, &value);
    if(value == dimTag.value())
      {
      meshes_of_proper_dim.insert(*i);
      }
    }
  return meshes_of_proper_dim;
}

//----------------------------------------------------------------------------
//find all entity sets that have this exact name tag
smtk::mesh::HandleRange get_meshsets(smtk::mesh::Handle handle,
                                     const std::string& name,
                                     const smtk::mesh::moab::InterfacePtr& iface)

{
  typedef std::vector< ::moab::EntityHandle >::const_iterator it;

  //I can't get get_entities_by_type_and_tag to work properly for this
  //query so I am going to do it the slow way by doing the checking manually

  //use a vector since we are going to do single element iteration, and
  //removal.
  std::vector< ::moab::EntityHandle > all_ents;
  std::vector< ::moab::EntityHandle > matching_ents;
  //get all ents
  iface->get_entities_by_type(handle, ::moab::MBENTITYSET, all_ents);

  //see which ones have a a matching name, and if so add it
  tag::QueryNameTag query_name(iface);
  for( it i = all_ents.begin(); i != all_ents.end(); ++i )
    {
    const bool has_name = query_name.fetch_name(*i);
    if(has_name &&
       ( std::strcmp(name.c_str(), query_name.current_name()) == 0 ) )
      { //has a matching name
      matching_ents.push_back(*i);
      }
    }

  all_ents.clear();

  smtk::mesh::HandleRange result;
  std::copy( matching_ents.rbegin(), matching_ents.rend(),
             ::moab::range_inserter(result) );
  return result;
}

//----------------------------------------------------------------------------
//get all cells held by this range
smtk::mesh::HandleRange get_cells(smtk::mesh::HandleRange meshsets,
                                  const smtk::mesh::moab::InterfacePtr& iface)

{
  // get all non-meshset entities in meshset, including in contained meshsets
  typedef ::moab::Range::const_iterator iterator;
  ::moab::Range entitiesCells;
  for(iterator i = meshsets.begin(); i != meshsets.end(); ++i)
    {
    //get_entities_by_handle appends to the range given
    iface->get_entities_by_handle(*i, entitiesCells, true);
    }
  return entitiesCells;
}


//----------------------------------------------------------------------------
//get all cells held by this range handle of a given cell type
smtk::mesh::HandleRange get_cells(smtk::mesh::HandleRange meshsets,
                                  smtk::mesh::CellType cellType,
                                  const smtk::mesh::moab::InterfacePtr& iface)
{
  int moabCellType = smtk::mesh::moab::smtkToMOABCell(cellType);

  ::moab::Range entitiesCells;

  // get all non-meshset entities in meshset of a given cell type
  typedef ::moab::Range::const_iterator iterator;
  for(iterator i = meshsets.begin(); i != meshsets.end(); ++i)
    {
    //get_entities_by_type appends to the range given
    iface->get_entities_by_type(*i,
                                static_cast< ::moab::EntityType >(moabCellType),
                                entitiesCells,
                                true);
    }
  return entitiesCells;
}

//----------------------------------------------------------------------------
//get all cells held by this range handle of a given cell type(s)
smtk::mesh::HandleRange get_cells(smtk::mesh::HandleRange meshsets,
                                  const smtk::mesh::CellTypes& cellTypes,
                                  const smtk::mesh::moab::InterfacePtr& iface)

{
  const std::size_t cellTypesToFind = cellTypes.count();
  if( cellTypesToFind == cellTypes.size())
    { //if all the cellTypes are enabled we should just use get_cells
      //all() method can't be used as it was added in C++11
    return get_cells( meshsets, iface);
    }
  else if(cellTypesToFind == 0)
    {
    return smtk::mesh::HandleRange();
    }

  //we now search from highest cell type to lowest cell type adding everything
  //to the range. The reason for this is that ranges perform best when inserting
  //from high to low values
  ::moab::Range entitiesCells;
  for(int i = (cellTypes.size() -1); i >= 0; --i )
    {
    //skip all cell types we don't have
    if( !cellTypes[i] )
      { continue; }

    smtk::mesh::CellType currentCellType = static_cast<smtk::mesh::CellType>(i);

    ::moab::Range cellEnts = get_cells(meshsets, currentCellType, iface);

    entitiesCells.insert(cellEnts.begin(), cellEnts.end());
    }

  return entitiesCells;
}

//----------------------------------------------------------------------------
//get all cells held by this range handle of a given dimension
smtk::mesh::HandleRange get_cells(smtk::mesh::HandleRange meshsets,
                                  smtk::mesh::DimensionType dim,
                                  const smtk::mesh::moab::InterfacePtr& iface)

{
  const int dimension = static_cast<int>(dim);

  //get all non-meshset entities of a given dimension
  typedef ::moab::Range::const_iterator iterator;
  ::moab::Range entitiesCells;
  for(iterator i = meshsets.begin(); i != meshsets.end(); ++i)
    {
    //get_entities_by_dimension appends to the range given
    iface->get_entities_by_dimension(*i, dimension, entitiesCells, true);
    }
  return entitiesCells;
}


//----------------------------------------------------------------------------
std::vector< std::string > compute_names(const smtk::mesh::HandleRange& r,
                                         const smtk::mesh::moab::InterfacePtr& iface)
{
  //construct a name tag query helper class
  tag::QueryNameTag query_name(iface);

  typedef ::moab::Range::const_iterator it;
  std::set< std::string > unique_names;
  for(it i = r.begin(); i != r.end(); ++i)
    {
    const bool has_name = query_name.fetch_name(*i);
    if(has_name)
      {
      unique_names.insert( std::string(query_name.current_name()) );
      }
    }
  //return a vector of the unique names
  return std::vector< std::string >(unique_names.begin(), unique_names.end());
}

//----------------------------------------------------------------------------
smtk::mesh::TypeSet compute_types(smtk::mesh::Handle handle,
                                  const smtk::mesh::moab::InterfacePtr& iface)
{
  int numMeshes = 0;
  iface->get_number_entities_by_type( handle, ::moab::MBENTITYSET, numMeshes);

  //iterate over all the celltypes and get the number for each
  //construct a smtk::mesh::CellTypes at the same time
  typedef ::smtk::mesh::CellType CellEnum;
  smtk::mesh::CellTypes ctypes;
  if(numMeshes > 0)
    {
    for(int i=0; i < ctypes.size(); ++i )
      {
      CellEnum ce = static_cast<CellEnum>(i);
      //now we need to convert from CellEnum to MoabType
      int moabEType = smtk::mesh::moab::smtkToMOABCell(ce);

      //some of the cell types that smtk supports moab doesn't support
      //so we can't query on those.
      int num = 0;
      iface->get_number_entities_by_type(handle,
                                         static_cast< ::moab::EntityType >(moabEType),
                                         num);
      ctypes[ce] = (num > 0);
      }
    }

  //determine the state of the typeset
  const bool hasMeshes = numMeshes > 0;
  const bool hasCells = ctypes.any();
  return smtk::mesh::TypeSet(ctypes, hasMeshes, hasCells) ;
}

//----------------------------------------------------------------------------
smtk::mesh::HandleRange point_intersect(const smtk::mesh::HandleRange& a,
                                        const smtk::mesh::HandleRange& b,
                                        const smtk::mesh::moab::ContainsFunctor& containsFunctor,
                                        const smtk::mesh::moab::InterfacePtr& iface)
{
  if(a.empty() || b.empty())
    { //the intersection with nothing is nothing
    return smtk::mesh::HandleRange();
    }

  //first get all the points of a
  ::moab::Range a_points; iface->get_connectivity(a, a_points);

  //result storage for creating the range. This is used since inserting
  //into a range is horribly slow
  std::vector< ::moab::EntityHandle > vresult;
  vresult.reserve( b.size() );

  //Some elements (e.g. structured mesh) may not have an explicit connectivity list.
  //we pass storage to the interface so that it can use that memory to construct
  //an explicit connectivity list for us.
  std::vector< ::moab::EntityHandle > storage;

  typedef ::moab::Range::const_iterator c_it;
  for(c_it i = b.begin(); i != b.end(); ++i)
    {
    const ::moab::EntityHandle* connectivity; //handle back to node list
    int num_nodes; //tells us the number of nodes
    const bool corners_only = false; //explicitly state we want all nodes of the cell

    //grab the raw connectivity array so we don't waste any memory
    iface->get_connectivity(*i, connectivity, num_nodes, corners_only, &storage);

    //call the contains functor to determine if this cell is considered
    //to be contained by a_points.
    bool contains = containsFunctor(a_points, connectivity, num_nodes);
    if(contains)
      { vresult.push_back( *i ); }
    }

  //now that we have all the cells that are the partial intersection
  ::moab::Range resulting_range;
  ::moab::Range::iterator hint = resulting_range.begin();

  const std::size_t size = vresult.size();
  for(std::size_t i = 0; i < size;)
    {
    std::size_t j;
    for(j = i + 1; j < size && vresult[j] == 1 + vresult[j-1]; j++);
      //empty for loop
    hint = resulting_range.insert( hint, vresult[i], vresult[i] + (j-i-1) );
    i = j;
    }

  return resulting_range;

}

//----------------------------------------------------------------------------
smtk::mesh::HandleRange point_difference(const smtk::mesh::HandleRange& a,
                                         const smtk::mesh::HandleRange& b,
                                         const smtk::mesh::moab::ContainsFunctor& containsFunctor,
                                         const smtk::mesh::moab::InterfacePtr& iface)
{
  if(b.empty())
    { //taking the difference from nothing results in nothing
    return smtk::mesh::HandleRange();
    }
  else if(a.empty())
    { //if a is empty that means all b of is the difference
    return b;
    }

  //first get all the points of a
  ::moab::Range a_points; iface->get_connectivity(a, a_points);

  //result storage for creating the range. This is used since inserting
  //into a range is horribly slow
  std::vector< ::moab::EntityHandle > vresult;
  vresult.reserve( b.size() );

  //Some elements (e.g. structured mesh) may not have an explicit connectivity list.
  //we pass storage to the interface so that it can use that memory to construct
  //an explicit connectivity list for us.
  std::vector< ::moab::EntityHandle > storage;

  typedef ::moab::Range::const_iterator c_it;
  for(c_it i = b.begin(); i != b.end(); ++i)
    {
    const ::moab::EntityHandle* connectivity; //handle back to node list
    int num_nodes; //tells us the number of nodes
    const bool corners_only = false; //explicitly state we want all nodes of the cell

    //grab the raw connectivity array so we don't waste any memory
    iface->get_connectivity(*i, connectivity, num_nodes, corners_only, &storage);

    //call the contains functor to determine if this cell is considered
    //to be contained by a_points. If we aren't contained than we go into
    //the difference result
    bool contains = containsFunctor(a_points, connectivity, num_nodes);
    if(!contains)
      { vresult.push_back( *i ); }
    }

  //now that we have all the cells that are the partial intersection
  ::moab::Range resulting_range;
  ::moab::Range::iterator hint = resulting_range.begin();

  const std::size_t size = vresult.size();
  for(std::size_t i = 0; i < size;)
    {
    std::size_t j;
    for(j = i + 1; j < size && vresult[j] == 1 + vresult[j-1]; j++);
      //empty for loop
    hint = resulting_range.insert( hint, vresult[i], vresult[i] + (j-i-1) );
    i = j;
    }

  return resulting_range;

}

}
}
}