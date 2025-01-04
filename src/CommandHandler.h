#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include <Arduino.h>

class CommandHandler {
public:
    using CommandFunction = void(*)(const String& cmd, const String& arg);
    CommandHandler();
    void addCommand(const String& command, CommandFunction func, const String& description = "");
    void executeCommand(const String& command);
    void helpCommand(const String& command, const String& arg);
    void listen();
    static void printHeader(const String& cmd, bool end = false);

private:
    struct Command {
        String name;
        String description;
        CommandFunction function;
    };

    Command commands[10]; // Maximal 10 Befehle
    int commandCount;

    const char* unknownCmd = "Unknown CMD"; // Fehlermeldung für unbekannte Befehle
    const char* cantAddCmd = "Cant add CMD"; // Fehlermeldung für unbekannte Befehle
};

#endif // COMMANDHANDLER_H
