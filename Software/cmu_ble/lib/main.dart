import 'dart:async';
import 'dart:math';
import 'dart:typed_data';
import 'package:cmu_ble/amplitude_range_edit_dialog.dart';
import 'package:cmu_ble/utils.dart';
import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:get/get.dart';
import 'package:cmu_ble/app_controller.dart';
import 'package:wakelock/wakelock.dart';

void main() => runApp(new MyApp());

class EmptyAppBar extends StatelessWidget implements PreferredSizeWidget {
  @override
  Widget build(BuildContext context) {
    return Container(
      color: Theme.of(context).primaryColor,
    );
  }

  @override
  Size get preferredSize => Size(0.0, 0.0);
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    SystemChrome.setSystemUIOverlayStyle(const SystemUiOverlayStyle(
      systemNavigationBarColor: Colors.black,
    ));
    return GetMaterialApp(
      title: "CMU",
      home: CMU(),
      debugShowCheckedModeBanner: false,
      themeMode: ThemeMode.dark,
      theme: ThemeData(
        primarySwatch: Colors.red
      ),
    );
  }
}

class CMU extends StatefulWidget {
  @override
  _CMUState createState() => _CMUState();
}

class _CMUState extends State<CMU> {
  AppController controller = Get.put(AppController());

  @override
  initState() {
    super.initState();
    Wakelock.enable();
  }

