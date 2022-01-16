import 'package:flutter/material.dart';
import 'package:fluttertoast/fluttertoast.dart';

class Utils {
  static FToast fToast = FToast();

  static void showToast(BuildContext context, String error, bool isCenter) {
    FocusScope.of(context).unfocus(); // hide keyboard
    Utils.fToast.init(context);
    Widget toast = Container(
      padding: const EdgeInsets.symmetric(horizontal: 16.0, vertical: 12.0),
      decoration: BoxDecoration(
        borderRadius: BorderRadius.circular(5.0),
        color: Theme
            .of(context)
            .primaryColor,
      ),
      child: Text(error, style: const TextStyle(color: Colors.white),),
    );
    fToast.removeQueuedCustomToasts();
    fToast.showToast(child: toast,
        fadeDuration: 300,
        toastDuration: const Duration(seconds: 2),
        gravity: (isCenter) ? ToastGravity.CENTER : ToastGravity.BOTTOM);
  }
}

class MyScrollBehavior extends ScrollBehavior {
  @override
  Widget buildViewportChrome(BuildContext context, Widget child, AxisDirection axisDirection) {
    return child;
  }
}