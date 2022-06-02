import 'dart:convert';

import 'package:flutter/material.dart';
import 'package:unnecessary_ui/tool.dart';
import 'package:file_picker/file_picker.dart';
import 'package:vision/model/recording.dart';
import 'package:vision/vision.dart';

void main() {
  runApp(const VisionTool());
}

class VisionTool extends StatefulWidget {
  const VisionTool({Key? key}) : super(key: key);

  @override
  State<VisionTool> createState() => _VisionToolState();
}

class _VisionToolState extends State<VisionTool> {


  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Vision',
      theme: ThemeData.from(
        colorScheme: const ColorScheme.dark(primary: Colors.cyanAccent),
        useMaterial3: true,
      ),
      home: ToolWindow(VisionController().tool),
    );
  }
}
