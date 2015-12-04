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
typedef std::map<internal::Id,LoopInfo> LoopsById;

struct SweepEvent
{
  enum SweepEventType {
    SEGMENT_START,
    SEGMENT_END,
    SEGMENT_CROSS
  };
  SweepEventType m_type;
  internal::Point m_posn;
  smtk::model::Edge m_edge[2];
  int m_indx[2];

  SweepEventType type() const { return this->m_type; }
  const internal::Point& point() const { return this->m_posn; }

  bool operator < (const SweepEvent& other) const
    {
    return
      (this->m_posn.x() < other.point().x() ||
       (this->m_posn.x() == other.point().x() &&
        (this->m_posn.y() < other.point().y() ||
         (this->m_posn.y() == other.point().y() &&
          (this->m_type < other.type() ||
           (this->m_type == other.type() &&
            ( // Types match, perform type-specific comparisons:
             ((this->m_type == SEGMENT_START || this->m_type == SEGMENT_END) &&
              (this->m_edge[0] < other.m_edge[0] ||
               (this->m_edge[0] == other.m_edge[0] && this->m_indx[0] < other.m_indx[0]))) ||
             (this->m_type == SEGMENT_CROSS &&
              (this->m_edge[0] < other.m_edge[0] ||
               (this->m_edge[0] == other.m_edge[0] &&
                (this->m_indx[0] < other.m_indx[0] ||
                 (this->m_indx[0] == other.m_indx[0] &&
                  (this->m_edge[1] < other.m_edge[1] ||
                   (this->m_edge[1] == other.m_edge[1] &&
                    (this->m_indx[1] < other.m_indx[1]
                    ))))))))))))))) ?
      true : false;
    }

  static SweepEvent SegmentStart(const internal::Point& posn, const smtk::model::Edge& edge, int segId)
    {
    SweepEvent event;
    event.m_type = SEGMENT_START;
    event.m_posn = posn;
    event.m_edge[0] = edge;
    event.m_indx[0] = segId;
    event.m_indx[1] = -1;
    return event;
    }
  static SweepEvent SegmentEnd(const internal::Point& posn, const smtk::model::Edge& edge, int segId)
    {
    SweepEvent event;
    event.m_type = SEGMENT_END;
    event.m_posn = posn;
    event.m_edge[0] = edge;
    event.m_indx[0] = segId;
    event.m_indx[1] = -1;
    return event;
    }
  static SweepEvent SegmentCross(const internal::Point& crossPos,
    const smtk::model::Edge& e0, int segId0,
    const smtk::model::Edge& e1, int segId1)
    {
    SweepEvent event;
    event.m_type = SEGMENT_CROSS;
    event.m_posn = crossPos;
    event.m_edge[0] = e0;
    event.m_indx[0] = segId0;
    event.m_edge[1] = e1;
    event.m_indx[1] = segId1;
    return event;
    }
};

#if 0
static void AddLoopsForEdge(
  CreateFaces* op,
  ModelEdgeMapT& modelEdgeMap,
  ModelEdgeMapT::iterator edgeInfo,
  LoopsById& loops,
  smtk::model::VertexSet& visitedVerts,
  std::map<internal::Point, int>& visitedPoints // number of times a point has been encountered (not counting periodic repeat at end of a single-edge loop); used to identify points that must be promoted to model vertices.
)
{
  if (!edgeInfo->first.isValid() || !op)
    {
    return; // garbage-in? garbage-out.
    }
  internal::EdgePtr edgeRec = op->findStorage<internal::edge>(edgeInfo->first.entity());

  smtk::model::Vertices endpts = edgeInfo->first.vertices();
  if (endpts.empty())
    { // Tessellation had better be a periodic loop. Traverse for bbox.
    //AddEdgePointsToBox(tess, box);
    }
  else
    { // Choose an endpoint and walk around the edge.
    }
}
#endif // 0

template<typename T>
void ConditionalErase(T& container, typename T::iterator item, bool shouldErase)
{
  if (shouldErase)
    container.erase(item);
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
  smtk::model::Model model;

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
      model = modelItem->value(0);
      smtk::model::Edges allEdges =
        model.cellsAs<smtk::model::Edges>();
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

  // Create an event queue and populate it with events
  // for each segment of each edge in modelEdgeMap.
  ModelEdgeMapT::iterator modelEdgeIt;
  std::set<SweepEvent> eventQueue; // sorted into a queue by point-x, point-y, event-type, and then event-specific data.
  for (modelEdgeIt = modelEdgeMap.begin(); modelEdgeIt != modelEdgeMap.end(); ++modelEdgeIt)
    {
    std::cout << "Consider " << modelEdgeIt->first.name() << "\n";
    internal::EdgePtr erec =
      this->findStorage<internal::edge>(
        modelEdgeIt->first.entity());

    if (erec->pointsSize() < 2)
      continue; // Do not handle edges with < 2 points.

    internal::PointSeq::const_iterator pit = erec->pointsBegin();
    int seg = 0;
    internal::Point last = *pit;
    for (++pit; pit != erec->pointsEnd(); ++pit, ++seg)
      {
      eventQueue.insert(SweepEvent::SegmentStart(last, modelEdgeIt->first, seg));
      eventQueue.insert(SweepEvent::SegmentEnd(*pit, modelEdgeIt->first, seg - 1));
      }
    }

  // The first event in eventQueue had better be a segment-start event.
  // So the first thing this event-loop should do is start processing edges.
  // As other edges are added, they must intersect all active edges
  // and add split events as required.
  std::set<SweepEvent>::iterator event;
  internal::Point sweepPosn = eventQueue.begin()->point();

  // Set the initial sweepline to before the beginning of the queue.
  sweepPosn.x(sweepPosn.x() - 1);
  bool shouldErase;
  for (
    event = eventQueue.begin();
    (event = eventQueue.begin()) != eventQueue.end();
    ConditionalErase(eventQueue, event, shouldErase))
    {
    shouldErase = true;
    if (event->point() != sweepPosn)
      {
      // Update sweep position. TODO: Need to do anything special here?
      sweepPosn = event->point();
      }
    std::cout
      << "Event " << event->type() << " posn " << event->point().x() << " " << event->point().y()
      << " edge " << event->m_edge[0].name() << " segment " << event->m_indx[0]
      << "\n";
    switch (event->type())
      {
    case SweepEvent::SEGMENT_START:
      // Add to active edges:
      //   Test for intersection with existing edges
      //     If any, add SEGMENT_CROSS events.
      //   Add to list in proper place
      // If the edge is neighbors others in the active list, either:
      //   a. Add
      break;
    case SweepEvent::SEGMENT_END:
      break;
    case SweepEvent::SEGMENT_CROSS:
      break;
      }
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
