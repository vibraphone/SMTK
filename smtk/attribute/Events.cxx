#include "smtk/attribute/Events.h"

#include "smtk/attribute/System.h"
#include "smtk/attribute/Attribute.h"
#include "smtk/attribute/Definition.h"
#include "smtk/attribute/ItemDefinition.h"
#include "smtk/attribute/ModelEntityItem.h"
#include "smtk/attribute/StringItem.h"
#include "smtk/attribute/DoubleItem.h"
#include "smtk/attribute/IntItem.h"

#include <list>

namespace smtk {
  namespace attribute {

SMTK_EVENT_RESPONDERS_IMPL(SystemCreatedEvent);
SMTK_EVENT_RESPONDERS_IMPL(SystemDestroyedEvent);
SMTK_EVENT_RESPONDERS_IMPL(SystemAddDefinitionEvent);

typedef std::list<EventDataStorage> EventStorageList;
static EventStorageList s_eventData;

// Look through the list of allocated event data objects,
// returning a free entry or allocating a new one. 
static EventDataStorage* findAndMarkStorage()
{
  // TODO: To make this threadsafe, add a mutex and lock it when searching for data.
  EventStorageList::iterator it;
  for (it = s_eventData.begin(); it != s_eventData.end(); ++it)
    {
    if (!it->m_inUse)
      {
      it->m_inUse = 1;
      return &(*it);
      }
    }
  //s_eventData.emplace(s_eventData.end()); // NB: Requires C++11, which we should have but...
  EventDataStorage blank;
  s_eventData.insert(s_eventData.end(), blank);
  EventDataStorage* event = &s_eventData.back();
  event->m_inUse = 1;
  return event;
}

static void unmarkAndFreeStorage(EventDataStorage* storage)
{
  if (storage)
    storage->m_inUse = 0;
}

EventDataStorage::EventDataStorage()
  : m_system(NULL)
{
}

EventDataStorage::EventDataStorage(smtk::attribute::System* sys)
  : m_system(sys)
{
}

EventData::EventData()
  : m_storage(findAndMarkStorage())
{
}

EventData::~EventData()
{
  unmarkAndFreeStorage(this->m_storage);
}

SystemEvent::SystemEvent(EventChangeType etype, System* sys)
{
  this->m_storage->m_type = etype;
  this->m_storage->m_system = sys;
}

SystemCreatedEvent::SystemCreatedEvent(System* sys)
  : SystemEvent(SystemCreatedEvent::type(), sys)
{
}

SystemDestroyedEvent::SystemDestroyedEvent(System* sys)
  : SystemEvent(SystemCreatedEvent::type(), sys)
{
}

SystemAddDefinitionEvent::SystemAddDefinitionEvent(DefinitionPtr def)
  : SystemEvent(SystemAddDefinitionEvent::type(), def->system())
{
  this->m_storage->m_definition = def;
}

  } // namespace attribute
} // namespace smtk
