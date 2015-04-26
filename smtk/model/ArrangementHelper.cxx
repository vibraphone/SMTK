//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
#include "smtk/model/ArrangementHelper.h"

#include "smtk/model/EntityRef.h"
#include "smtk/model/Manager.h"

namespace smtk {
  namespace model {

/// Private constructor since this class is a base class which should not be instantiated.
ArrangementHelper::ArrangementHelper()
{
}

ArrangementHelper::~ArrangementHelper()
{
}

/// This method is called after all related entities have been added and before arrangement updates are made.
void ArrangementHelper::doneAddingEntities()
{
}

/// Mark an entity (as having been visited).
void ArrangementHelper::mark(const EntityRef& ent, bool m)
{
  if (m)
    this->m_marked.insert(ent);
  else
    this->m_marked.erase(ent);
}

/// Return whether an entity is marked or not.
bool ArrangementHelper::isMarked(const EntityRef& ent) const
{
  return this->m_marked.find(ent) == this->m_marked.end() ? false : true;
}

/// Clear all the marks so that isMarked() returns false for any argument.
void ArrangementHelper::resetMarks()
{
}

/**\brief Remove all relationships, arrangements, tessellations, and
  *       modeling-kernel-generated properties from the manager.
  *
  * Subclasses should override this method, calling the parent class
  * implementation (i.e., this method) before removing any kernel-specific
  * properties added by their corresponding Session subclass.
  *
  * For example, the Exodus bridge adds the "exodus id" property.
  * Since it will be regenerated by re-transcription, it should be
  * removed in the Exodus session's ArrangementHelper subclass.
  */
void ArrangementHelper::reset(const smtk::model::EntityRef& ent)
{
  EntityRef mutableEnt(ent);

  Entity* erec;
  if (ent.isValid(&erec))
    erec->resetRelations();

  mutableEnt.clearArrangements();

  const Tessellation* tess;
  if ((tess = ent.hasTessellation()))
    mutableEnt.removeTessellation();
}

  } // namespace model
} // namespace smtk
