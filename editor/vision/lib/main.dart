import 'package:flutter/material.dart';
import 'package:unnecessary_ui/tool.dart';

void main() {
  runApp(const VisionTool());
}

class VisionTool extends StatelessWidget {
  const VisionTool({Key? key}) : super(key: key);

  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Vision',
      theme: ThemeData.dark(),
      home: ToolWindow(),
    );
  }
}
