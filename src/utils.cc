//
// Created by akshet on 2/2/23.
//

#include "utils.h"

#include <codecvt>
#include <locale>

std::string ws2s(const std::wstring& str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.to_bytes(str);
}
