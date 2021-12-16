import 'package:editor/components/window.dart';
import 'package:flutter/material.dart';
import 'package:material_design_icons_flutter/material_design_icons_flutter.dart';

class ProjectView extends Window {
  @override
  Widget build(BuildContext context) {
    return Container();
  }

  ProjectView() : super(MdiIcons.folder, 'Project');
}

class ProjectViewFactory extends WindowFactory {
  ProjectViewFactory() : super("project_view");

  @override
  Window create() {
    return ProjectView();
  }
}
