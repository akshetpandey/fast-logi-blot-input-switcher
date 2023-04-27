project(
    'fast-logi-channel-switcher',
    'cpp',
    default_options: ['warning_level=3', 'cpp_std=c++20', 'default_library=static'],
    license : 'MIT',
    meson_version: '>=1.0.0',
    version: '0.0.1',
)

conf_data = configuration_data({
    'PROGRAM': '"' + meson.project_name() + '"',
    'VERSION': '"' + meson.project_version() + '"',
})

configure_file(
  output : 'config.h',
  configuration : conf_data,
)
configuration_inc = include_directories('.')

hidapi = subproject('hidapi', default_options: ['cpp_std=c++20','default_library=static'])
absl = subproject('abseil-cpp', default_options: ['cpp_std=c++20', 'default_library=static'])
fmt = subproject('fmt', default_options: ['cpp_std=c++20', 'default_library=static'])
cxxopts = subproject('cxxopts', default_options: ['cpp_std=c++20', 'default_library=static'])
nlohmann_json = subproject('nlohmann_json', default_options: ['cpp_std=c++20', 'default_library=static'])
ftxui = subproject('ftxui', default_options: ['cpp_std=c++20', 'default_library=static'])

dependencies = [
    hidapi.get_variable('hidapi_dep'),
    fmt.get_variable('fmt_dep'),
    absl.get_variable('absl_base_dep'),
    absl.get_variable('absl_crc_dep'),
    absl.get_variable('absl_strings_dep'),
    absl.get_variable('absl_status_dep'),
    absl.get_variable('absl_log_dep'),
    cxxopts.get_variable('cxxopts_dep'),
    nlohmann_json.get_variable('nlohmann_json_dep'),
    ftxui.get_variable('component_dep'),
]

if host_machine.system() != 'windows'
    dependencies += [absl.get_variable('absl_debugging_dep')]
endif

sources = [
    'src/device_config.cc',
    'src/hid_utils.cc',
    'src/logitech_devices.cc',
    'src/main.cc',
    'src/tui.cc',
    'src/utils.cc',
]

executable(meson.project_name(), sources: sources, dependencies: dependencies, include_directories : configuration_inc)