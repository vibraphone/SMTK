//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================

#include "smtk/io/OperatorLog.h"

#include "smtk/io/AttributeReader.h"
#include "smtk/io/Logger.h"

#include "smtk/model/Operator.h"

#include "smtk/attribute/Attribute.h"
#include "smtk/attribute/IntItem.h"
#include "smtk/attribute/System.h"

#include "boost/algorithm/string.hpp"

using namespace smtk::model;

namespace smtk {
  namespace io {

OperatorLog::OperatorLog(smtk::model::ManagerPtr mgr)
  : m_hasFailures(false), m_manager(mgr)
{
  mgr->observe(smtk::model::CREATED_OPERATOR, OperatorLog::operatorCreated, this);
}

OperatorLog::~OperatorLog()
{
  smtk::model::ManagerPtr mgr = this->m_manager.lock();
  if (mgr)
    mgr->unobserve(smtk::model::CREATED_OPERATOR, OperatorLog::operatorCreated, this);

  smtk::model::OperatorPtr watched;
  WeakOpArray::iterator it;
  for (it = this->m_watching.begin(); it != this->m_watching.end(); ++it)
    {
    if ((watched = it->lock()))
      {
      watched->unobserve(smtk::model::WILL_OPERATE, OperatorLog::operatorInvoked, this);
      watched->unobserve(smtk::model::DID_OPERATE, OperatorLog::operatorReturned, this);
      }
    }
}

/// Return whether any operator has failed since the last call to resetFailures().
bool OperatorLog::hasFailures() const
{
  return this->m_hasFailures;
}

/// Tell the log that previous failures have been handled by the application.
void OperatorLog::resetFailures()
{
  this->m_hasFailures = false;
}

/**\brief Return the attribute system that holds hints for operator items.
  *
  * If no attribute system exists, one is created by calling prepareHintSystem().
  */
smtk::attribute::SystemPtr OperatorLog::hintSystem()
{
  if (!this->m_hintSys)
    this->prepareHintSystem();
  return this->m_hintSys;
}

smtk::attribute::AttributePtr OperatorLog::createHint(const std::string& hintDef)
{
  return this->hintSystem()->createAttribute(hintDef);
}

void OperatorLog::addHintForItem(const std::string& itemPath, smtk::attribute::AttributePtr hint, const std::string& sep)
{
  smtk::model::StringList separatedPath;
  boost::split(separatedPath, itemPath, boost::is_any_of(sep));
  this->addHintForItem(separatedPath, hint);
}

void OperatorLog::addHintForItem(const StringList& itemPath, smtk::attribute::AttributePtr hint)
{
  HintedItem key;
  key.Operator = "foo";
  key.Path = itemPath;
  this->m_hints[key].insert(hint);
}

std::set<smtk::attribute::AttributePtr> OperatorLog::hintsForItem(const std::string& itemPath, const std::string& sep) const
{
  smtk::model::StringList separatedPath;
  boost::split(separatedPath, itemPath, boost::is_any_of(sep));
  return this->hintsForItem(separatedPath);
}

std::set<smtk::attribute::AttributePtr> OperatorLog::hintsForItem(const smtk::model::StringList& itemPath) const
{
  HintedItem key;
  key.Path = itemPath;
  HintMap::const_iterator it = this->m_hints.find(key);
  if (it != this->m_hints.end())
    return it->second;
  return std::set<smtk::attribute::AttributePtr>();
}

void OperatorLog::resetHint(const std::string& itemPath, const std::string& sep)
{
  smtk::model::StringList separatedPath;
  boost::split(separatedPath, itemPath, boost::is_any_of(sep));
  this->resetHint(separatedPath);
}

void OperatorLog::resetHint(const smtk::model::StringList& itemPath)
{
  HintedItem key;
  key.Path = itemPath;
  this->m_hints.erase(key);
}

void OperatorLog::resetHints()
{
  this->m_hints.clear();
}

#include "smtk/io/OperatorLogHints_xml.h"

/**\brief Create an attribute system to hold hints and add definitions.
  *
  * Subclasses may override this method to change the base definitions
  * so they match the type of log being prepared.
  */
void OperatorLog::prepareHintSystem()
{
  this->m_hintSys = smtk::attribute::SystemPtr(new smtk::attribute::System);

  // Now read definitions into the system from a string.
  smtk::io::Logger tmpLog;
  smtk::io::AttributeReader rdr;
  bool ok = !rdr.readContents(
    *this->m_hintSys.get(),
    OperatorLogHints_xml, strlen(OperatorLogHints_xml),
    tmpLog);
  if (!ok)
    {
    std::cerr
      << "Error initializing hint system. Log follows:\n---\n"
      << tmpLog.convertToString()
      << "\n---\n";
    }
}

int OperatorLog::operatorCreated(
  smtk::model::OperatorEventType event,
  const smtk::model::Operator& op,
  void* data)
{
  OperatorLog* self = reinterpret_cast<OperatorLog*>(data);
  if (!self || event != CREATED_OPERATOR)
    return 0; // Don't stop an operation just because the recorder is unhappy.

  OperatorPtr oper = smtk::const_pointer_cast<Operator>(op.shared_from_this());
  self->m_watching.push_back(oper);
  oper->observe(smtk::model::WILL_OPERATE, OperatorLog::operatorInvoked, self);
  oper->observe(smtk::model::DID_OPERATE, OperatorLog::operatorReturned, self);
  return 0;
}

int OperatorLog::operatorInvoked(
  smtk::model::OperatorEventType event,
  const smtk::model::Operator& op,
  void* data)
{
  OperatorLog* self = reinterpret_cast<OperatorLog*>(data);
  if (!self)
    return 0; // Don't stop an operation just because the recorder is unhappy.

  return self->recordInvocation(event, op);
}

int OperatorLog::operatorReturned(
  smtk::model::OperatorEventType event,
  const smtk::model::Operator& op,
  smtk::model::OperatorResult r,
  void* data)
{
  OperatorLog* self = reinterpret_cast<OperatorLog*>(data);
  if (!self)
    return 0; // Don't stop an operation just because the recorder is unhappy.

  self->m_hasFailures |=
    (r->findInt("outcome")->value(0) == smtk::model::OPERATION_FAILED);
  return self->recordResult(event, op, r);
}

  } // namespace io
} // namespace smtk
