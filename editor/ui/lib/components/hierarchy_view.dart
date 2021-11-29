import 'package:editor/bindings/entity.dart';
import 'package:editor/components/entity.dart';
import 'package:editor/components/window.dart';
import 'package:flutter/material.dart';
import 'package:flutter_treeview/flutter_treeview.dart';
import 'package:material_design_icons_flutter/material_design_icons_flutter.dart';
import 'package:rxdart/subjects.dart';

class HierarchyView extends Window {
  @override
  Widget build(BuildContext context) {
    return HierarchyViewWidget();
  }

  HierarchyView() : super(MdiIcons.fileTree, "Hierarchy");
}

class HierarchyViewWidget extends StatefulWidget {
  final List<Entity> entities = List.generate(
    3,
    (index) => Entity(
      index % 2 == 0 ? null : "Entity " + index.toString(),
    ),
  );

  @override
  State createState() {
    return _HierarchyViewState();
  }
}

class _HierarchyViewState extends State<HierarchyViewWidget> {
  final BehaviorSubject<int> _selected = BehaviorSubject();
  TreeViewController? _treeController;

  _HierarchyViewState() {
    _selected.add(-1);
  }

  Node<Entity> createEntityNode(Entity e) {
    return Node(
      key: e.name ?? "unnamed",
      label: e.label,
      data: e,
      children: [for (Entity child in e.children) createEntityNode(child)],
    );
  }

  @override
  void initState() {
    super.initState();
    _treeController ??= TreeViewController(
        children: [for (var value in widget.entities) createEntityNode(value)]);
  }

  @override
  Widget build(BuildContext context) {
    var c = _treeController;
    if (c == null) {
      return Container();
    }
    return TreeView(
        controller: c,
        nodeBuilder: (context, node) {
          if (node.data == null) {
            return const Placeholder();
          }
          return EntityCard(
            entity: node.data,
            selected: node.expanded,
          );
        });
  }

  List<ExpansionPanel> buildEntities(
      List<Entity> entities, int currentlySelected) {
    return [
      for (int i = 0; i < entities.length; i++)
        buildEntityPanel(entities, i, currentlySelected)
    ];
  }

  ExpansionPanel buildEntityPanel(
      List<Entity> entities, int i, int currentlySelected) {
    var entity = entities[i];
    var children = buildEntities(entity.children, currentlySelected);
    return ExpansionPanel(
      headerBuilder: (context, isExpanded) {
        return EntityCard(
          entity: entity,
          selected: currentlySelected == i,
        );
      },
      body: children.isEmpty
          ? Container()
          : ExpansionPanelList(
              children: children,
            ),
      canTapOnHeader: true,
      isExpanded: currentlySelected == i,
    );
  }
}
