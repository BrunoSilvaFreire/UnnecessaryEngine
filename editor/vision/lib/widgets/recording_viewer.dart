import 'package:flutter/material.dart';
import 'package:timelines/timelines.dart';
import 'package:vision/model/recording.dart';

class RecordingViewer extends StatelessWidget {
  final Recording recording;

  const RecordingViewer({super.key, required this.recording});

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.all(8.0),
      child: Column(
        children: [
          for (var value in recording.archetypes)
            buildArchetype(context, value!)
        ],
      ),
    );
  }

  Container buildArchetype(BuildContext context, ArchetypeGroup value) {
    return Container(
      child: Card(
        elevation: 0,
        color: Theme.of(context).colorScheme.surfaceVariant,
        child: Row(
          children: [
            Icon(Icons.architecture),
            Text(
                "Archetype: ${recording.archetypes.indexOf(value)} (${value.workers.length} workers)"),
            Column(
              children: [
                for (var worker in value.workers)
                  buildWorker(context, worker!)
              ],
            )
          ],
        ),
      ),
    );
  }

  Widget buildWorker(BuildContext context, WorkerGroup worker) {
    return Container(
      child: Row(children: [
        Text("Worker"),
        for (var e in worker.events) buildEvent(e)
      ]),
    );
  }

  Widget buildEvent(JobEvent e) {
    Color col;
    switch (e.type) {
      case JobEventType.eWorkerStarted:
        col = Colors.greenAccent;
        break;
      case JobEventType.eWorkerExited:
        col = Colors.redAccent;
        break;
      case JobEventType.eWorkerSleeping:
        col = Colors.lightBlueAccent;
        break;
      case JobEventType.eWorkerAwaken:
        col = Colors.yellowAccent;
        break;
      case JobEventType.eJobStarted:
        col = Colors.purpleAccent;
        break;
      case JobEventType.eJobFinished:
        col = Colors.deepPurpleAccent;
        break;
      case JobEventType.eJobEnqueued:
        col = Colors.cyanAccent;
        break;
      case JobEventType.eJobUnlocked:
        col = Colors.lightGreen;
        break;
    }
    return Tooltip(
      message: e.label,
      showDuration: const Duration(milliseconds: 1),
      preferBelow: true,
      child: Container(
        height: 8,
        width: 8,
        decoration: BoxDecoration(
          shape: BoxShape.circle,
          color: col,
        ),
        margin: EdgeInsets.all(8),
      ),
    );
  }
}
