//=============================================================================
// Copyright (c) Kitware, Inc.
// All rights reserved.
// See LICENSE.txt for details.
//
// This software is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
// PURPOSE.  See the above copyright notice for more information.
//=============================================================================
#include "smtk/bridge/polygon/operators/CreateFaces.h"

#include "smtk/bridge/polygon/Session.h"
#include "smtk/bridge/polygon/internal/Config.h"
#include "smtk/bridge/polygon/internal/Model.h"
#include "smtk/bridge/polygon/internal/Edge.h"

#include "smtk/io/Logger.h"

#include "smtk/attribute/Attribute.h"
#include "smtk/attribute/DoubleItem.h"
#include "smtk/attribute/IntItem.h"
#include "smtk/attribute/ModelEntityItem.h"
#include "smtk/attribute/StringItem.h"

#include "smtk/bridge/polygon/CreateFaces_xml.h"

#include <set>

namespace smtk {
  namespace bridge {
    namespace polygon {

/// An internal structure used when discovering edge loops.
struct ModelEdgeInfo
{
  ModelEdgeInfo()
    : m_allowedOrientations(0)
    {
    this->m_visited[0] = this->m_visited[1] = false;
    }
  ModelEdgeInfo(int allowedOrientations)
    {
    this->m_allowedOrientations = allowedOrientations > 0 ? +1 : allowedOrientations < 0 ? -1 : 0;
    this->m_visited[0] = this->m_visited[1] = false;
    }
  ModelEdgeInfo(const ModelEdgeInfo& other)
    : m_allowedOrientations(other.m_allowedOrientations)
    {
    for (int i = 0; i < 2; ++i)
      m_visited[i] = other.m_visited[i];
    }

