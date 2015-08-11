#ifndef __smtk_attribute_Events_txx_
#define __smtk_attribute_Events_txx_

#include "ValueItemTemplate.h"

namespace smtk {
  namespace attribute {

template<typename T>
ItemValueChangedEvent<T>::ItemValueChangedEvent(ConstItemPtr itm, int index, T oldValue)
  : SystemEvent(ItemValueChangedEvent<T>::type(), itm->attribute()->system())
{
  this->m_storage->m_index = index;
  this->m_storage->m_item = smtk::const_pointer_cast<Item>(itm);
  this->m_storage->m_attribute = itm->attribute();
  this->m_storage->m_definition = this->attribute()->definition();
  this->m_storage->setValue(0, oldValue);
  //this->m_storage->m_itemDefinition = smtk::const_pointer_cast<Item>(itm)->definition();
}

template<typename T>
T ItemValueChangedEvent<T>::oldValue() const
{
  smtk::shared_ptr<smtk::attribute::ValueItemTemplate<T> > tmp;
  tmp =
    smtk::dynamic_pointer_cast<smtk::attribute::ValueItemTemplate<T> >(
      this->m_storage->m_item);
  return tmp->value(this->m_storage->m_index);
}

template<typename T>
T ItemValueChangedEvent<T>::newValue() const
{
  return this->m_storage->m_floatValue[0];
}

  } // namespace attribute
} // namespace smtk

#endif // __smtk_attribute_Events_txx_
