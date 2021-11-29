import 'package:flutter/material.dart';
import 'package:material_design_icons_flutter/material_design_icons_flutter.dart';

abstract class Window {
  final IconData? icon;
  final String title;

  Window(this.icon, this.title);

  Widget build(BuildContext context);
}

class WindowWidget extends StatelessWidget {
  final Window child;

  const WindowWidget({
    Key? key,
    required this.child,
  }) : super(key: key);

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
