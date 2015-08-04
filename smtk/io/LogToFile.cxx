#include "smtk/io/LogToFile.h"

#include "smtk/io/OperatorLog.h"

#include <fstream>

namespace smtk {
  namespace io {

/**\brief Construct a log processor that logs to a file or other C++ ostream.
  *
  */
LogToFile::LogToFile()
{
  this->m_stream = NULL;
  this->m_deleteStream = false;
  this->m_nextRecord = 0;
}

/**\brief Construct a log processor that logs to a file or other C++ ostream.
  *
  */
LogToFile::~LogToFile()
{
  this->finalize(NULL);
}

/**\brief Set where the log will be written.
  *
  * This should be called before any operations are logged.
  * Don't call both setStream and setFilename(); only the most recent will be used.
  */
void LogToFile::setFilename(const std::string& fname)
{
  if (fname == this->m_filename)
    return;

  if (this->m_stream)
    {
    this->finalize(NULL);

    if (this->m_deleteStream)
      delete this->m_stream;
    this->m_stream = NULL;
    }

  this->m_filename = fname;
  this->m_stream = new std::ofstream(fname);
  this->m_deleteStream = true;
  this->m_nextRecord = 0;
}

/**\brief Set where the log will be written.
  *
  * This should be called before any operations are logged.
  * Don't call both setStream and setFilename(); only the most recent will be used.
  */
void LogToFile::setStream(std::ostream* stream, bool deleteStream)
{
  if (this->m_stream == stream)
    return;

  this->finalize(NULL);
  this->m_filename = std::string();
  this->m_stream = stream;
  this->m_deleteStream = deleteStream;
}

void LogToFile::initialize(OperatorLog* log)
{
  if (log && this->m_stream && this->m_stream->good())
    (*this->m_stream) << log->preamble();
}

void LogToFile::addRecords(OperatorLog* log, std::size_t finalRecordId)
{
  if (log && this->m_stream && this->m_stream->good())
    {
    for (std::size_t i = this->m_nextRecord; i < finalRecordId; ++i)
      (*this->m_stream) << log->textForRecord(i);

    // Always flush stream so that recovery from the log is as thorough as possible.
    this->m_stream->flush();
    }
  this->m_nextRecord = finalRecordId;
}

void LogToFile::finalize(OperatorLog* log)
{
  if (log && this->m_stream && this->m_stream->good())
    (*this->m_stream) << log->postscript();
  std::ofstream* fstream = dynamic_cast<std::ofstream*>(this->m_stream);
  if (fstream)
    fstream->close();
  if (this->m_deleteStream)
    {
    delete this->m_stream;
    this->m_stream = NULL;
    }
}

  } // namespace io
} // namespace smtk
