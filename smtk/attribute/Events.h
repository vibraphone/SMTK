//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
#ifndef __smtk_attribute_Events_h
#define __smtk_attribute_Events_h


#include "smtk/PublicPointerDefs.h" // For EntityRef and EntityRefArray
#include "smtk/CoreExports.h" // for SMTKCORE_EXPORT
#include "smtk/Function.h" // for set<smtk::function<EventData> >

#include <string>
#include <vector>
#include <utility> // for std::pair

namespace smtk {
  namespace attribute {

/// The types of events that are reported on attribute systems and attributes:
enum EventChangeType
{
  SYSTEM_CREATED,                   //!< A new attribute system has been constructed.
  SYSTEM_DESTROYED,                 //!< An attribute system is about to be destructed.

  SYSTEM_ADD_DEFINITION,            //!< A new definition is being added to the system.
  SYSTEM_ADD_ATTRIBUTE,             //!< A new attribute is being added to the system.
  SYSTEM_DEL_ATTRIBUTE,             //!< An existing attribute is being removed from the system.
  SYSTEM_RENAME_ATTRIBUTE,          //!< An existing attribute is being renamed.
  SYSTEM_ADD_VIEW,                  //!< A new view is being added to the system.

  DEFINITION_ADD_ITEM_DEFINITION,   //!< An item definition is being added to a definition.
  DEFINITION_SET_LABEL,             //!< A definition's label is changing.
  DEFINITION_SET_VERSION,           //!< An item definition's schema version is being changed.
  DEFINITION_SET_ABSTRACT,          //!< An item definition's abstract- or concrete-ness is being changed.
  DEFINITION_SET_ADVANCE,           //!< An item definition's advance level is being changed.
  DEFINITION_SET_UNIQUE,            //!< An item definition's requirement for uniqueness is being changed.
  DEFINITION_SET_NODAL,             //!< An item definition's applicability to mesh nodes is being changed.
  DEFINITION_SET_NA_COLOR,          //!< An item definition's not-applicable color is being changed.
  DEFINITION_SET_DEFAULT_COLOR,     //!< An item definition's default color is being changed.
  DEFINITION_SET_ASSOCIATION_MASK,  //!< An item definition's model-entity association mask is being changed.
  DEFINITION_SET_BRIEF_DESCRIPTION, //!< An item definition's brief prose description is being changed.
  DEFINITION_SET_DETAIL_DESCRIPTION,//!< An item definition's detailed prose description is being changed.
  DEFINITION_SET_ITEM_OFFSET,       //!< An item definition's offset in an item is being changed.

  ATTRIBUTE_ADD_ITEM,               //!< An item is being added to an attribute as a child.
  ATTRIBUTE_ASSOCIATE_MODEL_ENTITY, //!< A model entity is being associated to an attribute.
  ATTRIBUTE_SET_USER_DATA,          //!< An attribute's user-supplied data is being changed.
  ATTRIBUTE_SET_COLOR,              //!< An attribute's color is being changed.
  ATTRIBUTE_SET_NAME,               //!< An attribute's name is being changed.
  ATTRIBUTE_SET_APPLICABLE_NODES,   //!< An attribute's applicability to mesh nodes is being changed.
  ATTRIBUTE_RESET_ITEMS,            //!< An attribute's child items are being reset.
  ATTRIBUTE_RESET_ASSOCIATIONS,     //!< An attribute's model-entity association are being reset.

