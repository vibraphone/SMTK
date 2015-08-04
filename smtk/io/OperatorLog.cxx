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
#include "smtk/io/LogProcessor.h"

#include "smtk/model/Operator.h"

#include "smtk/attribute/Attribute.h"
#include "smtk/attribute/DoubleItem.h"
#include "smtk/attribute/IntItem.h"
#include "smtk/attribute/StringItem.h"
#include "smtk/attribute/System.h"

#include "boost/algorithm/string.hpp"

using namespace smtk::model;

namespace smtk {
  namespace io {

static OperatorLog* ActiveOperatorLog = NULL;

OperatorLog::OperatorLog(smtk::model::ManagerPtr mgr)
  : m_hasFailures(false), m_manager(mgr)
{
  if (!ActiveOperatorLog)
    ActiveOperatorLog = this;

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
  if (ActiveOperatorLog == this)
    ActiveOperatorLog = NULL;
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

smtk::attribute::AttributePtr OperatorLog::createHint(const std::string& hintDefName, const std::string& context)
{
  smtk::attribute::AttributePtr hint = this->hintSystem()->createAttribute(hintDefName);
  if (!context.empty())
    hint->findString("context")->setValue(0, context);
  return hint;
}

void OperatorLog::addHintForItem(const std::string& itemPath, smtk::attribute::AttributePtr hint, const std::string& sep)
{
  smtk::model::StringList separatedPath;
  boost::split(separatedPath, itemPath, boost::is_any_of(sep));

  HintedItem key;
  key.Context = hint->findString("context")->value(0);
  key.SeparatedPath = separatedPath;
  key.Path = itemPath;
  key.Separator = sep;
  smtk::attribute::StringItem::Ptr iname = hint->findString("item name");
  if (iname->value(0).empty())
    {
    iname->setIsEnabled(true);
    if (iname->numberOfValues() < 1)
      iname->setNumberOfValues(1);
    iname->setValue(0, itemPath);
    }
  this->m_hints[key].insert(hint);
}

std::set<smtk::attribute::AttributePtr> OperatorLog::hintsForItem(
  const std::string& context,
  const std::string& itemPath,
  const std::string& sep) const
{
  smtk::model::StringList separatedPath;
  boost::split(separatedPath, itemPath, boost::is_any_of(sep));

  HintedItem key;
  key.Context = context;
  key.Path = itemPath;
  key.Separator = sep;
  key.SeparatedPath = separatedPath;
  HintMap::const_iterator it = this->m_hints.find(key);
  if (it != this->m_hints.end())
    return it->second;
  return std::set<smtk::attribute::AttributePtr>();
}

void OperatorLog::resetHint(const std::string& context, const std::string& itemPath, const std::string& sep)
{
  smtk::model::StringList separatedPath;
  boost::split(separatedPath, itemPath, boost::is_any_of(sep));

  HintedItem key;
  key.Context = context;
  key.Path = itemPath;
  key.Separator = sep;
  key.SeparatedPath = separatedPath;
  this->m_hints.erase(key);
}

void OperatorLog::resetHints(const std::string& context)
{
  HintedItem key;
  key.Context = context;
  HintMap::iterator it;
  while ((it = this->m_hints.lower_bound(key)) != this->m_hints.end())
    this->m_hints.erase(it);
}

void OperatorLog::resetHints()
{
  this->m_hints.clear();
}

/**\brief Return an active log instance.
  *
  * The first OperatorLog instance to be created is considered
  * active. Upon destruction, the active log is a NULL pointer
  * until the next log is constructed. (Note that logs created
  * after the initial log and before the initial log is destroyed
  * are ignored.)
  *
  * This is a convenience for applications that wish to coordinate
  * logging between C++ and Python. This method is Python-wrapped,
  * so an active log can be set and retrieved in both contexts.
  */
OperatorLog* OperatorLog::activeLog()
{
  return ActiveOperatorLog;
}

/**\brief Override the active log.
  *
  * See OperatorLog::activeLog() for more information.
  */
void OperatorLog::setActiveLog(OperatorLog* log)
{
  ActiveOperatorLog = log;
}

/**\brief Add a log processor to the log.
  *
  * The processor's initialize method will be called.
  */
void OperatorLog::addLogProcessor(smtk::shared_ptr<LogProcessor> processor)
{
  if (this->m_processors.insert(processor).second)
    {
    processor->initialize(this);
    }
}

/**\brief Remove a log processor from the log.
  *
  * The processor's finalize method will be called.
  */
void OperatorLog::removeLogProcessor(smtk::shared_ptr<LogProcessor> processor)
{
  if (processor)
    {
    processor->finalize(this);
    this->m_processors.erase(processor);
    }
}

/**\brief Remove all log processors from the log.
  *
  * Each processor's finalize method will be called.
  */
void OperatorLog::resetLogProcessors()
{
  for (ProcessorSet::iterator it = this->m_processors.begin(); it != this->m_processors.end(); ++it)
    {
    (*it)->finalize(this);
    }
  this->m_processors.clear();
}

/**\brief Notify log processors that new log records are available.
  */
void OperatorLog::processRecord(std::size_t finalRecordId)
{
  for (ProcessorSet::iterator it = this->m_processors.begin(); it != this->m_processors.end(); ++it)
    {
    (*it)->addRecords(this, finalRecordId);
    }
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

  std::size_t finalRecordId = self->recordInvocation(event, op);
  self->processRecord(finalRecordId);

  return 0;
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

  int finalRecordId = self->recordResult(event, op, r);
  self->processRecord(finalRecordId);

  return 0; // static_cast<int>(finalRecordId);
}

  } // namespace io
} // namespace smtk
