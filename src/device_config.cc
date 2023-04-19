//
// Created by akshet on 4/18/23.
//
#include "device_config.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "absl/log/log.h"
#include "fmt/core.h"
#include "fmt/ranges.h"
#include "nlohmann/json.hpp"

#include "hid_utils.h"
#include "logitech_devices.h"
#include "tui.h"

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SavedSwitchConfig, receiver, receiverPathHint, devices, toChannel)

static const std::string configFileName = "device_config.json";

std::filesystem::path getPlatformSpecificConfigPath() {
#if TARGET_OS_LINUX
    auto home = std::getenv("HOME");
    if (home == nullptr) {
        return configFileName;
    }
    return std::filesystem::path(home) / ".config" / "logitech-switcher" / configFileName;
#elif TARGET_OS_MAC
    auto home = std::getenv("HOME");
    if (home == nullptr) {
        return configFileName;
    }
    return std::filesystem::path(home) / "Library" / "Preferences" / "logitech-switcher" / configFileName;
#elif TARGET_OS_WINDOWS
    auto home = std::getenv("APPDATA");
    if (home == nullptr) {
        return configFileName;
    }
    return std::filesystem::path(home) / "logitech-switcher" / configFileName;
#endif
}

bool configExists() {
    auto configPath = getPlatformSpecificConfigPath();
    return std::filesystem::exists(configPath) && !std::filesystem::is_empty(configPath);
}

SavedSwitchConfig loadConfig() {
    auto configPath = getPlatformSpecificConfigPath();
    LOG(INFO) << fmt::format("loading config from: {}", configPath.string());
    auto configFile = std::ifstream(configPath);
    auto configJson = nlohmann::json::parse(configFile);
    LOG(INFO) << fmt::format("json file content: {}", to_string(configJson));
    return configJson.get<SavedSwitchConfig>();
}

void saveConfig(const SavedSwitchConfig& config) {
    auto configJson = nlohmann::json(config);
    LOG(INFO) << fmt::format("json file content: {}", to_string(configJson));
    auto configPath = getPlatformSpecificConfigPath();
    LOG(INFO) << fmt::format("saving config to: {}", configPath.string());
    std::filesystem::create_directories(configPath.parent_path());
    auto configFile = std::ofstream(configPath);
    configFile << configJson;
}

absl::Status generateConfig() {
    auto results = findAllReceivers();
    if (!results.ok()) {
        return results.status();
    } else {
        for (const auto& result : *results) {
            LOG(INFO) << fmt::format("found: {}", result.name);
        }
    }
    auto receiver = results->at(0);
    if (results->size() > 1) {
        auto receiverNames = std::vector<std::string>();
        for (const auto& result : *results) {
            receiverNames.push_back(result.name);
        }
        auto selected = showSelector("Found multiple receivers. Please select which one to use", receiverNames);
        receiver = results->at(selected);
    }

    // TODO: query receiver to find paired devices instead of using our known list
    auto devices = getDevicesForReceiver(receiver);
    auto deviceNames = std::vector<std::string>();
    for (const auto& device : devices) {
        deviceNames.push_back(device.name);
    }
    auto select = showMultiSelector("Select devices to switch", deviceNames);
    LOG(INFO) << fmt::format("selected: {}", fmt::join(select, ", "));
    auto selectedDevices = std::vector<DeviceType>();
    for (std::size_t index = 0; index < select.size(); index++) {
        if (select[index]) {
            selectedDevices.push_back(devices.at(index).productId);
        }
    }
    auto toChannel = getIntegerInput("Enter channel to switch to between [0, 2]: ", 0, 2);
    auto receiverPath = findReceiverPath(receiver);
    if (!receiverPath.ok()) {
        return receiverPath.status();
    }
    auto deviceConfig =
        SavedSwitchConfig{receiver.productId, *receiverPath, selectedDevices, static_cast<uint8_t>(toChannel)};
    saveConfig(deviceConfig);
    return absl::OkStatus();
}