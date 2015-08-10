#include "smtk/attribute/System.h"
#include "smtk/attribute/Definition.h"
#include "smtk/attribute/Events.h"

#include "smtk/common/testing/cxx/helpers.h"

#include "smtk/Function.h"

#include <iostream>

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

int main()
{
  testSystemCreation();
  testDefinitionCreation();
}