  int m_allowedOrientations; // 0: all, -1: only negative, +1: only positive
  bool m_visited[2]; // has the [0]: negative, [1]: positive orientation of the edge been visited already?
};

/// An internal structure used to map model edges to information about the space between them.
typedef std::map<smtk::model::Edge, ModelEdgeInfo> ModelEdgeMapT;

/// An internal structure used to hold a sequence of model edges which form a loop.
struct LoopInfo
{
  internal::Id m_id;
  internal::Rect m_bounds;
  smtk::model::Edges m_edges;
  std::set<internal::Id> m_children; // first-level holes
  bool operator < (const LoopInfo& other)
    {
    return ll(this->m_bounds) < ur(other.m_bounds);
    }
};

/// An internal structure that holds all the loops discovered, sorted by their lower-left bounding box coordinates.
typedef std::multiset<LoopInfo> LoopsByBox;

static void AddLoopsForEdge(
  Session* polySession,
  ModelEdgeMapT& modelEdgeMap,
  ModelEdgeMapT::iterator edgeInfo,
  LoopsByBox& loops,
  smtk::model::VertexSet& visitedVerts,
  std::map<internal::Point, int>& visitedPoints // number of times a point has been encountered (not counting periodic repeat at end of a single-edge loop); used to identify points that must be promoted to model vertices.
)
{
  if (!edgeInfo->first.isValid() || !polySession)
    {
    return; // garbage-in? garbage-out.
    }
  internal::EdgePtr edgeRec = polySession->findStorage<internal::edge>(edgeInfo->first.entity());

  smtk::model::Vertices endpts = edgeInfo->first.vertices();
  if (endpts.empty())
    { // Tessellation had better be a periodic loop. Traverse for bbox.
    //AddEdgePointsToBox(tess, box);
    }
  else
    { // Choose an endpoint and walk around the edge.
    }
}

smtk::model::OperatorResult CreateFaces::operateInternal()
{
  // Discover how the user wants to specify scaling.
  smtk::attribute::IntItem::Ptr constructionMethodItem = this->findInt("construction method");
  int method = constructionMethodItem->discreteIndex(0);

  smtk::attribute::DoubleItem::Ptr pointsItem = this->findDouble("points");
  smtk::attribute::IntItem::Ptr coordinatesItem = this->findInt("coordinates");
  smtk::attribute::IntItem::Ptr offsetsItem = this->findInt("offsets");

  smtk::attribute::ModelEntityItem::Ptr edgesItem = this->findModelEntity("edges");

  smtk::attribute::ModelEntityItem::Ptr modelItem = this->specification()->associations();

  internal::pmodel::Ptr storage; // Look up from session = internal::pmodel::create();
  bool ok = true;

  Session* sess = this->polygonSession();
  smtk::model::ManagerPtr mgr;
  if (!sess || !(mgr = sess->manager()))
    {
    // error logging requires mgr...
    return this->createResult(smtk::model::OPERATION_FAILED);
    }
  // Keep a set of model edges marked by the directions in which they
  // should be used to form faces. This will constrain what faces
  // may be created without requiring users to pick a point interior
  // to the face.
  //
  // This way, when users specify oriented (CCW) point sequences or
  // a preferred set of edges as outer loop + inner loops, we don't
  // create faces that fill the holes.
  // But when users specify that all possible faces should be created,
  // they don't have to pick interior points.
  //
  // -1 = use only negative orientation
  //  0 = no preferred direction: use in either or both directions
  // +1 = use only positive orientation
  ModelEdgeMapT modelEdgeMap;

  // First, collect or create edges to process:
  // These case values match CreateFaces.sbt indices (and enum values):
  switch (method)
    {
  case 0: // points, coordinates, offsets
      {
      // identify pre-existing model vertices from points
      // verify that existing edges/faces incident to model vertices
      //        do not impinge on proposed edge/face
      // run edge-creation pre-processing on each point-sequence?
      // determine sub-sequences of points that will
      //   (a) form new edges
      //   (b) make use of existing edges (with orientation)
      // determine loop nesting and edge splits required by intersecting loops
      // report point sequences, model vertices (existing, imposed by intersections, non-manifold), loops w/ nesting
      // ---
      // create new vertices as required
      // create edges on point sequences
      // modify/create vertex uses
      // create chains
      // create edge uses
      // create loops
      // create faces
      }
    break;
  case 1: // edges, points, coordinates
      {
      // for each edge
      //   for each model vertex
      //     walk loops where vertices have no face, aborting walk if an unselected edge is found.
      //     mark traversed regions and do not re-traverse
      //   OR IF NO MODEL VERTICES
      //     edge must be periodic and oriented properly... treat it as a loop to bound a hole+/^face-filling-the-hole.
      //     mark traversed regions and do not re-traverse
      //   determine loop nesting and edge splits required by intersecting loops
      //   report model vertices (imposed by intersections, non-manifold), loops w/ nesting
      // ---
      // create new vertices as required
      // modify vertex uses
      // create edge uses
      // create loops
      // create faces
      }
    break;
  case 2: // all non-overlapping
      {
      smtk::model::Edges allEdges =
        mgr->entitiesMatchingFlagsAs<smtk::model::Edges>(smtk::model::EDGE, /* exactMatch */ true);
      for (smtk::model::Edges::const_iterator it = allEdges.begin(); it != allEdges.end(); ++it)
        {
        modelEdgeMap[*it] = 0;
        }
      // Same as case 1 but with the set of all edges in model.
      //
      // Create a union-find struct
      // for each "model" vertex
      //   for each edge attached to each vertex
      //     add 2 union-find entries (UFEs), 1 per co-edge
      //     merge adjacent pairs of UFEs
      //     store UFEs on edges
      // For each loop, discover nestings and merge UFEs
      // For each edge
      //   For each unprocessed (nesting-wise) UFE
      //     Discover nesting via ray test
      //     Merge parent and child UFEs (if applicable)
      //     Add an (edge, coedge sign) tuple to a "face" identified by the given UFE
      // FIXME: Test for self-intersections?
      // FIXME: Deal w/ pre-existing faces?
      }
    break;
  default:
    ok = false;
    smtkInfoMacro(log(), "Unhandled construction method " << method << ".");
    break;
    }

  // For each model vertex of each collected edge (in modelEdgeMap), traverse edges to form loops.
  // Only loops whose edges are **all** in modelEdgeMap (with acceptable orientations) are included.
  // Only the "tightest possible" loops are created; each model vertex has an ordered list of edge
  // incidences and loops will be formed by visiting neighboring pairs so that no loop is bisected
  // by an interior edge.
  //
  // For each edge with 0 model vertices, verify that the edge is a loop and traverse to find bounds.
  // These edges always result in a new entry in loops.
  //
  // Keep the lower-leftmost and upper-rightmost points (not just model verts) of each loop.
  // The "children" member of LoopInfo records is not populated by this pass.
  LoopsByBox loops;
  smtk::model::VertexSet visitedVerts; // Model vertices whose edges are already being processed.
  std::map<internal::Point, int> visitedPoints;
  ModelEdgeMapT::iterator modelEdgeIt;
  for (modelEdgeIt = modelEdgeMap.begin(); modelEdgeIt != modelEdgeMap.end(); ++modelEdgeIt)
    {
    AddLoopsForEdge(
      sess,
      modelEdgeMap,
      modelEdgeIt,
      loops,
      visitedVerts,
      visitedPoints);
    }

  // Run a sweep line over the bounding points of each loop
  // to determine nesting, split intersecting edges, and detect
  // model vertices implied by edges that do not intersect but
  // do share a coincident point on the same loop/face.

  // Loops whose bboxes intersect the sweep-ray, sorted by when they become inactive:
  std::multimap<internal::Point, LoopsByBox::iterator> activeLoops;
  LoopsByBox::iterator lit = loops.begin();
  if (!loops.empty())
    {
    activeLoops.insert(std::make_pair(ur(lit->m_bounds), lit));
    }
  while (!activeLoops.empty())
    {
    // Push any loops at or beyond lit onto the stack whose lower bounds
    // are below the point where the next active loop retires
    // (i.e., activeLoops.begin()->first).
    // As we push loops (making them active), test:
    //   a. whether they intersect active loop edges or points (requiring edge-split)
    //   b. how loop is nested
    }

  // Create vertex-use, chain, edge-use, loop, and face records

  smtk::model::OperatorResult result;
  if (ok)
    {
    result = this->createResult(smtk::model::OPERATION_SUCCEEDED);
    //this->addEntityToResult(result, model, CREATED);
    }

  if (!result)
    {
    result = this->createResult(smtk::model::OPERATION_FAILED);
    }

  return result;
}

    } // namespace polygon
  } //namespace bridge
} // namespace smtk

smtkImplementsModelOperator(
  SMTKPOLYGONSESSION_EXPORT,
  smtk::bridge::polygon::CreateFaces,
  polygon_create_faces,
  "create faces",
  CreateFaces_xml,
  smtk::bridge::polygon::Session);
