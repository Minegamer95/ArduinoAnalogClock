#ifndef STRINGBUILDER_H
#define STRINGBUILDER_H

#include <Arduino.h>
class StringBuilder {
public:
  StringBuilder(bool clearAfterPrint) {
    buffer = "";
    autoClearOnPrint = clearAfterPrint;
  }

  // Überladung für C-Strings (const char*)
  StringBuilder &operator<<(const char *str) {
    buffer += str; // Füge den C-String hinzu
    return *this;
  }

  // Überladung für char-Arrays (char[])
  StringBuilder &operator<<(char *str) {
    buffer += str; // Füge den C-String hinzu
    return *this;
  }

  // Überladung für String-Objekte
  StringBuilder &operator<<(const String &str) {
    buffer += str; // Füge das String-Objekt hinzu
    return *this;
  }

  // Überladung für int-Werte
  StringBuilder &operator<<(int value) {
    buffer += String(value); // Füge den int-Wert als String hinzu
    return *this;
  }

  // Überladung für unsigned long-Werte
  StringBuilder &operator<<(unsigned long value) {
    buffer += String(value); // Füge den unsigned long-Wert als String hinzu
    return *this;
  }

  // Überladung für float-Werte
  StringBuilder &operator<<(float value) {
    buffer += String(value); // Füge den float-Wert als String hinzu
    return *this;
  }

  // Überladung für double-Werte
  StringBuilder &operator<<(double value) {
    buffer += String(value); // Füge den double-Wert als String hinzu
    return *this;
  }

  // Ausgabe des zusammengebauten Strings
  void print() {
    Serial.print(buffer);
    if (autoClearOnPrint)
      clear();
  }

  // Ausgabe des zusammengebauten Strings
  void println() {
    Serial.println(buffer);
    if (autoClearOnPrint)
      clear();
  }

  // Leere den Puffer
  void clear() {
    buffer = ""; // Leere den Puffer
  }

private:
  String buffer; // Puffer für den zusammengebauten String
  bool autoClearOnPrint;
};
#endif