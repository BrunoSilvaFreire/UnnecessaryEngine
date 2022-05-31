import 'package:flutter/material.dart';

import 'package:menubar/menubar.dart';

class Action {
  final String name;
  final VoidCallback onClick;
  final List<Action> _children = List.empty(growable: true);

  Action(this.name, this.onClick);
}

class Tool {}

class ToolWindow extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: MenuBar(body: body, menus: menus),
    );
  }
}
