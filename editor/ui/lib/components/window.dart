import 'package:editor/algorithm/tree.dart';
import 'package:editor/components/dock.dart';
import 'package:editor/components/dropdown_menu.dart';
import 'package:editor/components/unnecessary_editor.dart';
import 'package:flutter/material.dart';
import 'package:material_design_icons_flutter/material_design_icons_flutter.dart';

abstract class Window {
  final IconData? icon;
  final String title;

  Window(this.icon, this.title);

  Widget build(BuildContext context);
}

abstract class WindowFactory {
  String type;

  WindowFactory(this.type);

  Window create();
}

class WindowWidget extends StatelessWidget {
  final Window child;
  void Function(Window other)? onWidgetAdded;
  late Tree<MenuItem> moreTree;

  static Dock dockedWindow(Window child, {double offset = 0.5}) {
    Dock? dock;
    var window = WindowWidget(
      child: child,
      onWidgetAdded: (Window w) {
        if (dock != null) {
          dock.child = Dock(
            child: dock.child,
            other: Dock(
              child: WindowWidget(child: w),
            ),
          );
        } else {
          print('Null dock');
        }
      },
    );
    for (var dir in DockDirection.values) {
      window.moreTree.addChild(
        MenuItem("Set direction to " + dir.name, action: () {
          dock?.direction = dir;
        }),
      );
    }
    dock = Dock(
      offset: offset,
      child: window,
    );
    return dock;
  }

  WindowWidget({
    Key? key,
    required this.child,
    this.onWidgetAdded,
  }) : super(key: key) {
    moreTree = Tree.create(
      MenuItem(
        "Window options",
        action: () {},
        icon: MdiIcons.menuDown,
        iconOnly: true,
      ),
      (tree) {
        if (onWidgetAdded != null) {
          for (var value in UnnecessaryEditor.windowFactories) {
            tree.addChild(
              MenuItem(
                "Split and add " + value.type,
                action: () {
                  onWidgetAdded!(value.create());
                },
              ),
            );
          }
        }
      },
    );
  }

  @override
  Widget build(BuildContext context) {
    var icon = child.icon;
    var title = child.title;

    return Card(
      child: Column(
        children: [
          ListTile(
            leading: icon == null ? null : Icon(icon),
            title: Text(title),
            trailing: moreTree.isNotEmpty
                ? DropdownMenuButton(
                    moreTree,
                  )
                : null,
          ),
          Expanded(
            child: Builder(
              builder: (context) {
                return child.build(context);
              },
            ),
          )
        ],
      ),
    );
  }
}
