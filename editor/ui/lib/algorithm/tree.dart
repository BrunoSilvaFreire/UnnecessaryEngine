typedef TreeConfigurator<J> = void Function(Tree<J> tree);

class Tree<J> {
  J? data;
  late List<Tree<J>> children;

  Tree.create(this.data, TreeConfigurator<J> configurator) {
    children = List.empty(growable: true);
    configurator(this);
  }

  Tree(this.data) {
    children = List.empty(growable: true);
  }

  Tree<J> addChild(J data) {
    var tree = Tree(data);
    children.add(tree);
    return tree;
  }

  Tree<J> addAndConfigureChild(J data, TreeConfigurator<J> configurator) {
    var tree = Tree.create(data, configurator);
    children.add(tree);
    return tree;
  }

  bool get isEmpty => children.isEmpty;
  bool get isNotEmpty => children.isNotEmpty;
}
