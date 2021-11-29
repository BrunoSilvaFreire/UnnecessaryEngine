class Tree<T> {
  T? data;
  late List<Tree<T>> children;

  Tree(this.data) {
    children = List.empty(growable: true);
  }

  Tree<T> addChild(T data) {
    var tree = Tree(data);
    children.add(tree);
    return tree;
  }
}
