import 'package:editor/algorithm/tree.dart';
import 'package:flutter/material.dart';

class MenuItem {
  String name;
  IconData? icon;
  VoidCallback? action;
  bool iconOnly;

  MenuItem(this.name, {this.action, this.icon, this.iconOnly = false});
}

class DropdownMenuButton extends StatelessWidget {
  final Tree<MenuItem> tree;

  const DropdownMenuButton(this.tree);

  Text _buildTextTitle(Tree<MenuItem> value) =>
      Text(value.data?.name ?? "Unknown");

  Widget _getTitleFor(Tree<MenuItem> value) {
    var data = value.data!;
    var icon = data.icon;
    if (data.iconOnly) {
      return Icon(icon);
    }
    if (icon != null) {
      return Row(
        children: [
          Container(
              margin: const EdgeInsets.symmetric(horizontal: 4),
              child: Icon(icon)),
          _buildTextTitle(value)
        ],
      );
    }
    return _buildTextTitle(value);
  }

  List<PopupMenuItem<MenuItem>> _selectSubItems(Tree<MenuItem> value) {
    return [
      for (var a in value.children)
        PopupMenuItem(
          onTap: a.data?.action,
          child: _getTitleFor(a),
          value: a.data,
        )
    ];
  }

  @override
  Widget build(BuildContext context) {
    var data = tree.data!;
    if (data.iconOnly) {
      return PopupMenuButton(
        itemBuilder: (context) {
          return _selectSubItems(tree);
        },
        icon: Icon(data.icon),
        // child: buildTextTitle(value),
      );
    }
    return PopupMenuButton(
      tooltip: null,
      itemBuilder: (context) {
        return _selectSubItems(tree);
      },
      child: _getTitleFor(tree),
      // child: buildTextTitle(value),
    );
  }
}
