//
// Created by akshet on 1/31/23.
//

#include "logitech_devices.h"

#include <map>

#include "absl/log/check.h"

static std::map<ReceiverType, const ReceiverInfo> receiverTypeToInfo;
static std::map<DeviceType, const DeviceInfo> deviceTypeToInfo;

absl::StatusOr<const ReceiverInfo> ReceiverInfo::getInfo(ReceiverType receiverType) {
    if (receiverTypeToInfo.contains(receiverType)) {
        return receiverTypeToInfo.at(receiverType);
    }
    return absl::InvalidArgumentError(absl::StrCat("unknown receiver ", absl::Hex(receiverType)));
}

absl::StatusOr<const DeviceInfo> DeviceInfo::getInfo(DeviceType deviceType) {
    if (deviceTypeToInfo.contains(deviceType)) {
        return deviceTypeToInfo.at(deviceType);
    }
    return absl::InvalidArgumentError(absl::StrCat("unknown device ", absl::Hex(deviceType)));
}

std::vector<uint8_t> DeviceInfo::switchCommands(std::uint8_t to_channel) const {
    return {0x10, kind, switchCode.first, switchCode.second, to_channel, 0x00, 0x00};
}

std::vector<DeviceInfo> getDevicesForReceiver(const ReceiverInfo& receiverInfo) {
    std::vector<DeviceInfo> devices;
    for (const auto& [_, deviceInfo] : deviceTypeToInfo) {
        if (deviceInfo.receiverId == receiverInfo.productId) {
            devices.push_back(deviceInfo);
        }
    }
    return devices;
}

static void R(const ReceiverInfo& receiverInfo) {
    auto [_, success] = receiverTypeToInfo.insert({receiverInfo.productId, receiverInfo});
    CHECK(success) << "Failed to register receiver: " << receiverInfo.name;
}

static void R(const DeviceInfo& deviceInfo) {
    auto [_, success] = deviceTypeToInfo.insert({deviceInfo.productId, deviceInfo});
    CHECK(success) << "Failed to register device: " << deviceInfo.name;
}

static const auto r = []() {
    // Receivers
    R({UNIFYING_C52B, "Logitech Unifying Receiver"});
    R({UNIFYING_C532, "Logitech Unifying Receiver 2"});
    R({BOLT_C548, "Logitech Bolt Receiver"});

    // Devices
    R({MX_MECHANICAL, BOLT_C548, "Logitech MX Mechanical", DeviceInfo::DeviceKind::KEYBOARD, {0x09, 0x1E}});
    R({MX_MASTER_3S, BOLT_C548, "Logitech MX Master 3S", DeviceInfo::DeviceKind::MOUSE, {0x0A, 0x1B}});

    return 0;
}();
