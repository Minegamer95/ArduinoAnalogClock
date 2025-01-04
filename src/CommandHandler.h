#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include <Arduino.h>

class CommandHandler {
public:
  using CommandFunction = void (*)(const String &cmd, const String &arg);
  CommandHandler();
  void addCommand(const String &command, CommandFunction func,
                  const String &description = "", bool displayHeader = true);
  void executeCommand(const String &command);
  void helpCommand(const String &command, const String &arg);
  void listen();
  static void printHeader(const String &cmd, bool end = false);
  static bool isNumber(const String &str);
  static int toNumber(const String &str, int fallback = 0);
  static unsigned long extractTime(const String &str);
  static int CountArgs(const String &str, char separator = ' ');
  static String GetArg(const String &str, int index, char separator = ' ');

private:
  struct Command {
    String name;
    String description;
    bool displayHeaders;
    CommandFunction function;
  };

  Command commands[10]; // Maximal 10 Befehle
  int commandCount;

  const char *unknownCmd =
      "Unknown CMD"; // Fehlermeldung für unbekannte Befehle
  const char *cantAddCmd =
      "Cant add CMD"; // Fehlermeldung für unbekannte Befehle
};

#endif // COMMANDHANDLER_H
