import 'dart:io';
import 'package:file_picker/file_picker.dart';
import 'package:flutter/material.dart';
import 'package:rxdart/rxdart.dart';
import 'package:unnecessary_ui/layout/hierarchy.dart';
import 'package:unnecessary_ui/tool.dart';
import 'package:vision/widgets/recording_viewer.dart';
import 'model/recording.dart';

class VisionController {
  static final VisionController _singleton = VisionController._internal();

  factory VisionController() {
    return _singleton;
  }

  VisionController._internal() {
    openRecording.listen((value) {
      tool.addWindow(EditorWindow(
        "Recording Viewer",
        (context) {
          return RecordingViewer(recording: value);
        },
      ));
    });
  }

  final BehaviorSubject<Recording> openRecording = BehaviorSubject();
  final Tool tool = Tool()
    ..setCategories([
      MenuCategory(
        "File",
        actions: [
          MenuAction(
            "Open",
            () async {
              var onPicked = await FilePicker.platform.pickFiles(
                  dialogTitle: "Select Job Recording",
                  allowedExtensions: [".csv"],
                  allowMultiple: false);
              if (onPicked == null) {
                return;
              }
              var path = onPicked.paths.single;
              if (path == null) {
                return;
              }

              var recording = Recording.fromCSV(File(path).readAsStringSync());
              VisionController().openRecording.add(recording);
            },
          )
        ],
      ),
    ]);
}
