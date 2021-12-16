import 'package:editor/bindings/entity.dart';
import 'package:flutter/material.dart';

class EntityCard extends StatelessWidget {
  final Entity entity;
  final VoidCallback? onClicked;
  final bool selected;

  const EntityCard({
    Key? key,
    required this.entity,
    this.onClicked,
    this.selected = false,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    var name = entity.name;
    return ListTile(
      dense: true,
      selected: selected,
      title: Text(name ?? "Unnamed Entity"),
      onTap: onClicked,
    );
  }
}
