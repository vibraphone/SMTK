//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================


#include "smtk/attribute/DoubleItem.h"
#include "smtk/attribute/DoubleItemDefinition.h"

using namespace smtk::attribute;

//----------------------------------------------------------------------------
DoubleItem::DoubleItem(Attribute *owningAttribute,
                       int itemPosition):
  ValueItemTemplate<double>(owningAttribute, itemPosition)
{
}

//----------------------------------------------------------------------------
DoubleItem::DoubleItem(Item *inOwningItem,
                       int itemPosition,
                       int mySubGroupPosition):
  ValueItemTemplate<double>(inOwningItem, itemPosition, mySubGroupPosition)
{
}

//----------------------------------------------------------------------------
DoubleItem::~DoubleItem()
{
}
//----------------------------------------------------------------------------
Item::Type DoubleItem::type() const
{
  return DOUBLE;
}
//----------------------------------------------------------------------------
void DoubleItem::copyFrom(ItemPtr sourceItem, CopyInfo& info)
{
  // Assigns my contents to be same as sourceItem
  ValueItemTemplate<double>::copyFrom(sourceItem, info);
}
//----------------------------------------------------------------------------
Simple::Signal<void(Item*, int, const double&)> DoubleItem::s_valueChangeObserver;

template<>
Simple::Signal<void(Item*, int, const double&)>& ValueItemTemplate<double>::valueChangeObserver()
{
  return DoubleItem::s_valueChangeObserver;
}
