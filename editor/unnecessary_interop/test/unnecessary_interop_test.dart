import 'package:flutter_test/flutter_test.dart';
import 'package:unnecessary_interop/unnecessary_interop.dart';
import 'package:unnecessary_interop/unnecessary_interop_platform_interface.dart';
import 'package:unnecessary_interop/unnecessary_interop_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockUnnecessaryInteropPlatform 
    with MockPlatformInterfaceMixin
    implements UnnecessaryInteropPlatform {

  @override
  Future<String?> getPlatformVersion() => Future.value('42');
}

void main() {
  final UnnecessaryInteropPlatform initialPlatform = UnnecessaryInteropPlatform.instance;

  test('$MethodChannelUnnecessaryInterop is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelUnnecessaryInterop>());
  });

  test('getPlatformVersion', () async {
    UnnecessaryInterop unnecessaryInteropPlugin = UnnecessaryInterop();
    MockUnnecessaryInteropPlatform fakePlatform = MockUnnecessaryInteropPlatform();
    UnnecessaryInteropPlatform.instance = fakePlatform;
  
    expect(await unnecessaryInteropPlugin.getPlatformVersion(), '42');
  });
}
