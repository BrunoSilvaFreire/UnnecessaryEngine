// GENERATED CODE - DO NOT MODIFY BY HAND

part of 'dock_layout.dart';

// **************************************************************************
// JsonSerializableGenerator
// **************************************************************************

DockLayout _$DockLayoutFromJson(Map<String, dynamic> json) => DockLayout(
      json['windowType'] as String,
      (json['offset'] as num).toDouble(),
    );

Map<String, dynamic> _$DockLayoutToJson(DockLayout instance) =>
    <String, dynamic>{
      'windowType': instance.windowType,
      'offset': instance.offset,
    };
