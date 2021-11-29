import 'dart:ui';

import 'package:editor/components/dock.dart';
import 'package:editor/components/project_view.dart';
import 'package:editor/components/hierarchy_view.dart';
import 'package:editor/components/unnecessary_editor.dart';
import 'package:editor/components/window.dart';
import 'package:flutter/material.dart';
import 'package:material_design_icons_flutter/material_design_icons_flutter.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({Key? key}) : super(key: key);

  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return UnnecessaryEditor();
  }
}
