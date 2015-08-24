#include "smtk/attribute/System.h"
#include "smtk/attribute/Definition.h"
#include "smtk/attribute/Attribute.h"
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
  std::cout << "  No callbacks:   " << deltaT1 << " (" << (deltaT1/1e7) << "/call)\n";

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
  DoubleItemPtr allItems[NUM_ITEMS];
  for (int i = 0; i < NUM_ITEMS; ++i)
    {
    std::ostringstream os;
    os << "floatingPoint" << i;
    DoubleItemPtr itm = att->findDouble(os.str().c_str());
    allItems[i] = itm;
    }

  smtk::model::testing::Timer timer;
  timer.mark();
  for (int i = 0; i < NUM_REPS; ++i)
    for (int j = 0; j < NUM_ITEMS; ++j)
      allItems[j]->setValue(0, drand48());
  double deltaT = timer.elapsed();
  std::cout
    << "  " << NUM_REPS << " calls made on " << NUM_ITEMS << " instances: "
    << deltaT << " (" << (deltaT/NUM_REPS/NUM_ITEMS) << "/call)\n";

  delete sys;
  return 0;
}

int main()
{
  testAttributeValueChange();
  testPerInstanceObservers();
}
