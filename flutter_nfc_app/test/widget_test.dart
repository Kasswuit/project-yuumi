import 'package:flutter_test/flutter_test.dart';
import 'package:flutter_nfc_app/main.dart';

void main() {
  testWidgets('app shows NFC reader title', (tester) async {
    await tester.pumpWidget(const NfcStarterApp());

    expect(find.text('NFC Tag Reader'), findsOneWidget);
  });
}
