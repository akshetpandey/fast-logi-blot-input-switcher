//
// Created by akshet on 2/1/23.
//
#include "hid_utils.h"

#include "absl/log/log.h"
#include "fmt/core.h"

#include "utils.h"

absl::StatusOr<std::vector<const ReceiverInfo>> findAllReceivers() {
    std::vector<const ReceiverInfo> results;
    struct hid_device_info *devs, *curDev;
    devs = hid_enumerate(LOGITECH_VENDOR_ID, 0);  // 0,0 = find all devices
    curDev = devs;
    while (curDev) {
        auto receiver = ReceiverInfo::getInfo(static_cast<ReceiverType>(curDev->product_id));
        if (!receiver.ok() || receiver->usagePage != curDev->usage_page || receiver->usage != curDev->usage) {
//            LOG(INFO) << fmt::format("Ignoring device: {} - {}, {:04X}, {:04X}", ws2s(curDev->manufacturer_string),
//                                     ws2s(curDev->product_string), curDev->usage_page, curDev->usage);
            curDev = curDev->next;
            continue;
        }

        results.push_back(*receiver);

        LOG(INFO) << fmt::format("Receiver: {} - {} - {}", ws2s(curDev->manufacturer_string), ws2s(curDev->product_string), curDev->path);
//        LOG(INFO) << fmt::format("  vendorId:      {:04X}", curDev->vendor_id);
//        LOG(INFO) << fmt::format("  productId:     {:04X}", curDev->product_id);
//        LOG(INFO) << fmt::format("  usagePage:     {:04X}", curDev->usage_page);
//        LOG(INFO) << fmt::format("  usage:         {:04X}", curDev->usage);

        curDev = curDev->next;
    }
    hid_free_enumeration(devs);
    if (results.empty()) {
        return absl::NotFoundError("no receivers found");
    }
    return results;
}

absl::StatusOr<std::string> findReceiverPath(const ReceiverInfo& receiverInfo) {
    LOG(INFO) << fmt::format("finding receiver vid:{:04X} pid:{:04X} usagePage:{:04X} usage:{:04X}", receiverInfo.vendorId,
                             receiverInfo.productId, receiverInfo.usagePage, receiverInfo.usage);

    std::string receiverPath;
    auto devs = hid_enumerate(receiverInfo.vendorId, receiverInfo.productId);
    auto cur_dev = devs;
    while (cur_dev) {
        if (cur_dev->usage_page == receiverInfo.usagePage && cur_dev->usage == receiverInfo.usage) {
            receiverPath = cur_dev->path;
            break;
        }
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);

    if (receiverPath.empty()) {
        return absl::NotFoundError(absl::StrCat(absl::Hex(receiverInfo.productId), " receiver not found. hid error ",
                                                ws2s(hid_error(nullptr))));
    }

    return receiverPath;
}

// Make sure to close the returned device with hid_close
absl::StatusOr<hid_device*> openReceiverAtPath(const ReceiverInfo& receiverInfo, const std::string& receiverPath) {
    LOG(INFO) << fmt::format("opening receiver at path: {}", receiverPath);
    auto receiver = hid_open_path(receiverPath.c_str());
    if (receiver == nullptr) {
        return absl::UnavailableError(absl::StrCat("error opening device at path ", absl::Hex(receiverInfo.productId),
                                                   " hid error ", ws2s(hid_error(nullptr))));
    }
    return receiver;
}

absl::Status switchChanelForDevice(const DeviceInfo& deviceInfo, hid_device* receiver, uint8_t toChannel) {
    LOG(INFO) << fmt::format("switching device {}:{} to channel {}", deviceInfo.name, deviceInfo.productId, toChannel);
    auto command = deviceInfo.switchCommands(toChannel);
    auto res = hid_write(receiver, &command[0], std::size(command));
    if (res < 0) {
        return absl::UnavailableError(absl::StrCat("error switching device. hid error ", ws2s(hid_error(receiver))));
    }
    return absl::OkStatus();
}

absl::Status changeChannel(ReceiverType receiverType,
                           const std::string& receiverPathHint,
                           const std::vector<DeviceType>& deviceTypes,
                           std::uint8_t toChannel) {
    auto receiverInfo = ReceiverInfo::getInfo(receiverType);
    if (!receiverInfo.ok()) {
        return receiverInfo.status();
    }

    auto receiver = openReceiverAtPath(*receiverInfo, receiverPathHint);
    if (!receiver.ok()) {
        auto receiverPath = findReceiverPath(*receiverInfo);
        if (!receiverPath.ok()) {
            return receiverPath.status();
        }
        receiver = openReceiverAtPath(*receiverInfo, *receiverPath);
        if (!receiver.ok()) {
            return receiver.status();
        }
    }

    for (auto deviceType : deviceTypes) {
        auto deviceInfo = DeviceInfo::getInfo(deviceType);
        if (!deviceInfo.ok()) {
            return deviceInfo.status();
        }

        auto res = switchChanelForDevice(*deviceInfo, *receiver, toChannel);
        if (!res.ok()) {
            return res;
        }
    }

    hid_close(*receiver);
    return absl::OkStatus();
}
