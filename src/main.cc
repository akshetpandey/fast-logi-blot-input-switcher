#include <fstream>

#if !defined(_WIN32)
#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"
#endif

#include "absl/log/globals.h"
#include "absl/log/initialize.h"
#include "absl/log/log.h"
#include "cxxopts.hpp"
#include "fmt/core.h"
#include "hidapi.h"

#include "config.h"
#include "device_config.h"
#include "hid_utils.h"
#include "utils.h"

int main(int argc, char* argv[]) {
#if !defined(_WIN32)
    absl::InitializeSymbolizer(argv[0]);
    absl::FailureSignalHandlerOptions handlerOptions;
    absl::InstallFailureSignalHandler(handlerOptions);
#endif

    absl::SetMinLogLevel(absl::LogSeverityAtLeast::kInfo);
    absl::SetStderrThreshold(absl::LogSeverityAtLeast::kInfo);
    absl::InitializeLog();

    auto options = cxxopts::Options(PROGRAM, "Easily switch between paired devices on logitech receivers");
    options.add_options()                                                                           //
        ("g, generate-config", "generate a new config file and write to default config directory")  //
        ("h, help", "display this help message")                                                    //
        ("v, version", "display version");                                                          //

    auto args = options.parse(argc, argv);
    if (args.count("help")) {
        fmt::print("{}\n", options.help());
        return 0;
    }
    if (args.count("version")) {
        fmt::print("{}\n", VERSION);
        return 0;
    }

    if (hid_init() < 0) {
        fmt::print("Error on hid_init {}", ws2s(hid_error(nullptr)));
        return -1;
    }

    if (args.count("generate-config") || !configExists()) {
        fmt::print("No config file found. Generating one now\n");
        auto result = generateConfig();
        if (!result.ok()) {
            fmt::print("{}\n", result.ToString());
        }
    } else {
        auto deviceConfig = loadConfig();
        auto status = changeChannel(deviceConfig.receiver, deviceConfig.receiverPathHint, deviceConfig.devices,
                                    deviceConfig.toChannel);
        if (!status.ok()) {
            fmt::print("Error changing channels {}\n", status.ToString());
        }
    }

    hid_exit();
    return 0;
}