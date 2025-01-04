#include "CommandHandler.h"

CommandHandler::CommandHandler() : commandCount(0) {}

void CommandHandler::addCommand(const String &command, CommandFunction func,
                                const String &description, bool displayHeader) {
  if (commandCount < 10) { // Überprüfen, ob Platz für einen neuen Befehl ist
    commands[commandCount].name = command;
    commands[commandCount].function = func;
    commands[commandCount].description = description;
    commands[commandCount].displayHeaders = displayHeader;
    commandCount++;
  } else {
    printHeader(cantAddCmd);
    Serial.print(command);
    Serial.print(": ");
    Serial.println(description);
    printHeader(cantAddCmd, true);
  }
}

void CommandHandler::executeCommand(const String &command) {
  String cmd = command;
  int spaceIndex = cmd.indexOf(' ');
  String arg = (spaceIndex > 0) ? cmd.substring(spaceIndex + 1)
                                : ""; // Extrahiere die Argumente
  // Extrahiere nur den Befehl (alles vor dem ersten Leerzeichen)
  String commandName = (spaceIndex > 0) ? cmd.substring(0, spaceIndex) : cmd;

  for (int i = 0; i < commandCount; i++) {
    Command cmd = commands[i];
    if (cmd.name.equalsIgnoreCase(commandName)) {
      if (cmd.displayHeaders)
        printHeader(cmd.name);
      commands[i].function(cmd.name, arg); // Führe die zugehörige Funktion aus
      if (cmd.displayHeaders)
        printHeader(cmd.name, true);
      return;
    }
  }
  printHeader(unknownCmd);
  Serial.println(commandName);
  Serial.println("Type 'help' for a list off Commands");
  printHeader(unknownCmd, true);
}

void CommandHandler::listen() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    executeCommand(input);
  }
}

void CommandHandler::helpCommand(const String &command, const String &arg) {
  for (int i = 0; i < commandCount; i++) {
    Serial.print(commands[i].name);
    Serial.print(": ");
    Serial.println(commands[i].description);
  }
}

void CommandHandler::printHeader(const String &cmd, bool end) {
  if (!end)
    Serial.println();
  Serial.println("-------- " + cmd + " --------");
}

bool CommandHandler::isNumber(const String &str) {
  for (unsigned int i = 0; i < str.length(); i++) {
    if (!isdigit(str.charAt(i))) {
      return false;
    }
  }
  return true;
}

int CommandHandler::toNumber(const String &str, int fallback) {
  if (isNumber(str)) {
    return str.toInt();
  } else {
    return fallback;
  }
}

/// @brief Gibt die Anzahl an enthaltenen argumenten zurück, immer mindestens 1
/// @param str Der String in dem die Argumente gesucht werden sollen
/// @param separator Der Char der als trenzeichen fungieren soll
/// @return Die Anzahl an vorhandenen argumenten, immer 1 oder mehr
int CommandHandler::CountArgs(const String &str, char separator) {
  int argCount = 1;
  for (unsigned int i = 0; i < str.length(); i++) {
    if (str.charAt(i) == separator)
      argCount++;
  }
  return argCount;
}

String CommandHandler::GetArg(const String &str, int index, char separator) {
  int seperatorCount = 0;
  unsigned int startIndex = 0;
  unsigned int endIndex = str.length();

  // Wenn der index out of range ist einfach einen leeren String zurückgeben
  if (index >= CountArgs(str, separator))
    return "";

  for (unsigned int i = 0; i < str.length(); i++) {
    if (str.charAt(i) == separator) {
      seperatorCount++;
      if (seperatorCount == index)
        startIndex = i + 1;
      else if (seperatorCount == index + 1)
        endIndex = i;
    }
  }

  if (startIndex >= endIndex)
    return "";
  else
    return str.substring(startIndex, endIndex);
}

unsigned long CommandHandler::extractTime(const String &str) {
  int argCount = CountArgs(str, ':');

  // Die Zeit muss in hh:mm oder hh:mm:ss formatiert sein
  if (argCount != 2 && argCount != 3)
    return 0;

  unsigned long time = 0;
  time +=
      toNumber(GetArg(str, 0, ':')) * 3600UL;   // Stunden in Sekunden umrechnen
  time += toNumber(GetArg(str, 1, ':')) * 60UL; // Minuten in Sekunden umrechnen

  // Sekunden sind optional und müssen nicht angegeben werden
  if (argCount == 3)
    time += (unsigned long)toNumber(GetArg(str, 2, ':'));

  return time;
}