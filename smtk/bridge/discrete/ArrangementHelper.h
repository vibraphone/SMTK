//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
#ifndef __smtk_bridge_discrete_ArrangementHelper_h
#define __smtk_bridge_discrete_ArrangementHelper_h

#include "smtk/model/ArrangementHelper.h"
#include "smtk/bridge/discrete/discreteSessionExports.h" // for SMTKDISCRETESESSION_EXPORT
#include "smtk/SharedFromThis.h" // for smtkTypeMacro

#include "smtk/model/ArrangementKind.h"
#include "smtk/model/EntityRef.h"

#include <set>

class vtkModelEdge;
class vtkModelEdgeUse;

namespace smtk {
  namespace bridge {
    namespace discrete {

/**\brief Superclass for session-specific updates to arrangments of entities.
  *
  * Subclasses of this class are used by subclasses of the Session class
  * to store session-specific information used to update arrangment information
  * during transcription.
  */
class SMTKDISCRETESESSION_EXPORT ArrangementHelper : public smtk::model::ArrangementHelper
{
public:
  smtkTypeMacro(ArrangementHelper);
  smtkSuperclassMacro(smtk::model::ArrangementHelper);
  ArrangementHelper();
  virtual ~ArrangementHelper();

  void addArrangement(
    const smtk::model::EntityRef& parent,
    smtk::model::ArrangementKind k,
    const smtk::model::EntityRef& child);
  void addArrangement(
    const smtk::model::EntityRef& parent,
    smtk::model::ArrangementKind k,
    const smtk::model::EntityRef& child,
    int sense,
    smtk::model::Orientation orientation);
  void resetArrangements();

  virtual void doneAddingEntities();

  // Start of discrete-session specific methods:
  int findOrAssignSense(vtkModelEdgeUse* eu1, vtkModelEdgeUse* eu2);

protected:

  typedef std::map<vtkModelEdgeUse*,int> EdgeUseToSenseMap;
  typedef std::map<vtkModelEdge*, EdgeUseToSenseMap> EdgeToUseSenseMap;

  struct Spec {
    smtk::model::EntityRef parent;
    smtk::model::EntityRef child;
    smtk::model::ArrangementKind kind;
    int sense;

    Spec()
      : kind(smtk::model::KINDS_OF_ARRANGEMENTS), sense(-1) { }
    Spec(const smtk::model::EntityRef& p, const smtk::model::EntityRef& c, smtk::model::ArrangementKind k)
      : parent(p), child(c), kind(k), sense(-1) { }
    Spec(const smtk::model::EntityRef& p, const smtk::model::EntityRef& c, smtk::model::ArrangementKind k, int s)
      : parent(p), child(c), kind(k), sense(s) { }

    bool operator < (const Spec& other) const
      {
      return
        (this->kind < other.kind ||
         (this->kind == other.kind &&
          (this->parent < other.parent ||
           (this->parent == other.parent &&
            (this->child < other.child ||
             (this->child == other.child &&
              this->sense < other.sense)))))) ? true : false;
      }
  };

  std::set<Spec> m_arrangements;
  EdgeToUseSenseMap m_edgeUseSenses;

private:
  ArrangementHelper(const ArrangementHelper& other); // Not implemented.
  void operator = (const ArrangementHelper& other); // Not implemented.
};

    } // namespace discrete
  } // namespace bridge
} // namespace smtk

#endif // __smtk_bridge_discrete_ArrangementHelper_h