import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

class IghGamePage extends StatefulWidget {
  const IghGamePage({super.key});

  @override
  State<IghGamePage> createState() => _IghGamePageState();
}

class _IghGamePageState extends State<IghGamePage> {
  static const _gameChannel = MethodChannel('igh.educativo/game');
  bool _opening = false;

  Future<void> _openGame() async {
    if (_opening) return;

    setState(() => _opening = true);

    try {
      await _gameChannel.invokeMethod('openGame', <String, dynamic>{
        'userId': 1,
        'topicId': 1,
        'attemptId': 1,
      });
    } on PlatformException catch (e) {
      if (!mounted) return;
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('No se pudo abrir IGH EDUCATIVO: ${e.message}')),
      );
    } finally {
      if (mounted) {
        setState(() => _opening = false);
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Juegos')),
      body: Center(
        child: SizedBox(
          width: 300,
          height: 160,
          child: Card(
            clipBehavior: Clip.antiAlias,
            child: InkWell(
              onTap: _opening ? null : _openGame,
              child: Padding(
                padding: const EdgeInsets.all(20),
                child: Column(
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: [
                    const Icon(Icons.engineering, size: 48),
                    const SizedBox(height: 10),
                    const Text(
                      'IGH EDUCATIVO',
                      style: TextStyle(
                        fontSize: 22,
                        fontWeight: FontWeight.bold,
                      ),
                    ),
                    const SizedBox(height: 8),
                    Text(_opening ? 'Abriendo...' : 'Toca para jugar'),
                  ],
                ),
              ),
            ),
          ),
        ),
      ),
    );
  }
}
