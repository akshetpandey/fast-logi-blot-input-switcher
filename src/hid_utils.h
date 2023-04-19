//
// Created by akshet on 2/1/23.
//

#pragma once

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "hidapi.h"

#include "logitech_devices.h"

absl::StatusOr<std::vector<const ReceiverInfo>> findAllReceivers();

absl::StatusOr<std::string> findReceiverPath(const ReceiverInfo& receiverInfo);

absl::StatusOr<hid_device*> openReceiverAtPath(const ReceiverInfo& receiverInfo, const std::string& receiverPath);

absl::Status switchChanelForDevice(const DeviceInfo& deviceInfo, hid_device* receiver, uint8_t toChannel);

absl::Status changeChannel(ReceiverType receiverType,
                           const std::string& receiverPathHint,
                           const std::vector<DeviceType>& deviceTypes,
                           std::uint8_t toChannel);