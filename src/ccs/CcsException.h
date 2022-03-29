#ifndef CCS_EXCEPTION_H
#define CCS_EXCEPTION_H

#include <exception>
#include <string>

namespace CCS {

  class CcsException : public std::exception {
    std::string m_message;
  public:
    CcsException(std::string message);
    virtual const char *what();
  };
}

#endif