  ITEM_DEFINITION_SET_LABEL,              //!< An item definition's label is being changed.
  ITEM_DEFINITION_SET_UNITS,              //!< An item definition's units are being changed.
  ITEM_DEFINITION_SET_VERSION,            //!< An item definition's schema version is being changed.
  ITEM_DEFINITION_SET_OPTIONAL,           //!< An item definition is being marked as optional or mandatory.
  ITEM_DEFINITION_SET_ENABLED,            //!< An item definition is being marked as enabled or disabled.
  ITEM_DEFINITION_SET_ADVANCE_LEVEL,      //!< An item definition's advance level is being changed.
  ITEM_DEFINITION_SET_BRIEF_DESCRIPTION,  //!< An item definition's brief prose description is being changed.
  ITEM_DEFINITION_SET_DETAIL_DESCRIPTION, //!< An item definition's detailed prose description is being changed.
  ITEM_DEFINITION_SET_DEFAULT_INDEX,      //!< A discrete item definition's default discrete index is being changed.
  ITEM_DEFINITION_SET_NUM_REQUIRED_VALUES,//!< The number of values required by an item definition is being changed.
  ITEM_DEFINITION_SET_MAXIMUM_NUM_VALUES, //!< The maximum number of values allowed by an item definition is being changed.
  ITEM_DEFINITION_SET_EXPRESSION_DEFN,    //!< An item definition's expression-definition is being changed.
  ITEM_DEFINITION_SET_EXTENSIBLE,         //!< An item definition is being marked extensible or not.
  ITEM_DEFINITION_SET_VALUE_LABEL,        //!< The label for one value of item definition is being changed.
  ITEM_DEFINITION_SET_DEFAULT_VALUE,      //!< An item definition's default value is being changed.
  ITEM_DEFINITION_SET_MIN_RANGE,          //!< An item definition's minimum value is being changed.
  ITEM_DEFINITION_SET_MAX_RANGE,          //!< An item definition's maximum value is being changed.
  ITEM_DEFINITION_ADD_CONDITIONAL_CHILD,  //!< A conditional child-item-definition is being added to an item definition.
  ITEM_DEFINITION_ADD_CHILD,              //!< A child-item-definition is being added to an item definition.
  ITEM_DEFINITION_ADD_DISCRETE_VALUE,     //!< A discrete enumerant value is being added to an item definition.
  ITEM_DEFINITION_ADD_CATEGORY,           //!< A category is being added to an item definition.
  ITEM_DEFINITION_DEL_CATEGORY,           //!< A category is being removed from an item definition.

  ITEM_SET_LABEL,              //!< An item's label is being changed.
  ITEM_SET_NUM_VALUES,         //!< The number of values stored by an extensible item is being changed.
  ITEM_SET_VALUE,              //!< An item's value is being changed.
  ITEM_SET_ENABLED,            //!< An item is being marked as enabled or disabled.
  ITEM_SET_ADVANCE_LEVEL,      //!< An item's advance-level is being changed.
  ITEM_SET_USER_DATA,          //!< An item's user data is being changed.
  ITEM_SET_DEFINITION,         //!< An item's definition is being changed.
  ITEM_SET_DISCRETE_INDEX,     //!< An item's discrete index is being changed.
  ITEM_UNSET_VALUE,            //!< An item's value is being unset.
  GROUP_SET_NUM_GROUPS,        //!< The number of groups in a group-item is being changed.
  GROUP_SET_GROUP_ITEM,        //!< The membership of one group in a group-item is being changed.

  ANY_EVENT                 //!< All change types (used when calling System::observe).
};

/// A structure to hold information about attribute-system events. Used by EventData.
struct SMTKCORE_EXPORT EventDataStorage
{
  smtk::attribute::EventChangeType m_type;

  smtk::attribute::System* m_system;
  smtk::attribute::AttributePtr m_attribute;
  smtk::attribute::ItemPtr m_item;
  smtk::attribute::ItemDefinitionPtr m_itemDefinition;
  smtk::attribute::DefinitionPtr m_definition;
  smtk::common::ViewPtr m_view;
  std::string m_stringValue[2]; // 2 for old, new
  double m_floatValue[4]; // 4 for RGBA
  int m_intValue[2];
  int m_index;
  int m_inUse;

