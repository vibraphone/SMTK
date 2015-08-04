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
#include "smtk/SharedFromThis.h"
#include "smtk/PublicPointerDefs.h"
#include "smtk/model/Events.h"
#include "smtk/model/StringData.h"

#include <cstddef> // for std::size_t

namespace smtk {
  namespace io {

class LogProcessor;

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
  * <h2>What are hints?</h2>
  *
  * Hints are modeled as SMTK attributes derived from the "log hint"
  * definition, and provide two types of information:
  * information on how to mark up the value of the attribute being logged
  * and
  * information on which attributes the hint may be applied to.
  *
  * Consider this example:
  * a user picks an item in a 3-D render view to serve as
  * an operator's tool body; the hint associated with "tool" might be
  * a "pick-along-line" hint with a base point and direction.
  * The PythonOperatorLog might normally output <pre>
  *
  *   item0 = smtk.attribute.to_concrete(op1.specification().find('tool'))
  *   SetVectorValue(item0, [UUID('xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx'),])
  *
  * </pre>but could instead write this when a hint was present: <pre>
  *
  *   picked = view.pickAlongLine(basePt=[0,0,0], dir=[0,0,1])
  *   item0 = smtk.attribute.to_concrete(op1.specification().find('tool'))
  *   SetVectorValue(item0, picked)
  *
  * </pre>. In order to know that the pick-along-line hint should
  * be applied to the 'tool' item of the op1 operator, the application
  * can also store information with the hint when the pick occurs.
  *
  * Hints can make customization for macros much simpler since the
  * macro can be a script that finds applicable hints and chooses
  * among them to enhance its behavior.
  *
  * There is no requirement on the type of item that can be hinted;
  * any item (whether it is an IntItem or ModelEntityItem) can be
  * hinted. It is then up to the subclasses of OperatorLog to use
  * these hints or not.
  *
  * <h2>Using hints</h2>
  *
  * The log provides a method to create a particular type of hint.
  * Once created, you should populate the hint's fields and then
  * call a method on the log to register the hint with the item(s)
  * it applies to.<pre>
  *
  *     using namespace smtk::attribute;
  *
  *     // Create pick-along-line hint to be applied to an
  *     // operator the application has named "op1":
  *     Attribute::Ptr hint =
  *       log->createHint("pick-along-line", "op1");
  *
  *     // Populate hint fields:
  *     DoubleItem::Ptr base = hint->findDouble("basePt");
  *     DoubleItem::Ptr dirn = hint->findDouble("direction");
  *     base->setNumberOfValues(3);
  *     dirn->setNumberOfValues(3);
  *     // ...
  *
  *     // Register hint with log:
  *     log->addHintForItem("tool", hint);
  * </pre>
  *
  * Then, when an OperatorLog subclass's recordInvocation() method
  * is called so it can record the "op1" operator, it can<pre>
  *
  *     hintSet = log->hintsForItem("op1", "tool");
  *     // Iterate over hints in hintSet (there may be
  *     // more than one, especially if "tool" stores
  *     // multiple values) and record statements to
  *     // the log for each item...
  *     log->resetHints("op1"); // indicate we are done with "op1"
  * </pre>
  *
  * In this way, application-specific hints can encode user
  * intent in a flexible manner as long as the user interface
  * provides enough state to infer both pieces of information
  * required for hinting (the item being hinted and the specification
  * of the hint's derived fields).
  *
  * Applications can do this by making user interactions contextual
  * (i.e., having the user indicate which field of an operator a
  * selection should be applied to rather than specifying the field
  * after the selection has already taken place) or by storing
  * markup on model entities as actions such as selections take place
  * and then looking them up when marked-up items are used by an
  * operation.
  */
class SMTKCORE_EXPORT OperatorLog
{
public:
  OperatorLog(smtk::model::ManagerPtr mgr);
  virtual ~OperatorLog();
  smtkTypeMacro(OperatorLog);

  bool hasFailures() const;
  void resetFailures();

  smtk::attribute::SystemPtr hintSystem();
  smtk::attribute::AttributePtr createHint(const std::string& hintDefName, const std::string& context = "");
  void addHintForItem(const std::string& itemPath, smtk::attribute::AttributePtr hint, const std::string& sep = "/");
  std::set<smtk::attribute::AttributePtr> hintsForItem(const std::string& context, const std::string& itemPath, const std::string& sep = "/") const;
  void resetHint(const std::string& context, const std::string& itemPath, const std::string& sep = "/");
  void resetHints(const std::string& context);
  void resetHints();

  static OperatorLog* activeLog();
  static void setActiveLog(OperatorLog* log);

  virtual std::string preamble() const { return ""; }
  virtual std::string textForRecord(int i) const = 0;
  virtual std::string postscript() const { return ""; }

  void addLogProcessor(LogProcessorPtr processor);
  void removeLogProcessor(LogProcessorPtr processor);
  void resetLogProcessors();

protected:
  void processRecord(std::size_t recordId);

  /**\brief Log the invocation of an operator.
    *
    * The return value should be either -1 (failure) or the
    * number of the first record inserted to the log that
    * represents part of the given operation.
    *
    * Subclasses must implement this method.
    * Be aware that this method may not be called for
    * all operators if a filter is in place.
    */
  virtual std::size_t recordInvocation(
    smtk::model::OperatorEventType event,
    const smtk::model::Operator& op) = 0;

  /**\brief Log the result of an operator.
    *
    * The return value should be either -1 (failure) or the
    * number of the first record inserted to the log that
    * represents part of the given result.
    *
    * Subclasses must implement this method.
    * Be aware that this method may not be called for
    * all operators if a filter is in place.
    */
  virtual std::size_t recordResult(
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

  struct HintedItem
    {
    std::string Context;
    smtk::model::StringList SeparatedPath;
    smtk::model::String Path;
    smtk::model::String Separator;

    bool operator < (const HintedItem& other) const
      {
      return (
        (this->Context < other.Context) ||
        (this->Context == other.Context && this->SeparatedPath < other.SeparatedPath)) ?
        true : false;
      }
    };
  typedef std::vector<smtk::model::WeakOperatorPtr> WeakOpArray;
  typedef std::map<HintedItem, std::set<smtk::attribute::AttributePtr> > HintMap;
  typedef std::set<smtk::shared_ptr<LogProcessor> > ProcessorSet;

  bool m_hasFailures;
  smtk::model::WeakManagerPtr m_manager;
  smtk::attribute::SystemPtr m_hintSys;
  HintMap m_hints;
  WeakOpArray m_watching;
  ProcessorSet m_processors;
};

  } // namespace io
} // namespace smtk

#endif /* __smtk_io_OperatorLog_h */
