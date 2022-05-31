#include "include/unnecessary_interop/unnecessary_interop_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "unnecessary_interop_plugin.h"

void UnnecessaryInteropPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  unnecessary_interop::UnnecessaryInteropPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
