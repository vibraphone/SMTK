#ifndef __smtk_attribute_Events_txx_
#define __smtk_attribute_Events_txx_

#include "ValueItemTemplate.h"

namespace smtk {
  namespace attribute {

template<typename T>
ItemValueChangedEvent<T>::ItemValueChangedEvent(const Item* itm, int index, T oldValue)
{
  this->m_storage->m_index = index;
  this->m_storage->m_item = itm;
  this->m_storage->setValue(0, oldValue);
}

template<typename T>
T ItemValueChangedEvent<T>::oldValue() const
{
  smtk::shared_ptr<smtk::attribute::ValueItemTemplate<T> > tmp;
  tmp =
    smtk::dynamic_pointer_cast<smtk::attribute::ValueItemTemplate<T> >(
      this->m_storage->m_item->pointer());
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
