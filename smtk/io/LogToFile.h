#ifndef __smtk_io_LogToFile_h
#define __smtk_io_LogToFile_h

#include "smtk/io/LogProcessor.h"

#include <iostream>

namespace smtk {
  namespace io {

/**\brief A log processor that flushes records to a file as they are generated.
  *
  */
class LogToFile : public LogProcessor
{
public:
  smtkTypeMacro(LogToFile);
  smtkSuperclassMacro(LogProcessor);
  smtkSharedPtrCreateMacro(LogProcessor);

  LogToFile();
  virtual ~LogToFile();

  void setFilename(const std::string& fname);
  void setStream(std::ostream* stream, bool deleteStream);

  virtual void initialize(OperatorLog* log);
  virtual void addRecords(OperatorLog* log, std::size_t finalRecordId);
  virtual void finalize(OperatorLog* log);

protected:
  std::string m_filename;
  std::ostream* m_stream;
  bool m_deleteStream;
  std::size_t m_nextRecord;
};

  } // namespace io
} // namespace smtk

#endif // __smtk_io_LogToFile_h
