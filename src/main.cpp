#include "CommandHandler.h"
#include "PhysicalClock.h"
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
const char *changeFromTxt = "Von ";
const char *changeToTxt = "Zu ";
const char *dispTimeTxt = "Display Time: ";
const char *dispPosTxt = "Display Tick: ";
const char *targTimeTxt = "Target Time: ";
const char *targPosTxt = "Target Tick: ";
const char *arduinoTimeTxt = "Arduino Time: ";
const char *pauseTxt = "Clock is now: ";
const char *pauseRunningTxt = "Running";
const char *pauseStoppedTxt = "Stopped";

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
  Serial.println(dispTimeTxt + clock.getTimeStr(TimeType::Display));
  Serial.println(dispPosTxt + String(clock.getDisplayPosition()));
  Serial.println(targTimeTxt + clock.getTimeStr(TimeType::Target));
  Serial.println(targPosTxt + String(clock.getTargetPosition()));
  Serial.println(arduinoTimeTxt + String(clock.timestampToStr(currentTime)));

  Serial.print(pauseTxt);
  if (clock.getPaused())
    Serial.println(pauseStoppedTxt);
  else
    Serial.println(pauseRunningTxt);
};

void setDisplayTimeCmd(const String &cmd, const String &args) {
  Serial.print(changeFromTxt);
  Serial.print(dispTimeTxt);
  Serial.println(clock.getTimeStr(TimeType::Display));

  clock.setDisplayTimeSec(CommandHandler::extractTime(args));

  Serial.print(changeToTxt);
  Serial.print(dispTimeTxt);
  Serial.println(clock.getTimeStr(TimeType::Display));
};

void setTargetTimeCmd(const String &cmd, const String &args) {
  Serial.print(changeFromTxt);
  Serial.print(targTimeTxt);
  Serial.println(clock.getTimeStr(TimeType::Target));

  currentTime = CommandHandler::extractTime(args);
  currentTime %= clock.getDayLengthSec();

  clock.setTargetTimeSec(currentTime);

  Serial.print(changeToTxt);
  Serial.print(targTimeTxt);
  Serial.println(clock.getTimeStr(TimeType::Target));
};

void togglePauseCmd(const String &cmd, const String &args) {
  clock.setPaused(!clock.getPaused());
  Serial.print(pauseTxt);
  if (clock.getPaused())
    Serial.println(pauseStoppedTxt);
  else
    Serial.println(pauseRunningTxt);
};

void tickDisplay(const String &cmd, const String &args) {
  Serial.print(changeFromTxt);
  Serial.print(dispPosTxt);
  Serial.println(clock.getDisplayPosition());

  uLong newPos = clock.getDisplayPosition() + CommandHandler::toNumber(args);
  clock.setDisplayPosition(newPos);

  Serial.print(changeToTxt);
  Serial.print(dispPosTxt);
  Serial.println(clock.getDisplayPosition());
}

void tickDisplaySec(const String &cmd, const String &args) {
  Serial.print(changeFromTxt);
  Serial.print(dispTimeTxt);
  Serial.println(clock.getTimeStr(TimeType::Display));

  clock.setDisplayTimeSec(CommandHandler::extractTime(args) +
                          clock.getDisplayTimeSec());

  Serial.print(changeToTxt);
  Serial.print(dispTimeTxt);
  Serial.println(clock.getTimeStr(TimeType::Display));
}

void cmdTemplate(const String &cmd, const String &args) {
  CommandHandler::printHeader(cmd);
  CommandHandler::printHeader(cmd, true);
};
