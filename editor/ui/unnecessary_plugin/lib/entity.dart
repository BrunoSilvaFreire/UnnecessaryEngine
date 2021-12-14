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
  Future<List<J>> _invokeList<J>(String method) {
    return UnnecessaryPlugin.channel.invokeListMethod(method, this)
        as Future<List<J>>;
  }

  Future<J> _invoke<J>(String method) {
    return UnnecessaryPlugin.channel.invokeMethod(method, this) as Future<J>;
  }

  Future<List<ComponentMeta>> findComponents() => _invokeList("findComponents");

  Future<String> findName() {
    return _invoke("findName");
  }

  void setName(String name) async {
    UnnecessaryPlugin.channel.invokeMethod("setName", [this, name]);
  }
}
