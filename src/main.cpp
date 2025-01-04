#include "CommandHandler.h"
#include "PhysicalClock.h"
#include "StringBuilder.h"
#include <Arduino.h>

// Commands
void statusCmd(const String &cmd, const String &args);
void setDisplayTimeCmd(const String &cmd, const String &args);
void setTargetTimeCmd(const String &cmd, const String &args);
void togglePauseCmd(const String &cmd, const String &args);
void tickDisplay(const String &cmd, const String &args);
void tickDisplaySec(const String &cmd, const String &args);

void cmdTemplate(const String &cmd, const String &args);

PhysicalClock clock = PhysicalClock(0, 10, 11, true, 800, 720, true);
CommandHandler commandHandler;
uLong currentTime = 0;

// Language Text
const char *txtChangeFrom = "Von ";
const char *txtChangeTo = "Zu ";
const char *txtDispTime = "Anzeige-Zeit: ";
const char *txtDispPos = "Anzeige-Tick: ";
const char *txtTargTime = "Ziel-Zeit: ";
const char *txtTargPos = "Ziel-Tick: ";
const char *txtArduinoTime = "Arduino Zeit: ";
const char *txtPause = "Die Uhr ist: ";
const char *txtRunning = "Aktiv";
const char *txtStopped = "Angehalten";
const char *txtTimeDiffForeward = "Zeitdiffernz Vorwärts: ";
const char *txtTimeDiffBackward = "Zeitdiffernz Rückwärts: ";
const char *txtTicks = " Ticks";
const char *txtSeconds = " Sekunden";
const char *txtTime = " Uhr";
const char *txtTimeLong = " HH:MM:SS";
const char *txtListSeperator = ", ";

void setup() {
  TCCR1A = 0;                                        // CTC Mode
  TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); // Prescaler 1024
  TCNT1 = 0;                                         // Zähler auf 0
  OCR1A = 15624;                                     // 1 Sekunde
  TIMSK1 |= (1 << OCIE1A);                           // Interrupt aktiviere

  sei(); // Allow interrupts

  // Register Command
  commandHandler.addCommand(
      "help",
      [](const String &cmd, const String &arg) {
        commandHandler.helpCommand(cmd, arg);
      },
      "Displays the list of available commands.");
  commandHandler.addCommand("status", statusCmd, "Zeigt Status");
  commandHandler.addCommand(
      "setDispTime", setDisplayTimeCmd,
      "Setzt die Display Time, setDispTime <hh:mm>/<hh:mm:ss>");
  commandHandler.addCommand(
      "setTargTime", setTargetTimeCmd,
      "Setzt die Target Time, setTargTime <hh:mm>/<hh:mm:ss>");
  commandHandler.addCommand(
      "tick", setTargetTimeCmd,
      "Erhöht die Display Tick um die eingegebene Zahl, tick <int>");
  commandHandler.addCommand("tickTime", setTargetTimeCmd,
                            "Erhöht die Display Time um die gegebene Zeit, "
                            "tickTime <hh:mm>/<hh:mm:ss>");
  commandHandler.addCommand("toggel", togglePauseCmd,
                            "Started oder Stop die Uhr");

  Serial.begin(115200);
  Serial.println("Boot complete");
}

void loop() {
  clock.update();
  commandHandler.listen();
  delay(20);
}

ISR(TIMER1_COMPA_vect) {
  currentTime++;
  currentTime %= clock.getDayLengthSec();
  clock.setTargetTimeSec(currentTime);
}

void statusCmd(const String &cmd, const String &args) {
  StringBuilder sb = StringBuilder(true);
  sb << txtDispTime << clock.getTimeStr(TimeType::Display);
  sb.println();

  sb << txtDispPos << clock.getDisplayPosition();
  sb.println();

  sb << txtTargTime << clock.getTimeStr(TimeType::Target);
  sb.println();

  sb << txtTargPos << clock.getTargetPosition();
  sb.println();

  sb << txtArduinoTime << clock.timestampToStr(currentTime);
  sb.println();

  sb << txtPause << (clock.getPaused() ? txtStopped : txtRunning);
  sb.println();

  sb << txtTimeDiffForeward << clock.getForwardTickDiff() << txtTicks
     << txtListSeperator << clock.timestampToStr(clock.getForwardTickDiffSec())
     << txtTimeLong;
  sb.println();

  sb << txtTimeDiffBackward << clock.getBackwardTickDiff() << txtTicks
     << txtListSeperator << clock.timestampToStr(clock.getBackwardTickDiffSec())
     << txtTimeLong;
  sb.println();
};

void setDisplayTimeCmd(const String &cmd, const String &args) {
  StringBuilder sb = StringBuilder(true);
  sb << txtChangeFrom << txtDispTime << clock.getTimeStr(TimeType::Display);
  sb.println();

  clock.setDisplayTimeSec(CommandHandler::extractTime(args));

  sb << txtChangeTo << txtDispTime << clock.getTimeStr(TimeType::Display);
  sb.println();
};

void setTargetTimeCmd(const String &cmd, const String &args) {
  StringBuilder sb = StringBuilder(true);
  sb << txtChangeFrom << txtTargTime << clock.getTimeStr(TimeType::Target);
  sb.println();

  currentTime = CommandHandler::extractTime(args);
  currentTime %= clock.getDayLengthSec();
  clock.setTargetTimeSec(currentTime);

  sb << txtChangeTo << txtTargTime << clock.getTimeStr(TimeType::Target);
  sb.println();
};

void togglePauseCmd(const String &cmd, const String &args) {
  clock.setPaused(!clock.getPaused());
  StringBuilder sb = StringBuilder(true);
  sb << txtPause << (clock.getPaused() ? txtStopped : txtRunning);
  sb.println();
};

void tickDisplay(const String &cmd, const String &args) {
  StringBuilder sb = StringBuilder(true);
  sb << txtChangeFrom << txtDispTime << clock.getTimeStr(TimeType::Display);
  sb.println();

  uLong newPos = clock.getDisplayPosition() + CommandHandler::toNumber(args);
  clock.setDisplayPosition(newPos);

  sb << txtChangeTo << txtDispTime << clock.getTimeStr(TimeType::Display);
  sb.println();
}

void tickDisplaySec(const String &cmd, const String &args) {
  StringBuilder sb = StringBuilder(true);
  sb << txtChangeFrom << txtDispTime << clock.getTimeStr(TimeType::Display);
  sb.println();

  clock.setDisplayTimeSec(CommandHandler::extractTime(args) +
                          clock.getDisplayTimeSec());

  sb << txtChangeTo << txtDispTime << clock.getTimeStr(TimeType::Display);
  sb.println();
}

void cmdTemplate(const String &cmd, const String &args) {
  CommandHandler::printHeader(cmd);
  CommandHandler::printHeader(cmd, true);
};
