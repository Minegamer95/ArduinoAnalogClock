#include "CommandHandler.h"

CommandHandler::CommandHandler() : commandCount(0) {}

void CommandHandler::addCommand(const String& command, CommandFunction func, const String& description) {
    if (commandCount < 10) { // Überprüfen, ob Platz für einen neuen Befehl ist
        commands[commandCount].name = command;
        commands[commandCount].function = func;
        commands[commandCount].description = description;
        commandCount++;
    }
    else{
        printHeader(cantAddCmd);
        Serial.print(command);
        Serial.print(": ");
        Serial.println(description);
        printHeader(cantAddCmd, true);
    }
}

void CommandHandler::executeCommand(const String& command) {
    String cmd = command;
    cmd.toLowerCase();
    int spaceIndex = cmd.indexOf(' ');
    String arg = (spaceIndex > 0) ? cmd.substring(spaceIndex + 1) : ""; // Extrahiere die Argumente
    // Extrahiere nur den Befehl (alles vor dem ersten Leerzeichen)
    String commandName = (spaceIndex > 0) ? cmd.substring(0, spaceIndex) : cmd;

    for (int i = 0; i < commandCount; i++) {
        if (commands[i].name.equals(commandName)) {
            commands[i].function(commandName, arg); // Führe die zugehörige Funktion aus
            return;
        }
    }
    printHeader(unknownCmd);
    Serial.println(commandName);
    printHeader(unknownCmd, true);
}

void CommandHandler::listen(){
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        executeCommand(input);
  }
}

void CommandHandler::helpCommand(const String& command, const String& arg) {
    printHeader(command);
    for (int i = 0; i < commandCount; i++) {
        Serial.print(commands[i].name);
        Serial.print(": ");
        Serial.println(commands[i].description);
    }
    printHeader(command);
}

void CommandHandler::printHeader(const String& cmd, bool end){
    if(!end)
        Serial.println();
    Serial.println("-------- "+cmd+" --------");

}
