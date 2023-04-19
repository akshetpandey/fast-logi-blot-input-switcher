//
// Created by akshet on 4/18/23.
//

#include "tui.h"

#include "absl/log/log.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"

int showSelector(const std::string& header, const std::vector<std::string>& options) {
    auto screen = ftxui::ScreenInteractive::TerminalOutput();
    ftxui::MenuOption option;
    option.on_enter = screen.ExitLoopClosure();

    int selected = 0;
    auto menu = ftxui::Menu(&options, &selected, &option);

    auto renderer = ftxui::Renderer(menu, [&] {
        return ftxui::vbox({
            ftxui::text(header),
            menu->Render(),
        });
    });

    screen.Loop(renderer);
    return selected;
}

std::vector<bool> showMultiSelector(const std::string& header, const std::vector<std::string>& options) {
    std::vector<uint8_t> selected(options.size(), false);

    auto screen = ftxui::ScreenInteractive::TerminalOutput();

    auto inputList = ftxui::Container::Vertical({});
    for (auto i = 0; const auto& option : options) {
        inputList->Add(ftxui::Checkbox(option, reinterpret_cast<bool*>(&selected[i])));
        i++;
    }

    auto radioboxList = std::vector<std::string>{"Done"};
    int selectedRadiobox = 0;
    ftxui::RadioboxOption option;
    option.on_change = screen.ExitLoopClosure();
    auto doneButton = ftxui::Radiobox(&radioboxList, &selectedRadiobox, option);
    inputList->Add(doneButton);

    auto renderer = Renderer(inputList, [&] {
        return ftxui::vbox({
            ftxui::text(header),
            inputList->Render(),
        });
    });

    screen.Loop(renderer);
    return {selected.begin(), selected.end()};
}

int getIntegerInput(const std::string& header, int min, int max) {
    do {
        auto screen = ftxui::ScreenInteractive::TerminalOutput();
        std::string content;
        std::string placeholder = "Enter a number";
        ftxui::InputOption option;
        option.on_enter = screen.ExitLoopClosure();
        auto input = ftxui::Input(&content, &placeholder, option);
        auto renderer = ftxui::Renderer(input, [&] {
            return ftxui::vbox({
                ftxui::text(header),
                input->Render(),
            });
        });

        screen.Loop(renderer);
        try {
            auto res = std::stoi(content);
            if (res >= min && res <= max) {
                return res;
            } else {
                LOG(ERROR) << "Invalid input. Please enter a number between " << min << " and " << max;
            }
        } catch (std::invalid_argument& e) {
            LOG(ERROR) << "Invalid input. Please enter a number";
        }
    } while (true);
}