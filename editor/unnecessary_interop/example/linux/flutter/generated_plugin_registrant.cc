//
//  Generated file. Do not edit.
//

// clang-format off

#include "generated_plugin_registrant.h"

#include <unnecessary_interop/unnecessary_interop_plugin.h>

void fl_register_plugins(FlPluginRegistry* registry) {
  g_autoptr(FlPluginRegistrar) unnecessary_interop_registrar =
      fl_plugin_registry_get_registrar_for_plugin(registry, "UnnecessaryInteropPlugin");
  unnecessary_interop_plugin_register_with_registrar(unnecessary_interop_registrar);
}