  EventDataStorage();
  EventDataStorage(smtk::attribute::System* sys);
};

/**\brief A macro used to declare storage for responses to a particular event type.
  *
  * Each subclass of EventData should have this macro in its **public** section.
  * The first argument should be the particular EventChangeType enum corresponding
  * to this class.
  * The second argument of the macro should be the fully-qualified (i.e., including
  * all namespaces) classname of the event class type.
  */
#define SMTK_EVENT_RESPONDERS(EVENTENUM,EVENTCLASS) \
public: \
  /*typedef void (*Response)( const EVENTCLASS & );*/ \
  typedef smtk::function<void(const EVENTCLASS &)> Responder; \
  typedef std::vector<Responder> ResponderArray; \
protected: \
  static ResponderArray s_responses; \
public: \
  static EventChangeType type() \
    { \
    return EVENTENUM; \
    } \
  static const EVENTCLASS ::ResponderArray& responses() \
    { \
    return s_responses; \
    } \
  static std::size_t addResponse(Responder r) \
    { \
    std::size_t idx = s_responses.size(); \
    s_responses.push_back(r); \
    return idx; \
    } \
  static void removeResponse(std::size_t idx) \
    { \
    s_responses.erase(s_responses.begin() + idx); \
    } \
  static void resetResponses() \
    { \
    s_responses.clear(); \
    }

#define SMTK_EVENT_RESPONDERS_IMPL(EVENTCLASS) \
EVENTCLASS ::ResponderArray EVENTCLASS ::s_responses;

/// A base struct for reporting events
class SMTKCORE_EXPORT EventData
{
public:
  EventData(); // find and mark storage.
  ~EventData(); // mark storage as unused

  EventChangeType eventType() const
    {
    return this->m_storage->m_type;
    }
protected:
  EventDataStorage* m_storage;
};

/// An event where an attribute system is available for reference.
class SMTKCORE_EXPORT SystemEvent : public EventData
{
public:
  SystemEvent(EventChangeType etype, System* sys); // SYSTEM_CREATED or SYSTEM_DESTROYED
  System* system() const { return this->m_storage->m_system; }
};

/// The type of event triggered when an attribute system is created.
class SMTKCORE_EXPORT SystemCreatedEvent : public SystemEvent
{
public:
  SystemCreatedEvent(System* sys);
  SMTK_EVENT_RESPONDERS(SYSTEM_CREATED,smtk::attribute::SystemCreatedEvent);
};

/// The type of event triggered when an attribute system is destroyed.
class SMTKCORE_EXPORT SystemDestroyedEvent : public SystemEvent
{
public:
  SystemDestroyedEvent(System* sys);
  SMTK_EVENT_RESPONDERS(SYSTEM_DESTROYED,smtk::attribute::SystemDestroyedEvent);
};

/// An event triggered when adefinition is added to a system.
class SMTKCORE_EXPORT SystemAddDefinitionEvent : public SystemEvent
{
public:
  SystemAddDefinitionEvent(DefinitionPtr def);
  DefinitionPtr definition() const { return this->m_storage->m_definition; }
  SMTK_EVENT_RESPONDERS(SYSTEM_ADD_DEFINITION,smtk::attribute::SystemAddDefinitionEvent);
};

#if 0
/// A templated function
template<typename E, EventChangeType T>
void EventResponse(const E& event, const std::set<smtk::function<void, const E&> >& responses)
{
  typename std::set<smtk::function<void, const E&> >::const_iterator it;
  for (it = responses.begin(); it != responses.end(); ++it)
    (*it)(event);
}

/// An event triggered when a system's attribute membership changes.
struct SMTKCORE_EXPORT SystemMemberAttributeEvent : public SystemEvent
{
  SystemMemberAttributeEvent(EventChangeType etype, AttributePtr att);
  AttributePtr attribute() const { return this->m_storage->m_attribute; }
};

/// An event triggered when a system's view membership changes. (Systems may never lose views, only accrue them.)
struct SMTKCORE_EXPORT SystemMemberViewEvent : public SystemEvent
{
  SystemMemberViewEvent(smtk::common::ViewPtr view);
  smtk::common::ViewPtr view() const { return this->m_storage->m_view; }
};

struct SMTKCORE_EXPORT DefinitionEvent : public SystemEvent
{
  DefinitionEvent(DefinitionPtr def);
  DefinitionPtr definition() const { return this->m_storage->m_definition; }
}; 

struct SMTKCORE_EXPORT DefinitionAddItemDefinitionEvent : public DefinitionEvent
{
  DefinitionAddItemDefinitionEvent(DefinitionPtr def, ItemDefinitionPtr itm);
  ItemDefinitionPtr item() const { return this->m_storage->m_itemDefinition; }
  template<typename T>
  T itemAs() const { return smtk::dynamic_pointer_cast<T>(this->m_storage->m_itemDefinition); }
};

struct SMTKCORE_EXPORT DefinitionSetLabelEvent : public DefinitionEvent
{
  DefinitionSetLabelEvent(DefinitionPtr def, const std::string& newLabel);
  std::string oldLabel() const { return this->definition()->label(); }
  std::string newLabel() const { return this->m_storage->m_stringValue[0]; }
};

struct SMTKCORE_EXPORT DefinitionSetVersionEvent : public DefinitionEvent
{
  DefinitionSetVersionEvent(DefinitionPtr def, int newVersion);
  int oldVersion() const { return this->definition()->version(); }
  int newVersion() const { return this->m_storage->m_intValue[0]; }
};

struct SMTKCORE_EXPORT DefinitionSetBriefDescriptionEvent : public DefinitionEvent
{
  DefinitionSetBriefDescriptionEvent(DefinitionPtr def, const std::string& newBriefDescription);
  std::string oldDescription() const { return this->definition()->briefDescription(); }
  std::string newDescription() const { return this->m_storage->m_stringValue[0]; }
};

struct SMTKCORE_EXPORT DefinitionSetDetailedDescriptionEvent : public DefinitionEvent
{
  DefinitionSetDetailedDescriptionEvent(DefinitionPtr def, const std::string& newDetailedDescription);
  std::string oldDetailedDescription() const { return this->definition()->detailedDescription(); }
  std::string newDetailedDescription() const { return this->m_storage->m_stringValue[0]; }
};

struct SMTKCORE_EXPORT DefinitionSetAbstractEvent : public DefinitionEvent
{
  DefinitionSetAbstractEvent(DefinitionPtr def, bool newAbstract);
  bool oldAbstract() const { return this->definition()->version(); }
  bool newAbstract() const { return this->m_storage->m_intValue[0]; }
};

struct SMTKCORE_EXPORT DefinitionSetAdvanceLevelEvent : public DefinitionEvent
{
  DefinitionSetAdvanceLevelEvent(DefinitionPtr def, int newAdvanceLevel);
  int oldAdvanceLevel() const { return this->definition()->advanceLevel(); }
  int newAdvanceLevel() const { return this->m_storage->m_intValue[0]; }
};

struct SMTKCORE_EXPORT DefinitionSetUniqueEvent : public DefinitionEvent
{
  DefinitionSetUniqueEvent(DefinitionPtr def, bool newUnique);
  bool oldUnique() const { return this->definition()->isUnique(); }
  bool newUnique() const { return this->m_storage->m_intValue[0]; }
};

struct SMTKCORE_EXPORT DefinitionSetNodalEvent : public DefinitionEvent
{
  DefinitionSetNodalEvent(DefinitionPtr def, bool newNodal);
  bool oldNodal() const { return this->definition()->isNodal(); }
  bool newNodal() const { return this->m_storage->m_intValue[0]; }
};

