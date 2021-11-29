import 'package:editor/algorithm/tree.dart';
import 'package:editor/components/dock.dart';
import 'package:editor/components/hierarchy_view.dart';
import 'package:editor/components/project_view.dart';
import 'package:editor/components/window.dart';
import 'package:flutter/material.dart';
import 'package:material_design_icons_flutter/material_design_icons_flutter.dart';

class MenuItem {
  String name;
  IconData? icon;
  VoidCallback? action;

  MenuItem(
    this.name, {
    this.action,
    this.icon,
  });
}

class UnnecessaryEditor extends StatefulWidget {
  UnnecessaryEditor({Key? key}) : super(key: key) {
    menuItems = Tree(null);
    var scene = menuItems.addChild(
      MenuItem("Scene", icon: MdiIcons.earth),
    );
    scene.addChild(MenuItem("New scene", icon: MdiIcons.earthPlus));
    scene.addChild(MenuItem("Open scene", icon: MdiIcons.earthArrowRight));
    scene.addChild(MenuItem("Close scene", icon: MdiIcons.close));
    var edit = menuItems.addChild(MenuItem("Edit", icon: MdiIcons.fileEdit));
    edit.addChild(MenuItem("Create Entity", icon: MdiIcons.accountPlus));
  }

  late Tree<MenuItem> menuItems;

  static ThemeData createUnnecessaryTheme() {
    return ThemeData.from(
      colorScheme: const ColorScheme.dark(
          primary: Colors.cyanAccent,
          secondary: Colors.cyan,
          background: Colors.black),
    );
  }

  @override
  State<UnnecessaryEditor> createState() => _UnnecessaryEditorState();
}

class _UnnecessaryEditorState extends State<UnnecessaryEditor> {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Unnecessary Editor',
      theme: UnnecessaryEditor.createUnnecessaryTheme(),
      home: Scaffold(
        body: Column(
          children: [
            buildTopBar(),
            Expanded(
              child: Dock(
                child: Dock(
                  direction: DockDirection.down,
                  child: Dock(
                    offset: .25,
                    child: WindowWidget(
                      child: HierarchyView(),
                    ),
                    other: Dock(
                      child: Container(),
                    ),
                  ),
                  offset: .75,
                  other: Dock(
                    child: WindowWidget(
                      child: ProjectView(),
                    ),
                  ),
                ),
                offset: 0.8,
                other: Dock(
                  child: WindowWidget(
                    child: HierarchyView(),
                  ),
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }

  Widget buildTopBar() {
    List<Widget> widgets = [];
    for (var value in widget.menuItems.children) {
      var iconData = value.data?.icon;
      if (iconData != null) {
        widgets.add(
          PopupMenuButton(
            itemBuilder: (context) {
              return selectSubItems(value);
            },
            child: getTitleFor(value),
            // child: buildTextTitle(value),
          ),
        );
      } else {
        widgets.add(
          TextButton(
            onPressed: () {},
            child: buildTextTitle(value),
          ),
        );
      }
    }
    return SizedBox(
      height: 48,
      child: Card(
        child: Padding(
          padding: const EdgeInsets.symmetric(horizontal: 8.0),
          child: ButtonBar(
            children: widgets,
            mainAxisSize: MainAxisSize.max,
            alignment: MainAxisAlignment.start,
          ),
        ),
      ),
    );
  }

  Widget getTitleFor(Tree<MenuItem> value) {
    var icon = value.data!.icon;
    if (icon != null) {
      return Row(
        children: [
          Container(
              margin: const EdgeInsets.symmetric(horizontal: 4),
              child: Icon(icon)),
          buildTextTitle(value)
        ],
      );
    }
    return buildTextTitle(value);
  }

  Text buildTextTitle(Tree<MenuItem> value) =>
      Text(value.data?.name ?? "Unknown");

  List<PopupMenuItem<MenuItem>> selectSubItems(Tree<MenuItem> value) {
    return [
      for (var a in value.children)
        PopupMenuItem(
          onTap: a.data?.action,
          child: getTitleFor(a),
          value: a.data,
        )
    ];
  }
}
