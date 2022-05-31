import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:unnecessary_interop/unnecessary_interop_method_channel.dart';

void main() {
  MethodChannelUnnecessaryInterop platform = MethodChannelUnnecessaryInterop();
  const MethodChannel channel = MethodChannel('unnecessary_interop');

  TestWidgetsFlutterBinding.ensureInitialized();

  setUp(() {
    channel.setMockMethodCallHandler((MethodCall methodCall) async {
      return '42';
    });
  });

  tearDown(() {
    channel.setMockMethodCallHandler(null);
  });

  test('getPlatformVersion', () async {
    expect(await platform.getPlatformVersion(), '42');
  });
}
