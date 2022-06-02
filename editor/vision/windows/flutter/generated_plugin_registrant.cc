//
//  Generated file. Do not edit.
//

// clang-format off

#include "generated_plugin_registrant.h"

#include <menubar/menubar_plugin.h>
#include <unnecessary_interop/unnecessary_interop_plugin_c_api.h>

void RegisterPlugins(flutter::PluginRegistry* registry) {
  MenubarPluginRegisterWithRegistrar(
      registry->GetRegistrarForPlugin("MenubarPlugin"));
  UnnecessaryInteropPluginCApiRegisterWithRegistrar(
      registry->GetRegistrarForPlugin("UnnecessaryInteropPluginCApi"));
}
