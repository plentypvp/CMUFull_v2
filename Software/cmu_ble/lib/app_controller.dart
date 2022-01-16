import 'dart:async';
import 'dart:math';
import 'dart:typed_data';
import 'dart:ui';

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:flutter_blue/flutter_blue.dart';
import 'package:get/get.dart';
import 'package:cmu_ble/ffi_bridge.dart';
import 'package:cmu_ble/future_progress_dialog.dart';

class AppController extends FullLifeCycleController {
  // Защита от повторных вызовов функции подключения
  bool connectionPending = false;
  bool isReading = false;

  // Bluetooth и ссылка на устройство
  FlutterBlue flutterBlue = FlutterBlue.instance;
  BluetoothDevice? cmuDevice;

  // Подписки на статус соединения и значения ADC
  StreamSubscription? connectStatusSubscription;

  // Записываемые характеристики
  BluetoothCharacteristic? neopixelChar;
  BluetoothCharacteristic? dspChar;

  // Отображаемые данные
  bool isConnectedAndRunning = false; // отображать экран подключения или соответствующий статусу
  String status = "Инициализация..."; // статус подключения
  List<ScanResult> scanResults = [];
  Completer deviceSelectCompleter = Completer();

  // После подключения
  NeoPixelSettings neoPixelSettings = NeoPixelSettings.initial(); // настройки NeoPixel
  DspSettings dspSettings = DspSettings.initial(); // настройки DSP

  // Нативный интерфейс
  final FfiBridge ffiBridge = FfiBridge();

  // Константы
  final Guid cmuServiceGuid = Guid("00001523-1111-eaed-1523-745eeabcd121");
  final Guid cmuModeCharGuid = Guid("00001525-1111-eaed-1523-745eeabcd121");
  final Guid cmuDspCharGuid = Guid("00001526-1111-eaed-1523-745eeabcd121");
  final String cmuName = "ArtemChip-Light";

  Future<void> attemptConnect() async {
    // Предотвратить двойной вызов
    if (connectionPending) return;
    connectionPending = true;

    while (true) {
      try {
        // Если мы были подключены и сделали hot restart, соединение не сбрасывается, выполняем это сами
        status = "Инициализация...";
        update();
        await attemptDisconnect(false);

        // Соединение сброшено, можем искать, только сперва останавливаем поиск который уже идет (если идет)
        status = "Поиск устройств...";
        update();
        scanResults = await flutterBlue.scan(
          timeout: const Duration(seconds: 5),
          withServices: [cmuServiceGuid],
        ).toList();

        // Нашли устройство раньше завершения поиска, значит его останавливаем.
        await flutterBlue.stopScan();
        if (scanResults.isEmpty) {
          throw Exception("Устройств не найдено.");
        }

        // Отображаем список устройств и предлагаем выбор
        deviceSelectCompleter = Completer();
        status = "Выберите устройство из списка.";
        update();

        // Выбрали устройство, подключаемся
        cmuDevice = await deviceSelectCompleter.future;
        status = "Выполняется подключение...";
        update();
        await cmuDevice!.connect(timeout: const Duration(seconds: 5));

        // Список сервисов
        status = "Запрос сервисов...";
        update();
        List<BluetoothService> serviceList = await cmuDevice!.discoverServices();
        BluetoothService pulseService = serviceList.firstWhere((element) => element.uuid == cmuServiceGuid, orElse: () => throw Exception("Сервис в устройстве не найден."));

        // Получаем характеристики и подписываемся
        status = "Запрос характеристик...";
        update();

        // Характеристики
        neopixelChar = pulseService.characteristics.firstWhere((element) => element.uuid == cmuModeCharGuid, orElse: () => throw Exception("Характеристика сервиса NeoPixel не найдена."));
        dspChar = pulseService.characteristics.firstWhere((element) => element.uuid == cmuDspCharGuid, orElse: () => throw Exception("Характеристика сервиса DSP не найдена."));
        await Future.delayed(const Duration(milliseconds: 100));
        if (neopixelChar != null) neopixelChanged(await neopixelChar!.read());
        if (dspChar != null) dspChanged(await dspChar!.read());

        // Обработка потери соединения
        connectStatusSubscription = cmuDevice!.state.listen((BluetoothDeviceState event) {
          if (event == BluetoothDeviceState.disconnected) {
            status = "Соединение потеряно. Переподключение через 3 сек.";
            isConnectedAndRunning = false;
            update();
            attemptDisconnect(true);
          }
        }, onError: (x) {}, cancelOnError: true);

        // Успех
        status = "Запущено.";
        isConnectedAndRunning = true;
        update();
        connectionPending = false;
        break;
      } catch (err) {
        // Повторяем начиная заново
        if (err is PlatformException) {
          PlatformException ple = err;
          print(ple.message);
          print(ple.stacktrace);
        }
        status = err.toString() + "\nПовторная попытка через 3 сек.";
        update();
        await Future.delayed(const Duration(seconds: 3));
      }
    }
  }

  Future<void> attemptDisconnect(bool toReconnect) async {
    await flutterBlue.stopScan();
    await cancelAllSubscriptions();
    List<BluetoothDevice> alreadyConnected = await flutterBlue.connectedDevices;
    for (BluetoothDevice dev in alreadyConnected) {
      if (dev.name == cmuName) await dev.disconnect();
    }
    if (cmuDevice != null) {
      await cmuDevice!.disconnect();
      if (toReconnect) {
        Future.delayed(const Duration(seconds: 3)).then((value) => attemptConnect());
      }
    }
  }

  Future<void> cancelAllSubscriptions() async {
    // Нет подписок
  }

  @override
  void onInit() {
    super.onInit();
    attemptConnect();
  }

  @override
  void onClose() async {
    await attemptDisconnect(false);
    super.onClose();
  }

  @override
  void didChangeAppLifecycleState(AppLifecycleState state) {
    super.didChangeAppLifecycleState(state);
    if (state == AppLifecycleState.inactive) {

    } else if (state == AppLifecycleState.resumed) {

    }
  }

  void neopixelChanged(List<int> event) {
    if (event.length == FfiBridge.NEOPIXEL_SIZE) {
      neoPixelSettings = ffiBridge.getNeopixelSettings(event);
      update();
    }
  }

  void dspChanged(List<int> event) {
    if (event.length == FfiBridge.DSP_SIZE) {
      dspSettings = ffiBridge.getDspSettings(event);
      update();
    }
  }

  Future<void> updateNeoPixelSettings(BuildContext context) {
    if (neopixelChar == null) return Future.value();
    List<int> newSettingsBytes = ffiBridge.setNeopixelSettings(neoPixelSettings);
    return showDialog(context: context, builder: (BuildContext context) => FutureProgressDialog(neopixelChar!.write(newSettingsBytes)));
  }

  Future<void> updateDspSettings(BuildContext context) {
    if (dspChar == null) return Future.value();
    List<int> newSettingsBytes = ffiBridge.setDspSettings(dspSettings);
    return showDialog(context: context, builder: (BuildContext context) => FutureProgressDialog(dspChar!.write(newSettingsBytes)));
  }
}