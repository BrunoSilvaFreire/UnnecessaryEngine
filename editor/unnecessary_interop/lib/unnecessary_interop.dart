
import 'unnecessary_interop_platform_interface.dart';

class UnnecessaryInterop {
  Future<String?> getPlatformVersion() {
    return UnnecessaryInteropPlatform.instance.getPlatformVersion();
  }
}
