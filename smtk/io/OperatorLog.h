//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
#ifndef __smtk_io_OperatorLog_h
#define __smtk_io_OperatorLog_h
/*! \file */

#include "smtk/CoreExports.h"
#include "smtk/SystemConfig.h"
#include "smtk/PublicPointerDefs.h"
#include "smtk/model/Events.h"

namespace smtk {
  namespace io {

/**\brief Log operations run in a session for later adaptation or replay.
  *
  * This class captures all of the operator creation events
  * for a given model manager and invokes pure virtual methods
  * to log the events if they are deemed acceptable by a filter.
  *
  * It also provides a way for applications to associate hints
  * with specific parameters of an operator and/or result so that
  * subclasses can write more intelligent logs. For example, a
  * log file normally stores model entity associations for the
  * tool and workpiece by their UUIDs. But this information is
  * not useful in many contexts because UUIDs can be different
  * between invocations. Instead, it is desirable to store information
  * about how the user selected the tool or workpiece; that is not
  * information available in SMTK because it is application-dependent.
  * So, the OperatorLog class keeps an attribute system and maps the
  * names of items in an operator's specification to attributes that
  * describe the user's or application's selection logic. 
  * Hints can be fetched by subclasses and used to customize the log.
  *
  * If the user picked an item in a 3-D render view to serve as
  * an operator's tool, then the hint associated with "tool" might be
  * a "pick-line" hint with a base point and direction. For example,
  * the PythonOperatorLog might normally output <pre>
  *
  *   item0 = smtk.attribute.to_concrete(op1.specification().find('tool'))
  *   SetVectorValue(item0, [UUID('xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx'),])
  *
  * </pre> but given a hint could instead write <pre>
  *
  *   picked = view.pickAlongLine(basePt=[0,0,0], dir=[0,0,1])
  *   item0 = smtk.attribute.to_concrete(op1.specification().find('tool'))
  *   SetVectorValue(item0, picked)
  *
  * </pre>. This makes customization for macros much simpler.
  *
  * There is no requirement on the type of item that can be hinted;
  * any item (whether it is an IntItem or ModelEntityItem) can be
  * hinted. It is then up to the subclasses of OperatorLog to use
  * these hints or not.
  */
class SMTKCORE_EXPORT OperatorLog
{
public:
  OperatorLog(smtk::model::ManagerPtr mgr);
  virtual ~OperatorLog();

  bool hasFailures() const;
  void resetFailures();

  smtk::attribute::SystemPtr hintSystem();
  smtk::attribute::AttributePtr createHint(const std::string& hintDef);
  void setHintForItem(const std::string& itemPath, smtk::attribute::AttributePtr);
  smtk::attribute::AttributePtr hintForItem(const std::string& itemPath) const;
  void resetHint(const std::string& itemPath);
  void resetHints();

protected:
  /**\brief Log the invocation of an operator.
    *
    * Subclasses must implement this method.
    * Be aware that this method may not be called for
    * all operators if a filter is in place.
    */
  virtual int recordInvocation(
    smtk::model::OperatorEventType event,
    const smtk::model::Operator& op) = 0;

  /**\brief Log the result of an operator.
    *
    * Subclasses must implement this method.
    * Be aware that this method may not be called for
    * all operators if a filter is in place.
    */
  virtual int recordResult(
    smtk::model::OperatorEventType event,
    const smtk::model::Operator& op,
    smtk::model::OperatorResult r) = 0;

  virtual void prepareHintSystem();

  static int operatorCreated(
    smtk::model::OperatorEventType event,
    const smtk::model::Operator& op,
    void* user);
  static int operatorInvoked(
    smtk::model::OperatorEventType event,
    const smtk::model::Operator& op,
    void* user);
  static int operatorReturned(
    smtk::model::OperatorEventType event,
    const smtk::model::Operator& op,
    smtk::model::OperatorResult r,
    void* user);

  typedef std::vector<smtk::model::WeakOperatorPtr> WeakOpArray;
  typedef std::map<std::string, smtk::attribute::AttributePtr> HintMap;

  bool m_hasFailures;
  smtk::model::WeakManagerPtr m_manager;
  smtk::attribute::SystemPtr m_hintSys;
  HintMap m_hints;
  WeakOpArray m_watching;
};

  } // namespace io
} // namespace smtk

#endif /* __smtk_io_OperatorLog_h */
