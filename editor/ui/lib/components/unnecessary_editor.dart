import 'package:editor/algorithm/tree.dart';
import 'package:editor/components/dock.dart';
import 'package:editor/components/dropdown_menu.dart';
import 'package:editor/components/hierarchy_view.dart';
import 'package:editor/components/project_view.dart';
import 'package:editor/components/window.dart';
import 'package:flutter/material.dart';
import 'package:material_design_icons_flutter/material_design_icons_flutter.dart';

class UnnecessaryEditor extends StatefulWidget {
  static List<WindowFactory> windowFactories = [
    HierarchyViewFactory(),
    ProjectViewFactory()
  ];

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
              child: WindowWidget.dockedWindow(
                ProjectView(),
              )
            ),
          ],
        ),
      ),
    );
  }

  Widget buildTopBar() {
    List<Widget> widgets = [];
    for (var value in widget.menuItems.children) {
      widgets.add(DropdownMenuButton(value));
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
}
