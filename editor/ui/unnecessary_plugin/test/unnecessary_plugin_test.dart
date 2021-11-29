import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:unnecessary_plugin/unnecessary_plugin.dart';

void main() {
  const MethodChannel channel = MethodChannel('unnecessary_plugin');

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
    expect(await UnnecessaryPlugin.platformVersion, '42');
  });
}
