//
// Created by akshet on 2/1/23.
//

#pragma once

#include "absl/status/status.h"

#include "logitech_devices.h"

struct SavedSwitchConfig {
    ReceiverType receiver;
    // Receiver path when this config was written. The path may change if the receiver is plugged into a different port.
    std::string receiverPathHint;
    std::vector<DeviceType> devices;
    std::uint8_t toChannel;
};

bool configExists();

SavedSwitchConfig loadConfig();

void saveConfig(const SavedSwitchConfig& config);

absl::Status generateConfig();