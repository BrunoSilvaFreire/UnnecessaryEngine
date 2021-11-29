import 'dart:math';

class Entity {
  String? name;
  late List<Entity> _children;

  static const kDefaultEntityName = "Unnamed Entity";

  List<Entity> get children => _children;

  get label => name ?? kDefaultEntityName;

  Entity(
    this.name,
  ) {
    _children = List.generate(
      Random.secure().nextInt(3),
      (index) => Entity(
          "Child of " + (name ?? kDefaultEntityName) + " #" + index.toString()),
    );
  }
}
