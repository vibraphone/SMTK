#include "smtk/attribute/System.h"
#include "smtk/attribute/Definition.h"
#include "smtk/attribute/Events.h"
#include "smtk/attribute/DoubleItem.h"
#include "smtk/attribute/DoubleItemDefinition.h"

#include "smtk/common/testing/cxx/helpers.h"

#include "smtk/Function.h"

#include "smtk/model/testing/cxx/helpers.h"

#include <iostream>
#include <stdlib.h> // for drand48

using namespace smtk::attribute;

int testSystemCreation()
{
  std::cout << "\ntestSystemCreation:\n";
  bool didCallConstructor = false;
  std::size_t cbConIdx =
    SystemCreatedEvent::responses() +=
      [&didCallConstructor](const SystemCreatedEvent* event) {
        didCallConstructor = true;
        std::cout << "  Detected attribute system creation: " << event->system() << "\n";
      };
  test(cbConIdx >= 1 && !SystemCreatedEvent::responses().empty(), "No creation callbacks registered!");

  bool didCallDestructor = false;
  std::size_t cbDesIdx =
    SystemDestroyedEvent::responses() +=
      [&didCallDestructor](const SystemDestroyedEvent* event) {
        didCallDestructor = true;
        std::cout << "  Detected attribute system destruction: " << event->system() << "\n";
      };
  System* dummy = new System;
  test(didCallConstructor, "Did not receive callback for system construction.");

  delete dummy;
  test(didCallDestructor, "Did not receive callback for system destruction.");

  SystemCreatedEvent::responses().reset();
  SystemDestroyedEvent::responses().reset();
  test(SystemCreatedEvent::responses().empty(), "Did not reset callbacks!");
  return 0;
}

int testDefinitionCreation()
{
  std::cout << "\ntestDefinitionCreation:\n";
  bool didCall = false;
  std::size_t cbIdx =
    SystemAddDefinitionEvent::responses() +=
      [&didCall](const SystemAddDefinitionEvent* event) {
        didCall = true;
        std::cout
          << "  Definition \"" << event->definition()->type() << "\""
          << " added to system " << event->system() << "\n";
      };

  System* sys = new System;
  DefinitionPtr def = sys->createDefinition("testDefinition");

  delete sys;

  SystemAddDefinitionEvent::responses() -= cbIdx;
  test(SystemAddDefinitionEvent::responses().empty(), "Definition callback not unregistered!");
  return 0;
}

int testAttributeValueChange()
{
  std::cout << "\ntestAttributeValueChange:\n";
  bool didCall = false;
  std::size_t cbIdx =
    ItemValueChangedEvent<double>::responses() +=
      [&didCall](const ItemValueChangedEvent<double>* event) {
        if (!didCall)
        std::cout
          << "  value of \"" << event->item()->name() << "\" changed from \"" << event->oldValue() << "\""
          << " to \"" << event->newValue() << "\""
          << " (system " << event->system() << ")\n";
        didCall = true;
      };

  System* sys = new System;
  DefinitionPtr def = sys->createDefinition("testDefinition");
  DoubleItemDefinitionPtr dblDef = DoubleItemDefinition::New("floatingPoint");
  def->addItemDefinition(dblDef);
  AttributePtr att = sys->createAttribute("fringle", def);
  DoubleItemPtr itm = att->findDouble("floatingPoint");
  itm->setValue(0, 2.718281828);

  smtk::model::testing::Timer timer;
  timer.mark();
  for (int i = 0; i < 10000000; ++i)
    itm->setValue(0, drand48());
  double deltaT1 = timer.elapsed();
  std::cout << "  Callback with one conditional:   " << deltaT1 << " (" << (deltaT1/1e7) << "/call)\n";

  ItemValueChangedEvent<double>::responses() -= cbIdx;
  test(ItemValueChangedEvent<double>::responses().empty(), "Definition callback not unregistered!");

  timer.mark();
  for (int i = 0; i < 10000000; ++i)
    itm->setValue(0, drand48());
  double deltaT2 = timer.elapsed();
  std::cout << "  No callbacks registered:         " << deltaT2 << " (" << (deltaT2/1e7) << "/call)\n";

  // Now add a bunch of do-nothing callbacks:
  for (int i = 0; i < 1000; ++i)
    {
    std::size_t cbIdx =
      ItemValueChangedEvent<double>::responses() +=
        [](const ItemValueChangedEvent<double>* event) {
          (void)event;
        };
    }
  timer.mark();
  for (int i = 0; i < 1000000; ++i)
    itm->setValue(0, drand48());
  double deltaT3 = timer.elapsed();
  std::cout << "  1000 empty callbacks registered: " << deltaT3 << " (" << (deltaT3/1e6) << "/call)\n";
  ItemValueChangedEvent<double>::responses().reset();

  std::cout
    << "  Per-event overhead: " << (deltaT2/1e7) << "s/event\n"
    << "  Per-CB overhead: " << (deltaT3/1e6 - deltaT2/1e7)/1000. << "s/event/CB\n";

  delete sys;
  return 0;
}

#define NUM_ITEMS 1000
#define NUM_REPS 1000
int testPerInstanceObservers()
{
  std::cout << "\ntestPerInstanceObservers:\n";
  System* sys = new System;
  DefinitionPtr def = sys->createDefinition("testDefinition");

  // Create an attribute definition with 1000 items and, for each,
  // register an observer that only runs when the items match.
  std::string itemNames[NUM_ITEMS];
  for (int i = 0; i < NUM_ITEMS; ++i)
    {
    std::ostringstream os;
    os << "floatingPoint" << i;
    DoubleItemDefinitionPtr dblDef = DoubleItemDefinition::New(os.str().c_str());
    def->addItemDefinition(dblDef);
    }
  AttributePtr att = sys->createAttribute("fringle", def);
  int count = 0;
  DoubleItemPtr allItems[NUM_ITEMS];
  for (int i = 0; i < NUM_ITEMS; ++i)
    {
    std::ostringstream os;
    os << "floatingPoint" << i;
    DoubleItemPtr itm = att->findDouble(os.str().c_str());
    itm->connect<ItemValueChangedEvent<double> >(
      [&count](const ItemValueChangedEvent<double>* event) {
        ++count;
      });
    allItems[i] = itm;
    }

  smtk::model::testing::Timer timer;
  timer.mark();
  for (int i = 0; i < NUM_REPS; ++i)
    for (int j = 0; j < NUM_ITEMS; ++j)
      allItems[j]->setValue(0, drand48());
  double deltaT = timer.elapsed();
  std::cout
    << "  " << NUM_REPS << " counter-increment callbacks registered on " << NUM_ITEMS << " instances: "
    << deltaT << " (" << (deltaT/NUM_REPS/NUM_ITEMS) << "/call)\n";
  ItemValueChangedEvent<double>::responses().reset();
  std::cout << "  count is " << count << ", expected " << (NUM_ITEMS * NUM_REPS) << "\n";
  test(count == NUM_ITEMS * NUM_REPS, "Bad count of item-value changes.");

  delete sys;
  return 0;
}

int main()
{
  testSystemCreation();
  testDefinitionCreation();
  testAttributeValueChange();
  testPerInstanceObservers();
}
