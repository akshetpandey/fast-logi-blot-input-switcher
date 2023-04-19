//
// Created by akshet on 1/31/23.
//

#pragma once

#include "absl/status/status.h"
#include "absl/status/statusor.h"

static const std::uint16_t LOGITECH_VENDOR_ID = 0x046D;

enum ReceiverType : std::uint16_t {
    UNIFYING_C52B = 0xC52B,
    UNIFYING_C532 = 0xC532,
    BOLT_C548 = 0xC548,
};

enum DeviceType : std::uint16_t {
    MX_MECHANICAL = 0xB366,
    MX_MASTER_3S = 0xB034,
};

struct ReceiverInfo {
    ReceiverType productId;
    std::string name;

    std::uint16_t vendorId = LOGITECH_VENDOR_ID;
    std::uint16_t usagePage = 0xFF00;
    std::uint16_t usage = 0x0001;

    static absl::StatusOr<const ReceiverInfo> getInfo(ReceiverType receiverType);
};

struct DeviceInfo {
    enum DeviceKind : std::uint8_t {
        KEYBOARD = 0x01,
        MOUSE = 0x02,
    };

    DeviceType productId;
    ReceiverType receiverId;
    std::string name;
    DeviceKind kind;
    std::pair<std::uint8_t, std::uint8_t> switchCode;

    [[nodiscard]] std::vector<std::uint8_t> switchCommands(std::uint8_t to_channel) const;

    static absl::StatusOr<const DeviceInfo> getInfo(DeviceType deviceType);
};

std::vector<DeviceInfo> getDevicesForReceiver(const ReceiverInfo& receiverInfo);