#include "smtk/model/StringData.h"

#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string/split.hpp"

#include <sstream>

namespace smtk {
  namespace model {

// Internal method to escape characters in a string.
static String escape(const String& text, const String& charactersToEscape)
{
  String result;
  return result;
}

/// Split a \a text string into a list of strings, treating \a separator as characters marking breaks.
StringList split(const String& text, const String& separator)
{
  StringList result;
  String::size_type i;
  String::size_type pos;
  for (i = 0; i != String::npos; i = (pos == String::npos ? pos : pos + 1))
    {
    String::size_type pos = text.find_first_of(separator, i);
    if (i < pos)
      result.push_back(text.substr(i, pos - i));
    }
  return result;
}
/// Join an array of strings (\a textList) together with \a separator between each entry.
String join(const StringList& textList, const String& separator)
{
  std::ostringstream os;
  String result;
  StringList::const_iterator it = textList.begin();
  if (it != textList.end())
    {
    os << escape(*it, separator);
    for (++it; it != textList.end(); ++it)
      os << separator << escape(*it, separator);
    }
  return result;
}

  } // namespace model
} // namespace smtk
