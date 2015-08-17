//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================


#include "smtk/attribute/StringItem.h"
#include "smtk/attribute/StringItemDefinition.h"

using namespace smtk::attribute;

//----------------------------------------------------------------------------
StringItem::StringItem(Attribute *owningAttribute,
                       int itemPosition):
  ValueItemTemplate<std::string>(owningAttribute, itemPosition)
{
}

//----------------------------------------------------------------------------
StringItem::StringItem(Item *inOwningItem,
                       int itemPosition,
                       int mySubGroupPosition):
  ValueItemTemplate<std::string>(inOwningItem, itemPosition, mySubGroupPosition)
{
}
//----------------------------------------------------------------------------
StringItem::~StringItem()
{
}
//----------------------------------------------------------------------------
Item::Type StringItem::type() const
{
  return STRING;
}
//----------------------------------------------------------------------------
void StringItem::copyFrom(ItemPtr sourceItem, CopyInfo& info)
{
  // Assigns my contents to be same as sourceItem
  ValueItemTemplate<std::string>::copyFrom(sourceItem, info);
}
//----------------------------------------------------------------------------
Simple::Signal<void(Item*, int, const std::string&)> StringItem::s_valueChangeObserver;

template<>
Simple::Signal<void(Item*, int, const std::string&)>& ValueItemTemplate<std::string>::valueChangeObserver()
{
  return StringItem::s_valueChangeObserver;
}
