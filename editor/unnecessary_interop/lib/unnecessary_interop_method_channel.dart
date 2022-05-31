import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'unnecessary_interop_platform_interface.dart';

/// An implementation of [UnnecessaryInteropPlatform] that uses method channels.
class MethodChannelUnnecessaryInterop extends UnnecessaryInteropPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('unnecessary_interop');

  @override
  Future<String?> getPlatformVersion() async {
    final version = await methodChannel.invokeMethod<String>('getPlatformVersion');
    return version;
  }
}
