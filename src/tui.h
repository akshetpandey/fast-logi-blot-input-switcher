//
// Created by akshet on 4/18/23.
//

#pragma once

#include <string>
#include <vector>

int showSelector(const std::string& header, const std::vector<std::string>& options);

std::vector<bool> showMultiSelector(const std::string& header, const std::vector<std::string>& options);

int getIntegerInput(const std::string& header, int min, int max);