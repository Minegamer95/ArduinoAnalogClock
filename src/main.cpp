#include <Arduino.h>
#include "PhysicalClock.h"
#include "CommandHandler.h"

void statusCmd(const String& cmd, const String& args);
void setDisplayTimeCmd(const String& cmd, const String& args);
void cmdTemplate(const String& cmd, const String& args);


PhysicalClock clock = PhysicalClock(0, 10, 11);
CommandHandler commandHandler;
uLong currentTime = 0;
uLong lastUpdate = 0;

// Language Text
const char* dispTime = "Display Time: ";
const char* dispPos = "Display Tick: ";
const char* targTime = "Target Time: ";
const char* targPos = "Target Tick: ";

void setup() {
  // Configure Timer 1 for CTC mode
  TCCR1A = 0; // Set entire TCCR1A register to 0
  TCCR1B = 0; // Same for TCCR1B
  TCCR1B |= (1 << WGM12);

  // Set CS12, CS10 bits for prescaler 1024
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // Initialize counter value to 0
  TCNT1 = 0;
  // Set timer count for 1-second increments
  OCR1A = 15624; // = (16 * 10^6) / (1024) - 1
  // Enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei(); // Allow interrupts

  // Register Command
  commandHandler.addCommand("help", [](const String& cmd, const String& arg) {
    commandHandler.helpCommand(cmd, arg);
  }, "Displays the list of available commands.");
  commandHandler.addCommand("status", statusCmd, "Zeigt Status");
  commandHandler.addCommand("setDispTime", setDisplayTimeCmd, "Setzt die Display Time, setDispTime <hh:mm>/<hh:mm:ss>");

  Serial.begin(9600);
}

void loop() {
  clock.update();
  commandHandler.listen();

  if(millis() - lastUpdate > 10000){
    Serial.println(clock.getTimeStr(TimeType::Target));
    Serial.println(clock.getTargetTimeSec());
    lastUpdate = millis();
  }
}

ISR(TIMER1_COMPA_vect){
  currentTime++;
  clock.setTargetTimeSec(currentTime * 60);
}

void statusCmd(const String& cmd, const String& args){
  CommandHandler::printHeader(cmd);
  Serial.println(dispTime + clock.getTimeStr(TimeType::Display));
  Serial.println(dispPos + String(clock.getDisplayPosition()));
  Serial.println(targTime + clock.getTimeStr(TimeType::Target));
  Serial.println(targPos + String(clock.getTargetPosition()));

  CommandHandler::printHeader(cmd, true);
};

void setDisplayTimeCmd(const String& cmd, const String& args){
  CommandHandler::printHeader(cmd);
  Serial.print("Von ");
  Serial.print(dispTime);
  Serial.println(clock.getTimeStr(TimeType::Display));

  Serial.print("Args: ");
  Serial.print(args);
  Serial.print(", Count: ");
  Serial.print(CommandHandler::CountArgs(args, ':'));
  Serial.print(", Hour: ");
  Serial.print(CommandHandler::GetArg(args, 0, ':'));
  Serial.print(", Min: ");
  Serial.print(CommandHandler::GetArg(args, 1, ':'));

  Serial.println();
  clock.setDisplayTimeSec(CommandHandler::extractTime(args));

  Serial.print("Zu ");
  Serial.print(dispTime);
  Serial.println(clock.getTimeStr(TimeType::Display));
  CommandHandler::printHeader(cmd, true);
};

void cmdTemplate(const String& cmd, const String& args){
  CommandHandler::printHeader(cmd);
  CommandHandler::printHeader(cmd, true);
};

