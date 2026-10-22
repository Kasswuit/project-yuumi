import 'package:flutter/material.dart';
import 'package:nfc_manager/nfc_manager.dart';

void main() {
  runApp(const NfcStarterApp());
}

class NfcStarterApp extends StatelessWidget {
  const NfcStarterApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'NFC Tag Reader',
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(seedColor: Colors.deepPurple),
        useMaterial3: true,
      ),
      home: const NfcReaderPage(),
    );
  }
}

class NfcReaderPage extends StatefulWidget {
  const NfcReaderPage({super.key});

  @override
  State<NfcReaderPage> createState() => _NfcReaderPageState();
}

class _NfcReaderPageState extends State<NfcReaderPage> {
  String _status = 'Ready to scan';
  String _tagData = 'No tag scanned yet.';
  bool _isReading = false;

  Future<void> _startScan() async {
    final available = await NfcManager.instance.isAvailable();
    if (!available) {
      setState(() {
        _status = 'NFC is not available on this device';
        _tagData = 'No tag scanned.';
      });
      return;
    }

    setState(() {
      _isReading = true;
      _status = 'Hold the device near an NFC tag';
      _tagData = 'Waiting for a tag...';
    });

    try {
      await NfcManager.instance.startSession(
        onDiscovered: (tag) async {
          setState(() {
            _status = 'Tag detected';
            _tagData = tag.data.toString();
            _isReading = false;
          });

          await NfcManager.instance.stopSession();
        },
      );
    } catch (e) {
      setState(() {
        _status = 'Scan failed';
        _tagData = e.toString();
        _isReading = false;
      });
    }
  }

  Future<void> _stopScan() async {
    await NfcManager.instance.stopSession();
    setState(() {
      _status = 'Session stopped';
      _isReading = false;
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('NFC Tag Reader'),
      ),
      body: Padding(
        padding: const EdgeInsets.all(24),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,
          children: [
            Card(
              child: Padding(
                padding: const EdgeInsets.all(16),
                child: Column(
                  children: [
                    Text(
                      _status,
                      style: Theme.of(context).textTheme.titleMedium,
                    ),
                    const SizedBox(height: 12),
                    Text(
                      _tagData,
                      style: Theme.of(context).textTheme.bodyLarge,
                    ),
                  ],
                ),
              ),
            ),
            const SizedBox(height: 24),
            FilledButton.icon(
              onPressed: _isReading ? null : _startScan,
              icon: const Icon(Icons.nfc_rounded),
              label: const Text('Scan NFC tag'),
            ),
            const SizedBox(height: 12),
            OutlinedButton.icon(
              onPressed: _isReading ? _stopScan : null,
              icon: const Icon(Icons.stop_circle_outlined),
              label: const Text('Stop session'),
            ),
          ],
        ),
      ),
    );
  }
}
