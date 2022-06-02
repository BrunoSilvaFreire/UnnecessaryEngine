import 'package:docking/docking.dart';
import 'package:flutter/material.dart' hide MenuItem;
import 'package:menubar/menubar.dart';
import 'package:rxdart/rxdart.dart';
import 'package:material_design_icons_flutter/material_design_icons_flutter.dart';

import 'layout/hierarchy.dart';

class MenuAction {
  final String name;
  final VoidCallback onClick;

  MenuAction(this.name, this.onClick);
}

class MenuCategory {
  final String name;
  List<MenuAction> actions = List.empty(growable: true);
  final List<MenuCategory> subCategories = List.empty(growable: true);

  MenuCategory(this.name, {required this.actions});

  void addActions(Iterable<MenuAction> toAdd) {
    actions.addAll(toAdd);
  }

  Submenu widget() {
    return Submenu(label: name, children: [
      for (var cat in subCategories) cat.widget(),
      for (var act in actions)
        MenuItem(
          label: act.name,
          onClicked: act.onClick,
        ),
    ]);
  }
}

class Tool {
  List<MenuCategory>? _categories;
  final List<EditorWindow> _windows = List.empty(growable: true);
  final BehaviorSubject<List<EditorWindow>> windowStream = BehaviorSubject();

  void setCategories(List<MenuCategory> menuCategories) {
    _categories = menuCategories;
    setApplicationMenu(menuCategories.map((e) => e.widget()).toList());
  }

  List<EditorWindow> get windows => _windows;

  void addWindow(EditorWindow window) {
    _windows.add(window);
    windowStream.add(_windows);
  }

  void deleteWindow(EditorWindow window) {
    _windows.remove(window);
    windowStream.add(_windows);
  }
}

class ToolWindow extends StatelessWidget {
  final Tool tool;

  ToolWindow(this.tool);

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: StreamBuilder<List<EditorWindow>>(
          stream: tool.windowStream,
          builder: (context, snapshot) {
            List<EditorWindow>? data = snapshot.data;
            if (data == null || data.isEmpty) {
              return const Center(child: Text("It's so lonely in here :("));
            }
            var list = data
                .map(
                  (e) => DockingItem(name: e.title, widget: e),
                )
                .toList();
            DockingArea root;
            if (list.length == 1) {
              root = list.single;
            } else {
              root = DockingColumn(list);
            }
            return Docking(
              layout: DockingLayout(root: root),
              onItemClose: (item) {
                tool.deleteWindow(item.widget as EditorWindow);
              },
            );
          }),
    );
  }
}
