#include "CcsException.h"
namespace CCS {

  CcsException::CcsException(std::string message) {
    m_message = message;
  }

  const char* CcsException::what() {
    return m_message.c_str();
  }
}