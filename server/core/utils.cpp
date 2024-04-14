#include "server/core/include/utils.hpp"

#include <algorithm>
#include <sstream>

std::string str2hex(unsigned long long int value) {
  std::string result;
  while (value != 0) {
    const auto digit = value & 0x0F;
    result.push_back(HEX_DIGITS[digit]);

    value >>= 4U;
  }

  if (result.empty()) {
    result.push_back(HEX_DIGITS[0]);
  }

  std::reverse(result.begin(), result.end());
  return result;
}

std::string quote(const std::string &s) {
  std::stringstream buf;
  buf << '"';

  const size_t n = s.size();
  for (size_t i = 0; i != n; ++i) {
    const unsigned char chr = s[i];

    switch (chr) {
    case '\n':
      buf << "\\n";
      break;

    case '\t':
      buf << "\\t";
      break;

    case '\r':
      buf << "\\r";
      break;

    case '\0':
      buf << "\\0";
      break;

    case '"':
      buf << "\\\"";
      break;

    case '\\':
      buf << "\\\\";
      break;

    default: {
      if (MIN_ASCII_PRINTABLE <= chr && chr <= MAX_ASCII_PRINTABLE) {
        buf << chr;
      } else {
        buf << "\\x" << str2hex(chr);
      }
    }

    break;
    }
  }

  buf << '"';
  return buf.str();
}
