import 'package:flutter/material.dart';

enum DockDirection { up, down, left, right }

class Dock extends StatelessWidget {
  Dock({
    Key? key,
    this.other,
    required this.child,
    this.direction = DockDirection.right,
    this.offset = 0.5,
  }) : super(key: key);
  final Dock? other;
  final Widget child;
  DockDirection direction;
  double offset;

  @override
  Widget build(BuildContext context) {
    Size size = MediaQuery.of(context).size;
    Widget base;

    if (other != null) {
      Size first, second;
      switch (direction) {
        case DockDirection.up:
          double firstHeight = size.height * offset;
          double remaining = size.height - firstHeight;
          first = Size(size.width, firstHeight);
          second = Size(size.width, remaining);
          break;
        case DockDirection.down:
          double fw = size.height * offset;
          double remaining = size.height - fw;
          first = Size(size.width, fw);
          second = Size(size.width, remaining);
          break;
        case DockDirection.left:
          double firstWidth = size.width * offset;
          double remaining = size.width - firstWidth;
          first = Size(firstWidth, size.height);
          second = Size(remaining, size.height);
          break;
        case DockDirection.right:
          double firstWidth = size.width * offset;
          double remaining = size.width - firstWidth;
          first = Size(firstWidth, size.height);
          second = Size(remaining, size.height);
          break;
      }
      switch (direction) {
        case DockDirection.up:
        case DockDirection.down:
          base = Column(
            children: [
              SizedBox(
                width: first.width,
                height: first.height,
                child: child,
              ),
              SizedBox(
                width: second.width,
                height: second.height,
                child: other,
              )
            ],
          );
          break;
        case DockDirection.left:
        case DockDirection.right:
          base = Row(
            children: [
              SizedBox(
                width: first.width,
                height: first.height,
                child: child,
              ),
              SizedBox(
                width: second.width,
                height: second.height,
                child: other,
              )
            ],
          );
          break;
      }
    } else {
      base = SizedBox(
        width: size.width,
        height: size.height,
        child: Placeholder(),
      );
    }

    return base;
  }
}