  DEFINITION_SET_NA_COLOR,          //!< An item definition's not-applicable color is being changed.
  DEFINITION_SET_DEFAULT_COLOR,     //!< An item definition's default color is being changed.
  DEFINITION_SET_ASSOCIATION_MASK,  //!< An item definition's model-entity association mask is being changed.
  DEFINITION_SET_ITEM_OFFSET,       //!< An item definition's offset in an item is being changed.

#endif // 0

// -----------------------------------------------------------------------------
// Example usage
#if 0
system->observe(ANY_EVENT, [](const EventData* event) {
  switch (event->type())
    {
  case SYSTEM_ADD_DEFINITION: // ...
  case SYSTEM_ADD_ATTRIBUTE: // ...
    });

system->observe(ATTRIBUTE_SET_NAME, [](AttributeSetNameEvent* event) { ... });

class Attribute {
  template<typename T>
  void observe(EventChangeType etype, smtk::function<void,T> func) {
    AttributePtr att = shared_from_this();
    this->system()->observe(etype, [](const T* edata) {
      if (edata->attribute() == att) func(etype, edata);
    });
  }
};
attribute->observe(ATTRIBUTE_SET_NAME, ...);
#endif // 0

  } // namespace attribute
} // namespace smtk

#endif // __smtk_attribute_Events_h
