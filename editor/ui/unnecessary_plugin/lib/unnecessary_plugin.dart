
import 'dart:async';

import 'package:flutter/services.dart';

class UnnecessaryPlugin {
  static const MethodChannel _channel = MethodChannel('unnecessary_plugin');

  static MethodChannel get channel => _channel;

  static Future<String?> get platformVersion async {
    final String? version = await _channel.invokeMethod('getPlatformVersion');
    return version;
  }
}
