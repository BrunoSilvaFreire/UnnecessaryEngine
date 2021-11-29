import 'package:unnecessary_plugin/unnecessary_plugin.dart';

class Property {
  String name;

  Property(this.name);
}

class ComponentMeta {
  String name;
  List<Property> properties;

  ComponentMeta(this.name, this.properties);
}

class Entity {
  Future<List<T>> _invokeList<T>(String method) {
    return UnnecessaryPlugin.channel.invokeListMethod(method, this)
        as Future<List<T>>;
  }

  Future<T> _invoke<T>(String method) {
    return UnnecessaryPlugin.channel.invokeMethod(method, this) as Future<T>;
  }

  Future<List<ComponentMeta>> findComponents() => _invokeList("findComponents");

  Future<String> findName() {
    return _invoke("findName");
  }

  void setName(String name) async {
    UnnecessaryPlugin.channel.invokeMethod("setName", [this, name]);
  }
}
