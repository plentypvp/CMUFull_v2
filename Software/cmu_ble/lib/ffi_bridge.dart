import 'dart:ffi';
import 'dart:io';
import 'dart:core';
import 'package:ffi/ffi.dart';
import 'package:get/get.dart';

typedef GetNeoPixelSettingsFunction = NeoPixelSettingsStruct Function(Pointer<Uint8> arr);
typedef GetNeoPixelSettingsFunctionDart = NeoPixelSettingsStruct Function(Pointer<Uint8> arr);
typedef GetDspSettingsFunction = DspSettingsStruct Function(Pointer<Uint8> arr);
typedef GetDspSettingsFunctionDart = DspSettingsStruct Function(Pointer<Uint8> arr);

typedef SetNeoPixelSettingsFunction = Pointer<Uint8> Function(NeoPixelSettingsStruct structure);
typedef SetNeoPixelSettingsFunctionDart = Pointer<Uint8> Function(NeoPixelSettingsStruct structure);
typedef SetDspSettingsFunction = Pointer<Uint8> Function(DspSettingsStruct structure);
typedef SetDspSettingsFunctionDart = Pointer<Uint8> Function(DspSettingsStruct structure);

class NeoPixelSettings {
  int neopixelMode = 1;
  int maxRedBrightness = 255;
  int maxGreenBrightness = 70;
  int maxBlueBrightness = 70;

  NeoPixelSettings({required this.neopixelMode, required this.maxRedBrightness, required this.maxGreenBrightness, required this.maxBlueBrightness});

  Map toMap() {
    return {
      'neopixelMode': neopixelMode,
      'maxRedBrightness': maxRedBrightness,
      'maxGreenBrightness': maxGreenBrightness,
      'maxBlueBrightness': maxBlueBrightness,
    };
  }

  @override
  String toString() {
    return toMap().toString();
  }

  static initial() {
    return NeoPixelSettings(
      neopixelMode: 1,
      maxRedBrightness: 255,
      maxGreenBrightness: 70,
      maxBlueBrightness: 70,
    );
  }
}

class DspSettings {
  int redIntegrator = 3;
  int redMinAmplitude = 60;
  int redMaxAmplitude = 1024;
  int greenIntegrator = 3;
  int greenMinAmplitude = 60;
  int greenMaxAmplitude = 1024;
  int blueIntegrator = 3;
  int blueMinAmplitude = 60;
  int blueMaxAmplitude = 1024;

  DspSettings({required this.redIntegrator, required this.redMinAmplitude, required this.redMaxAmplitude, required this.greenIntegrator, required this.greenMinAmplitude, required this.greenMaxAmplitude, required this.blueIntegrator, required this.blueMinAmplitude, required this.blueMaxAmplitude});

  Map toMap() {
    return {
      'redIntegrator': redIntegrator,
      'redMinAmplitude': redMinAmplitude,
      'redMaxAmplitude': redMaxAmplitude,
      'greenIntegrator': greenIntegrator,
      'greenMinAmplitude': greenMinAmplitude,
      'greenMaxAmplitude': greenMaxAmplitude,
      'blueIntegrator': blueIntegrator,
      'blueMinAmplitude': blueMinAmplitude,
      'blueMaxAmplitude': blueMaxAmplitude,
    };
  }

  @override
  String toString() {
    return toMap().toString();
  }

  static initial() {
    return DspSettings(
      redIntegrator: 3,
      redMinAmplitude: 60,
      redMaxAmplitude: 1024,
      greenIntegrator: 3,
      greenMinAmplitude: 60,
      greenMaxAmplitude: 1024,
      blueIntegrator: 3,
      blueMinAmplitude: 60,
      blueMaxAmplitude: 1024
    );
  }
}

class NeoPixelSettingsStruct extends Struct {
  @Uint8()
  external int neopixelMode;

  @Uint8()
  external int maxRedBrightness;

  @Uint8()
  external int maxGreenBrightness;

  @Uint8()
  external int maxBlueBrightness;
}

class DspSettingsStruct extends Struct {
  @Uint8()
  external int redIntegrator;

  @Uint8()
  external int greenIntegrator;

  @Uint8()
  external int blueIntegrator;

  @Uint16()
  external int redMinAmplitude;

  @Uint16()
  external int redMaxAmplitude;

  @Uint16()
  external int greenMinAmplitude;

  @Uint16()
  external int greenMaxAmplitude;

  @Uint16()
  external int blueMinAmplitude;

  @Uint16()
  external int blueMaxAmplitude;
}

