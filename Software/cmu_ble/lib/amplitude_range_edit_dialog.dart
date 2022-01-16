import 'package:cmu_ble/app_controller.dart';
import 'package:cmu_ble/utils.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:get/get.dart';

const Color GREY_COLOR = Color(0xFF828282);

class AmplitudeRangeDialog extends StatefulWidget {
  final int channel; // 0 - red, 1 - green, 2 - blue
  const AmplitudeRangeDialog({Key? key, required this.channel}) : super(key: key);

  @override
  AmplitudeRangeDialogState createState() => AmplitudeRangeDialogState();
}

class AmplitudeRangeDialogState extends State<AmplitudeRangeDialog> {
  AppController controller = Get.find();

  // Введенные данные
  TextEditingController minController = TextEditingController();
  TextEditingController maxController = TextEditingController();

  bool isProgress = false;

  static const double padding = 20.0;
  static const double cornerRadius = 8.0;
  static const double avatarRadius = 47.0;
  static const double outerAvatarRadius = 50.0;

  AmplitudeRangeDialogState();

  @override
  void initState() {
    super.initState();
    if (widget.channel == 0) {
      minController.text = controller.dspSettings.redMinAmplitude.toString();
      maxController.text = controller.dspSettings.redMaxAmplitude.toString();
    } else if (widget.channel == 1) {
      minController.text = controller.dspSettings.greenMinAmplitude.toString();
      maxController.text = controller.dspSettings.greenMaxAmplitude.toString();
    } else {
      minController.text = controller.dspSettings.blueMinAmplitude.toString();
      maxController.text = controller.dspSettings.blueMaxAmplitude.toString();
    }
  }

  @override
  void dispose() {
    minController.dispose();
    maxController.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return WillPopScope(child: Dialog(
      shape: RoundedRectangleBorder(
        borderRadius: BorderRadius.circular(padding),
      ),
      elevation: 0.0,
      backgroundColor: Colors.transparent,
      child: dialogContent(context),
    ), onWillPop: () => Future.value(!isProgress));
  }

  Widget dialogContent(BuildContext context) {
    return Stack(
      children: <Widget>[
        Container(
            width: 450,
            padding: const EdgeInsets.only(
              top: avatarRadius + padding,
              bottom: padding,
              left: padding,
              right: padding,
            ),
            margin: const EdgeInsets.only(top: avatarRadius),
            decoration: BoxDecoration(
              color: Theme.of(context).dialogBackgroundColor,
              shape: BoxShape.rectangle,
              borderRadius: BorderRadius.circular(cornerRadius),
              boxShadow: const [
                BoxShadow(
                  color: Colors.black26,
                  blurRadius: 10.0,
                  offset: Offset(0.0, 10.0),
                ),
              ],
            ),
            child: SingleChildScrollView(
              child: Column(
                mainAxisSize: MainAxisSize.min,
                crossAxisAlignment: CrossAxisAlignment.start,
                children: <Widget>[
                  Center(
                      child: Text(
                        'Редактировать диапазон'.tr,
                        style: const TextStyle(
                          fontSize: 24.0,
                          fontWeight: FontWeight.w700,
                        ),
                        textAlign: TextAlign.center,
                      )
                  ),
                  const SizedBox(height: 16.0),
                  numberTextField(minController, 'Минимум', 4, !isProgress),
                  const SizedBox(height: 8.0),
                  numberTextField(maxController, 'Максимум', 4, !isProgress),
                  const SizedBox(height: 8.0),
                  Center(
                    child: (isProgress) ? const Padding(padding: EdgeInsets.all(10), child: CircularProgressIndicator()) : TextButton(
                      onPressed: () async {
                        // Заполненность проверить
                        if (minController.text.isEmpty) {
                          Utils.showToast(context, 'Минимум пуст.', true);
                          return;
                        }
                        if (maxController.text.isEmpty) {
                          Utils.showToast(context, 'Максимум пуст.', true);
                          return;
                        }

                        int? maxVal;
                        int? minVal;
                        maxVal = int.tryParse(maxController.text.toString());
                        minVal = int.tryParse(minController.text.toString());

                        if ((maxVal == null) || (minVal == null)) {
                          Utils.showToast(context, 'Одно из чисел неверно.', true);
                          return;
                        }
                        if ((maxVal < 0) || (maxVal > 4096) || (minVal < 0) || (minVal > 4096)) {
                          Utils.showToast(context, 'Диапазон от 0 до 4096.', true);
                          return;
                        }

                        // Выставить настройки
                        if (widget.channel == 0) {
                          controller.dspSettings.redMinAmplitude = minVal;
                          controller.dspSettings.redMaxAmplitude = maxVal;
                        } else if (widget.channel == 1) {
                          controller.dspSettings.greenMinAmplitude = minVal;
                          controller.dspSettings.greenMaxAmplitude = maxVal;
                        } else {
                          controller.dspSettings.blueMinAmplitude = minVal;
                          controller.dspSettings.blueMaxAmplitude = maxVal;
                        }

                        // Сохранить диапазон
                        setState(() {
                          isProgress = true;
                        });
                        await controller.updateDspSettings(context);
                        Get.back();
                      },
                      child: Text('OK'.tr),
                    ),
                  ),
                ],
              ),
            ),
        ),
        Positioned(
          left: padding,
          right: padding,
          child: CircleAvatar(
            backgroundColor: Theme.of(context).primaryColor,
            radius: outerAvatarRadius,
            child: const CircleAvatar(
              backgroundImage: AssetImage('assets/mainicon.png'),
              radius: avatarRadius,
              backgroundColor: Colors.white,
            ),
          ),
        ),
      ],
    );
  }

  Widget numberTextField(TextEditingController contr, String hint, int maxLength, bool enabled) {
    return TextField(
      enabled: enabled,
      controller: contr,
      textAlign: TextAlign.center,
      decoration: InputDecoration(
          contentPadding: const EdgeInsets.symmetric(vertical: 10.0, horizontal: 25.0),
          border: const OutlineInputBorder(
            borderRadius: BorderRadius.all(
              Radius.circular(30.0),
            ),
          ),
          enabledBorder: const OutlineInputBorder(
            borderRadius: BorderRadius.all(
              Radius.circular(30.0),
            ),
            borderSide: BorderSide(color: GREY_COLOR, width: 1.0),
          ),
          focusedBorder: const OutlineInputBorder(
            borderRadius: BorderRadius.all(
              Radius.circular(30.0),
            ),
            borderSide: BorderSide(color: GREY_COLOR, width: 1.0),
          ),
          filled: true,
          hintStyle: const TextStyle(color: GREY_COLOR),
          hintText: hint,
          fillColor: Colors.transparent
      ),
      keyboardType: TextInputType.number,
      inputFormatters: [FilteringTextInputFormatter.allow(RegExp("[0-9]"))],
      maxLines: 1,
      maxLength: maxLength,
      buildCounter: (BuildContext context, {required int currentLength, required bool isFocused, required int? maxLength}) => Container(),
      style: const TextStyle(fontWeight: FontWeight.w500),
    );
  }
}