  @override
  void dispose() async {
    Wakelock.disable();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Colors.black,
      body: SafeArea(
        child: GetBuilder(
          init: controller,
          builder: mainBodyBuilder,
        )
      ),
    );
  }

  Widget mainBodyBuilder(AppController controller) {
    if (controller.isConnectedAndRunning) {
      // Мы подключены, отображаем экран в соответствии со статусом импланта
      return SingleChildScrollView(
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.center,
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            const SizedBox(height: 16),
            const Text('MAX9814 AGC:', textAlign: TextAlign.center, style: TextStyle(fontSize: 22.0, color: Colors.white, fontWeight: FontWeight.bold)),
            const SizedBox(height: 8),
            const Text('Attack = 1.2 ms\nHold = 30 ms\nRelease = 4800 ms', textAlign: TextAlign.center, style: TextStyle(fontSize: 17.0, color: Colors.white, fontWeight: FontWeight.normal)),
            const SizedBox(height: 15),
            Container(height: 1, color: Colors.red),
            const SizedBox(height: 16),
            const Text('Фильтры IIR-формата:', textAlign: TextAlign.center, style: TextStyle(fontSize: 22.0, color: Colors.white, fontWeight: FontWeight.bold)),
            const SizedBox(height: 8),
            const Text('Блокировка DC 15 Гц (2-го порядка)\nНЧ 0 - 200 Гц (6-го порядка)\nСЧ 600 - 3000 Гц (6-го порядка)\nВЧ 4500 Гц и выше (6-го порядка)', textAlign: TextAlign.center, style: TextStyle(fontSize: 17.0, color: Colors.white, fontWeight: FontWeight.normal)),
            const SizedBox(height: 15),
            Container(height: 1, color: Colors.red),
            const SizedBox(height: 16),
            const Text('Интегратор:', textAlign: TextAlign.center, style: TextStyle(fontSize: 22.0, color: Colors.white, fontWeight: FontWeight.bold)),
            const SizedBox(height: 16),
            const Text('Интегратор красного:', textAlign: TextAlign.center, style: TextStyle(fontSize: 17.0, color: Colors.white, fontWeight: FontWeight.normal)),
            Slider(
              min: 1.0,
              max: 5.0,
              divisions: 4,
              value: controller.dspSettings.redIntegrator.toDouble(),
              onChanged: (double newVal) {
                setState(() {
                  controller.dspSettings.redIntegrator = newVal.toInt();
                });
              },
              onChangeEnd: (double newVal) {
                // Send the change
                setState(() {
                  controller.dspSettings.redIntegrator = newVal.toInt();
                });
                controller.updateDspSettings(context);
              },
            ),
            const SizedBox(height: 16),
            const Text('Интегратор зеленого:', textAlign: TextAlign.center, style: TextStyle(fontSize: 17.0, color: Colors.white, fontWeight: FontWeight.normal)),
            Slider(
              min: 1.0,
              max: 5.0,
              divisions: 4,
              value: controller.dspSettings.greenIntegrator.toDouble(),
              onChanged: (double newVal) {
                setState(() {
                  controller.dspSettings.greenIntegrator = newVal.toInt();
                });
              },
              onChangeEnd: (double newVal) {
                // Send the change
                setState(() {
                  controller.dspSettings.greenIntegrator = newVal.toInt();
                });
                controller.updateDspSettings(context);
              },
            ),
            const SizedBox(height: 16),
            const Text('Интегратор синего:', textAlign: TextAlign.center, style: TextStyle(fontSize: 17.0, color: Colors.white, fontWeight: FontWeight.normal)),
            Slider(
              min: 1.0,
              max: 5.0,
              divisions: 4,
              value: controller.dspSettings.blueIntegrator.toDouble(),
              onChanged: (double newVal) {
                setState(() {
                  controller.dspSettings.blueIntegrator = newVal.toInt();
                });
              },
              onChangeEnd: (double newVal) {
                // Send the change
                setState(() {
                  controller.dspSettings.blueIntegrator = newVal.toInt();
                });
                controller.updateDspSettings(context);
              },
            ),
            const SizedBox(height: 8),
            Container(height: 1, color: Colors.red),
            const SizedBox(height: 16),
            const Text('Диапазон амплитуд:', textAlign: TextAlign.center, style: TextStyle(fontSize: 22.0, color: Colors.white, fontWeight: FontWeight.bold)),
            const SizedBox(height: 16),
            const Text('Красный:', textAlign: TextAlign.center, style: TextStyle(fontSize: 17.0, color: Colors.white, fontWeight: FontWeight.normal)),
            InkWell(
              child: Text(controller.dspSettings.redMinAmplitude.toString() + " - " + controller.dspSettings.redMaxAmplitude.toString(), style: const TextStyle(fontSize: 22.0, color: Colors.red, fontWeight: FontWeight.bold)),
              onTap: () {
                showDialog(context: context, builder: (BuildContext context) => const AmplitudeRangeDialog(channel: 0)).then((value) => setState(() {}));
              },
            ),
            const SizedBox(height: 16),
            const Text('Зеленый:', textAlign: TextAlign.center, style: TextStyle(fontSize: 17.0, color: Colors.white, fontWeight: FontWeight.normal)),
            InkWell(
              child: Text(controller.dspSettings.greenMinAmplitude.toString() + " - " + controller.dspSettings.greenMaxAmplitude.toString(), style: const TextStyle(fontSize: 22.0, color: Colors.red, fontWeight: FontWeight.bold)),
              onTap: () {
                showDialog(context: context, builder: (BuildContext context) => const AmplitudeRangeDialog(channel: 1)).then((value) => setState(() {}));
              },
            ),
            const SizedBox(height: 16),
            const Text('Синий:', textAlign: TextAlign.center, style: TextStyle(fontSize: 17.0, color: Colors.white, fontWeight: FontWeight.normal)),
            InkWell(
              child: Text(controller.dspSettings.blueMinAmplitude.toString() + " - " + controller.dspSettings.blueMaxAmplitude.toString(), style: const TextStyle(fontSize: 22.0, color: Colors.red, fontWeight: FontWeight.bold)),
              onTap: () {
                showDialog(context: context, builder: (BuildContext context) => const AmplitudeRangeDialog(channel: 2)).then((value) => setState(() {}));
              },
            ),
            const SizedBox(height: 16),
            Container(height: 1, color: Colors.red),
            const SizedBox(height: 16),
            const Text('Настройки NeoPixel:', textAlign: TextAlign.center, style: TextStyle(fontSize: 22.0, color: Colors.white, fontWeight: FontWeight.bold)),
            const SizedBox(height: 16),
            const Text('Режим:', textAlign: TextAlign.center, style: TextStyle(fontSize: 17.0, color: Colors.white, fontWeight: FontWeight.normal)),
            DropdownButton<int>(
              dropdownColor: Colors.black,
              value: controller.neoPixelSettings.neopixelMode,
              items: const [
                DropdownMenuItem(
                  child: Text('Смешанный', style: TextStyle(color: Colors.white)),
                  value: 0,
                ),
                DropdownMenuItem(
                  child: Text('Разделенный', style: TextStyle(color: Colors.white)),
                  value: 1,
                ),
                DropdownMenuItem(
                  child: Text('Только красный', style: TextStyle(color: Colors.white)),
                  value: 2,
                ),
                DropdownMenuItem(
                  child: Text('Красный индикатор', style: TextStyle(color: Colors.white)),
                  value: 3,
                ),
                DropdownMenuItem(
                  child: Text('Зеленый индикатор', style: TextStyle(color: Colors.white)),
                  value: 4,
                ),
                DropdownMenuItem(
                  child: Text('Синий индикатор', style: TextStyle(color: Colors.white)),
                  value: 5,
                ),
                DropdownMenuItem(
                  child: Text('Смешанный индикатор', style: TextStyle(color: Colors.white)),
                  value: 6,
                ),
              ],
              onChanged: (int? val) {
                if (val != null) {
                  setState(() {
                    controller.neoPixelSettings.neopixelMode = val;
                  });
                  controller.updateNeoPixelSettings(context);
                }
              }
            ),
            const SizedBox(height: 16),
            const Text('Яркость красного:', textAlign: TextAlign.center, style: TextStyle(fontSize: 17.0, color: Colors.white, fontWeight: FontWeight.normal)),
            Slider(
              min: 0.0,
              max: 255.0,
              divisions: 256,
              value: controller.neoPixelSettings.maxRedBrightness.toDouble(),
              onChanged: (double newVal) {
                setState(() {
                  controller.neoPixelSettings.maxRedBrightness = newVal.toInt();
                });
              },
              onChangeEnd: (double newVal) {
                // Send the change
                setState(() {
                  controller.neoPixelSettings.maxRedBrightness = newVal.toInt();
                });
                controller.updateNeoPixelSettings(context);
              },
            ),
            const SizedBox(height: 16),
            const Text('Яркость зеленого:', textAlign: TextAlign.center, style: TextStyle(fontSize: 17.0, color: Colors.white, fontWeight: FontWeight.normal)),
            Slider(
              min: 0.0,
              max: 255.0,
              divisions: 256,
              value: controller.neoPixelSettings.maxGreenBrightness.toDouble(),
              onChanged: (double newVal) {
                setState(() {
                  controller.neoPixelSettings.maxGreenBrightness = newVal.toInt();
                });
              },
              onChangeEnd: (double newVal) {
                // Send the change
                setState(() {
                  controller.neoPixelSettings.maxGreenBrightness = newVal.toInt();
                });
                controller.updateNeoPixelSettings(context);
              },
            ),
            const SizedBox(height: 16),
            const Text('Яркость синего:', textAlign: TextAlign.center, style: TextStyle(fontSize: 17.0, color: Colors.white, fontWeight: FontWeight.normal)),
            Slider(
              min: 0.0,
              max: 255.0,
              divisions: 256,
              value: controller.neoPixelSettings.maxBlueBrightness.toDouble(),
              onChanged: (double newVal) {
                setState(() {
                  controller.neoPixelSettings.maxBlueBrightness = newVal.toInt();
                });
              },
              onChangeEnd: (double newVal) {
                // Send the change
                setState(() {
                  controller.neoPixelSettings.maxBlueBrightness = newVal.toInt();
                });
                controller.updateNeoPixelSettings(context);
              },
            ),
            const SizedBox(height: 16),
          ],
        )
      );
    } else {
      // Мы не подключены, отображаем статус подключения
      if (controller.scanResults.isNotEmpty && !controller.deviceSelectCompleter.isCompleted) {
        return Column(
          children: [
            const SizedBox(height: 32),
            Text(controller.status, textAlign: TextAlign.center, style: const TextStyle(fontSize: 18.0, color: Colors.white, fontWeight: FontWeight.bold)),
            const SizedBox(height: 16),
            Expanded(
              child: ScrollConfiguration(
                behavior: MyScrollBehavior(),
                child: ListView.builder(
                    itemCount: controller.scanResults.length,
                    itemBuilder: (BuildContext context, int itemIndex) {
                      return Container(
                          padding: const EdgeInsets.all(8.0),
                          child: InkWell(
                            child: Container(
                              decoration: BoxDecoration(
                                  border: Border.all(color: Colors.red),
                                  borderRadius: const BorderRadius.all(Radius.circular(4.0))
                              ),
                              padding: const EdgeInsets.all(8.0),
                              child: Text(controller.scanResults.elementAt(itemIndex).device.id.id, style: const TextStyle(fontSize: 20.0, color: Colors.white)),
                            ),
                            onTap: () {
                              controller.deviceSelectCompleter.complete(controller.scanResults.elementAt(itemIndex).device);
                            },
                          )
                      );
                    }
                ),
              ),
            )
          ],
        );
      } else {
        return Center(
          child: Text(controller.status, textAlign: TextAlign.center, style: const TextStyle(fontSize: 18.0, color: Colors.white, fontWeight: FontWeight.bold)),
        );
      }
    }
  }
}