class FfiBridge {
  static const int NEOPIXEL_SIZE = 4;
  static const int DSP_SIZE = 16;
  GetNeoPixelSettingsFunctionDart? _getNeopixelSettings;
  GetDspSettingsFunctionDart? _getDspSettings;
  
  SetNeoPixelSettingsFunctionDart? _setNeopixelSettings;
  SetDspSettingsFunctionDart? _setDspSettings;

  FfiBridge() {
    final dl = Platform.isAndroid
        ? DynamicLibrary.open('libstructs.so')
        : DynamicLibrary.process();

    _getNeopixelSettings = dl.lookupFunction<GetNeoPixelSettingsFunction, GetNeoPixelSettingsFunctionDart>("get_neopixel_settings");
    _getDspSettings = dl.lookupFunction<GetDspSettingsFunction, GetDspSettingsFunctionDart>("get_dsp_settings");
    _setNeopixelSettings = dl.lookupFunction<SetNeoPixelSettingsFunction, SetNeoPixelSettingsFunctionDart>("set_neopixel_settings");
    _setDspSettings = dl.lookupFunction<SetDspSettingsFunction, SetDspSettingsFunctionDart>("set_dsp_settings");
  }

  Pointer<Uint8> uint8ListToArray(List<int> list) {
    final ptr = malloc.allocate<Uint8>(sizeOf<Uint8>() * list.length);
    for (var i = 0; i < list.length; i++) {
      ptr.elementAt(i).value = list[i];
    }
    return ptr;
  }

  NeoPixelSettings getNeopixelSettings(List<int> event) {
    NeoPixelSettingsStruct nativeStruct = _getNeopixelSettings!(uint8ListToArray(event));
    return NeoPixelSettings(
      neopixelMode: nativeStruct.neopixelMode,
      maxRedBrightness: nativeStruct.maxRedBrightness,
      maxGreenBrightness: nativeStruct.maxGreenBrightness,
      maxBlueBrightness: nativeStruct.maxBlueBrightness
    );
  }

  DspSettings getDspSettings(List<int> combined) {
    DspSettingsStruct nativeStruct = _getDspSettings!(uint8ListToArray(combined));
    print(nativeStruct.redMaxAmplitude);
    return DspSettings(
      redIntegrator: nativeStruct.redIntegrator,
      redMaxAmplitude: nativeStruct.redMaxAmplitude,
      redMinAmplitude: nativeStruct.redMinAmplitude,
      greenIntegrator: nativeStruct.greenIntegrator,
      greenMaxAmplitude: nativeStruct.greenMaxAmplitude,
      greenMinAmplitude: nativeStruct.greenMinAmplitude,
      blueIntegrator: nativeStruct.blueIntegrator,
      blueMaxAmplitude: nativeStruct.blueMaxAmplitude,
      blueMinAmplitude: nativeStruct.blueMinAmplitude,
    );
  }

  List<int> setNeopixelSettings(NeoPixelSettings settings_new) {
    NeoPixelSettingsStruct nativeStruct = _getNeopixelSettings!(uint8ListToArray(List.generate(NEOPIXEL_SIZE, (index) => 0)));

    nativeStruct.neopixelMode = settings_new.neopixelMode;
    nativeStruct.maxRedBrightness = settings_new.maxRedBrightness;
    nativeStruct.maxGreenBrightness = settings_new.maxGreenBrightness;
    nativeStruct.maxBlueBrightness = settings_new.maxBlueBrightness;

    Pointer<Uint8> result = _setNeopixelSettings!(nativeStruct);
    return List.generate(NEOPIXEL_SIZE, (index) => result.elementAt(index).value);
  }
  
  List<int> setDspSettings(DspSettings settings_new) {
    DspSettingsStruct nativeStruct = _getDspSettings!(uint8ListToArray(List.generate(DSP_SIZE, (index) => 0)));
    
    nativeStruct.redMinAmplitude = settings_new.redMinAmplitude;
    nativeStruct.redMaxAmplitude = settings_new.redMaxAmplitude;
    nativeStruct.redIntegrator = settings_new.redIntegrator;
    nativeStruct.greenMinAmplitude = settings_new.greenMinAmplitude;
    nativeStruct.greenMaxAmplitude = settings_new.greenMaxAmplitude;
    nativeStruct.greenIntegrator = settings_new.greenIntegrator;
    nativeStruct.blueMinAmplitude = settings_new.blueMinAmplitude;
    nativeStruct.blueMaxAmplitude = settings_new.blueMaxAmplitude;
    nativeStruct.blueIntegrator = settings_new.blueIntegrator;
    
    Pointer<Uint8> result = _setDspSettings!(nativeStruct);
    return List.generate(DSP_SIZE, (index) => result.elementAt(index).value);
  }
}