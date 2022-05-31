import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'unnecessary_interop_method_channel.dart';

abstract class UnnecessaryInteropPlatform extends PlatformInterface {
  /// Constructs a UnnecessaryInteropPlatform.
  UnnecessaryInteropPlatform() : super(token: _token);

  static final Object _token = Object();

  static UnnecessaryInteropPlatform _instance = MethodChannelUnnecessaryInterop();

  /// The default instance of [UnnecessaryInteropPlatform] to use.
  ///
  /// Defaults to [MethodChannelUnnecessaryInterop].
  static UnnecessaryInteropPlatform get instance => _instance;
  
  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [UnnecessaryInteropPlatform] when
  /// they register themselves.
  static set instance(UnnecessaryInteropPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<String?> getPlatformVersion() {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }
}
