//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
#include "smtk/bridge/discrete/ArrangementHelper.h"

#include "smtk/model/ArrangementKind.h"
#include "smtk/model/Manager.h"

namespace smtk {
  namespace bridge {
    namespace discrete {

/// Private constructor since this class is a base class which should not be instantiated.
ArrangementHelper::ArrangementHelper()
{
}

ArrangementHelper::~ArrangementHelper()
{
}

void ArrangementHelper::addArrangement(
  const smtk::model::EntityRef& parent,
  smtk::model::ArrangementKind k,
  const smtk::model::EntityRef& child)
{
  // FIXME: if arrangement already exists, do not queue it.
  this->m_arrangements.insert(Spec(parent, child, k));
}

void ArrangementHelper::addArrangement(
  const smtk::model::EntityRef& parent,
  smtk::model::ArrangementKind k,
  const smtk::model::EntityRef& child,
  int sense,
  smtk::model::Orientation orientation)
{
  // FIXME: if arrangement already exists, do not queue it.
  this->m_arrangements.insert(
    Spec(
      parent,
      child,
      k,
      2 * sense + (orientation == smtk::model::POSITIVE ? 1 : 0)));
}

void ArrangementHelper::resetArrangements()
{
  this->m_arrangements.clear();
}

/// This method is called after all related entities have been added and before arrangement updates are made.
void ArrangementHelper::doneAddingEntities()
{
  std::set<Spec>::iterator it;
  for (it = this->m_arrangements.begin(); it != this->m_arrangements.end(); ++it)
    {
    std::cout
      << "Add " << it->parent.flagSummary(0)
      << " " << smtk::model::NameForArrangementKind(it->kind)
      << " " << it->child.flagSummary(0) << "\n";
    if (it->parent.manager() != it->child.manager())
      {
      std::cerr << "  Mismatched or nil managers. Skipping.\n";
      }
    it->parent.manager()->addDualArrangement(
      it->parent.entity(), it->child.entity(),
      it->kind,
      /* sense */ it->sense / 2,
      it->sense % 2 ? smtk::model::POSITIVE : smtk::model::NEGATIVE);
    }
}

    } // namespace discrete
  } // namespace bridge
} // namespace smtk
