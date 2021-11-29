typedef TreeConfigurator<T> = void Function(Tree<T> tree);

class Tree<T> {
  T? data;
  late List<Tree<T>> children;

  Tree.create(this.data, TreeConfigurator<T> configurator) {
    children = List.empty(growable: true);
    configurator(this);
  }

  Tree(this.data) {
    children = List.empty(growable: true);
  }

  Tree<T> addChild(T data) {
    var tree = Tree(data);
    children.add(tree);
    return tree;
  }

  Tree<T> addAndConfigureChild(T data, TreeConfigurator<T> configurator) {
    var tree = Tree.create(data, configurator);
    children.add(tree);
    return tree;
  }

  bool get isEmpty => children.isEmpty;
  bool get isNotEmpty => children.isNotEmpty;
}
