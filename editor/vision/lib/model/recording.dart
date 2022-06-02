import 'dart:collection';
import 'dart:io';
import 'dart:math';
import 'package:dart_numerics/dart_numerics.dart' as num;
import 'package:csv/csv.dart';

enum JobEventType {
  eWorkerStarted,
  eWorkerExited,
  eWorkerSleeping,
  eWorkerAwaken,
  eJobStarted,
  eJobFinished,
  eJobEnqueued,
  eJobUnlocked
}

class JobEvent {
  final int archetype;
  final int worker;
  final int handle;
  final String label;
  final int time;
  final JobEventType type;

  JobEvent(this.archetype, this.worker, this.handle, this.label, this.time,
      this.type);

  static JobEvent fromCSV(List e) {
    var handleCandidate = e[2];
    int handle = -1;
    if (handleCandidate is int) {
      handle = handleCandidate;
    }
    return JobEvent(
      e[0],
      e[1],
      handle,
      e[3],
      e[4],
      JobEventType.values[e[5]],
    );
  }
}

class WorkerGroup {
  final List<JobEvent> events = List.empty(growable: true);
}

class ArchetypeGroup {
  final List<WorkerGroup?> workers = List.empty(growable: true);

  void add(JobEvent value) {
    if (value.worker >= workers.length) {
      workers.length = value.worker + 1;
    }
    var worker = workers[value.worker];
    if (worker == null) {
      workers[value.worker] = worker = WorkerGroup();
    }
    worker.events.add(value);
  }
}

class Recording {
  final List<JobEvent> events;
  late List<ArchetypeGroup?> archetypes;
  late int start = num.int64MaxValue, finish = num.int64MinValue;

  Recording(this.events) {
    archetypes = List<ArchetypeGroup?>.empty(growable: true);
    for (var value in events) {
      if (value.archetype >= archetypes.length) {
        archetypes.length = value.archetype + 1;
      }
      var group = archetypes[value.archetype];
      if (group == null) {
        archetypes[value.archetype] = group = ArchetypeGroup();
      }
      start = min(start, value.time);
      finish = max(finish, value.time);
      group.add(value);
    }
    for (var value in archetypes) {
      for (var worker in value!.workers) {
        worker!.events.sort((a, b) {
          return a.time.compareTo(b.time);
        });
      }
    }
  }

  static Recording fromCSV(String csv) {
    var convert = CsvToListConverter().convert(csv);
    return Recording(convert.skip(1).map((e) => JobEvent.fromCSV(e)).toList());
  }
}
