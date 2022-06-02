import 'package:flutter/material.dart';
import 'package:docking/docking.dart';

class EditorWindow extends StatelessWidget {
  final String title;
  final WidgetBuilder child;

  EditorWindow(this.title, this.child);

  @override
  Widget build(BuildContext context) {
    return Card(child: Builder(builder: child));
  }
}
