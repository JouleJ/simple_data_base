#pragma once

#include <string>

const unsigned long long int MIN_ASCII_PRINTABLE = 32;
const unsigned long long int MAX_ASCII_PRINTABLE = 127;

const std::string HEX_DIGITS = "0123456789ABCEDF";

std::string str2hex(unsigned long long int value);
std::string quote(const std::string &s);
