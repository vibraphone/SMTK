#include "smtk/attribute/System.h"
#include "smtk/attribute/Attribute.h"
#include "smtk/attribute/Definition.h"
#include "smtk/attribute/DoubleItem.h"
#include "smtk/attribute/DoubleItemDefinition.h"

#include "smtk/common/testing/cxx/helpers.h"

#include "smtk/Function.h"

#include "smtk/model/testing/cxx/helpers.h"

#include <iostream>
#include <stdlib.h> // for drand48

using namespace smtk::attribute;

int testAttributeValueChange()
{
  std::cout << "\ntestAttributeValueChange:\n";
  bool didCall = false;
  size_t cb1 =
    ValueItemTemplate<double>::valueChangeObserver() +=
      [&didCall](Item* itm, int idx, const double& oldVal) {
        DoubleItem* ditm = dynamic_cast<DoubleItem*>(itm);
        if (!didCall)
          std::cout
            << "  value of \"" << itm->name() << "\" changed from \"" << oldVal << "\""
            << " to \"" << ditm->value(idx) << "\""
            << " (system " << itm->attribute()->system() << ")\n";
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
  std::cout << "Callback with one conditional:   " << deltaT1 << " (" << (deltaT1/1e7) << "/call)\n";

  bool success = (itm->valueChangeObserver() -= cb1);
  test(success, "Definition callback not unregistered!");

  timer.mark();
  for (int i = 0; i < 10000000; ++i)
    itm->setValue(0, drand48());
  double deltaT2 = timer.elapsed();
  std::cout << "No callbacks registered:         " << deltaT2 << " (" << (deltaT2/1e7) << "/call)\n";

  // Now add a bunch of do-nothing callbacks:
  for (int i = 0; i < 1000; ++i)
    {
    std::size_t cbIdx =
      itm->valueChangeObserver() +=
        [](Item*, int, const double& val) {
          (void)val;
        };
    }
  timer.mark();
  for (int i = 0; i < 1000000; ++i)
    itm->setValue(0, drand48());
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
  testAttributeValueChange();
}
