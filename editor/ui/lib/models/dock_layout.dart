import 'package:editor/components/dock.dart';
import 'package:json_annotation/json_annotation.dart';

part 'dock_layout.g.dart';

@JsonSerializable(createFactory: true)
class DockLayout {
  final String windowType;
  double offset;
  DockLayout? child;

  DockLayout(this.windowType, this.offset, {child});
}
