//=============================================================================
// Copyright (c) Kitware, Inc.
// All rights reserved.
// See LICENSE.txt for details.
//
// This software is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
// PURPOSE.  See the above copyright notice for more information.
//=============================================================================
#ifndef __smtk_bridge_polygon_internal_bpConfig_h
#define __smtk_bridge_polygon_internal_bpConfig_h
#ifndef SHIBOKEN_SKIP

#include "smtk/SharedPtr.h"
#include "smtk/common/UUID.h"

#ifndef _MSC_VER
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wc99-extensions"
#  pragma GCC diagnostic ignored "-Wvariadic-macros"
#endif
#include "boost/polygon/polygon.hpp"
#ifndef _MSC_VER
#  pragma GCC diagnostic pop
#endif

#include <list>
#include <map>
#include <set>

namespace smtk {
  namespace bridge {
    namespace polygon {
      namespace internal {

        class entity;
        class vertex;
        class edge;
        class face;
        class pmodel;

        typedef smtk::shared_ptr<entity> EntityPtr;
        typedef smtk::shared_ptr<vertex> VertexPtr;
        typedef smtk::shared_ptr<edge> EdgePtr;
        typedef smtk::shared_ptr<face> FacePtr;

        typedef long long Coord;
        typedef smtk::common::UUID Id;
        typedef boost::polygon::point_data<Coord> Point;
        typedef boost::polygon::segment_data<Coord> Segment;
        typedef boost::polygon::interval_data<Coord> Interval;
        typedef std::map<Point,Id> PointToVertexId;
        typedef std::map<Id,EntityPtr> EntityIdToPtr;
        typedef std::list<Point> PointSeq;
        typedef std::map<Point,VertexPtr> VertexById;

      } // namespace internal
    } // namespace polygon
  } // namespace bridge
} // namespace smtk

#endif // SHIBOKEN_SKIP
#endif // __smtk_bridge_polygon_internal_bpConfig_h
