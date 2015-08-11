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
    SystemCreatedEvent::addResponse(
      [&didCallConstructor](const SystemCreatedEvent& event) {
        didCallConstructor = true;
        std::cout << "  Detected attribute system creation: " << event.system() << "\n";
      });
  test(SystemCreatedEvent::responses().size() == 1, "No creation callbacks registered!");

  bool didCallDestructor = false;
  std::size_t cbDesIdx =
    SystemDestroyedEvent::addResponse(
      [&didCallDestructor](const SystemDestroyedEvent& event) {
        didCallDestructor = true;
        std::cout << "  Detected attribute system destruction: " << event.system() << "\n";
      });
  System* dummy = new System;
  test(didCallConstructor, "Did not receive callback for system construction.");

  delete dummy;
  test(didCallDestructor, "Did not receive callback for system destruction.");

  SystemCreatedEvent::resetResponses();
  SystemDestroyedEvent::resetResponses();
  test(SystemCreatedEvent::responses().empty(), "Did not reset callbacks!");
  return 0;
}

int testDefinitionCreation()
{
  std::cout << "\ntestDefinitionCreation:\n";
  bool didCall = false;
  std::size_t cbIdx =
    SystemAddDefinitionEvent::addResponse(
      [&didCall](const SystemAddDefinitionEvent& event) {
        didCall = true;
        std::cout
          << "  Definition \"" << event.definition()->type() << "\""
          << " added to system " << event.system() << "\n";
      });

  System* sys = new System;
  DefinitionPtr def = sys->createDefinition("testDefinition");

  delete sys;

  SystemAddDefinitionEvent::removeResponse(cbIdx);
  test(SystemAddDefinitionEvent::responses().empty(), "Definition callback not unregistered!");
  return 0;
}

int testAttributeValueChange()
{
  std::cout << "\ntestAttributeValueChange:\n";
  bool didCall = false;
  std::size_t cbIdx =
    ItemValueChangedEvent<double>::addResponse(
      [&didCall](const ItemValueChangedEvent<double>& event) {
        if (!didCall)
        std::cout
          << "  value of \"" << event.item()->name() << "\" changed from \"" << event.oldValue() << "\""
          << " to \"" << event.newValue() << "\""
          << " (system " << event.system() << ")\n";
        didCall = true;
      });

  System* sys = new System;
  DefinitionPtr def = sys->createDefinition("testDefinition");
  DoubleItemDefinitionPtr dblDef = DoubleItemDefinition::New("floatingPoint");
  def->addItemDefinition(dblDef);
  AttributePtr att = sys->createAttribute("fringle", def);
  att->findDouble("floatingPoint")->setValue(0, 2.718281828);

  smtk::model::testing::Timer timer;
  timer.mark();
  for (int i = 0; i < 10000000; ++i)
    att->findDouble("floatingPoint")->setValue(0, drand48());
  double deltaT1 = timer.elapsed();
  std::cout << "Callback with one conditional:   " << deltaT1 << " (" << (deltaT1/1e7) << "/call)\n";

  ItemValueChangedEvent<double>::removeResponse(cbIdx);
  test(ItemValueChangedEvent<double>::responses().empty(), "Definition callback not unregistered!");

  timer.mark();
  for (int i = 0; i < 10000000; ++i)
    att->findDouble("floatingPoint")->setValue(0, drand48());
  double deltaT2 = timer.elapsed();
  std::cout << "No callbacks registered:         " << deltaT2 << " (" << (deltaT2/1e7) << "/call)\n";

  // Now add a bunch of do-nothing callbacks:
  for (int i = 0; i < 1000; ++i)
    {
    std::size_t cbIdx =
      ItemValueChangedEvent<double>::addResponse(
        [](const ItemValueChangedEvent<double>& event) {
          (void)event;
        });
    }
  timer.mark();
  for (int i = 0; i < 1000000; ++i)
    att->findDouble("floatingPoint")->setValue(0, drand48());
  double deltaT3 = timer.elapsed();
  std::cout << "1000 empty callbacks registered: " << deltaT3 << " (" << (deltaT3/1e6) << "/call)\n";

  std::cout
    << "Per-event overhead: " << (deltaT2/1e7) << "s/event\n"
    << "Per-CB overhead: " << (deltaT3/1e6 - deltaT2/1e7)/1000. << "s/event/CB\n";

  delete sys;
  return 0;
}

int main()
{
  testSystemCreation();
  testDefinitionCreation();
  testAttributeValueChange();
}
