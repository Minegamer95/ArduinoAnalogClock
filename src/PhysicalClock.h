#ifndef PHYSICALCLOCK_H
#define PHYSICALCLOCK_H

#include <Arduino.h>
#include <avr/io.h>

using uLong = unsigned long;

enum TimeType { Target, Display };

class PhysicalClock {
private:
  /// @brief The amount of Seconds a entiry day has
  const uLong _dayLength = 24UL * 60 * 60;

  uLong _displayPosition;
  uLong _targetPostion;
  uLong _minTickDelayInMs;
  uLong _tickPinTurnOfDelay;
  uLong _lastTick;
  uLong _maxPosition;
  uLong _forwardDiffSecDelay = 10;
  /// @brief Die Maximale Zeit die in Sekunden angegeben werden kann, z.B. 24h,
  /// oder 12h
  uLong _maxTime = 24UL * 60 * 60;
  /// @brief Die Aktuelle Richtiung die vom Motortreiber genutzt wird um die Uhr
  /// ticken zu lassen
  bool direction;
  /// @brief The Status if the clock is paused or is not
  bool _paused;
  // Gpio pin für die erste richtung des Motortreibers
  uint8_t diractionA;
  // Gpio pin für die andere richtung des Motortreibers
  uint8_t diractionB;

public:
  // Konstruktor
  PhysicalClock(uLong displayPos, uint8_t diractionPinA, uint8_t diractionPinB,
                bool startDirection, uLong minTickDelayMs, uLong maxPos,
                bool paused) {
    _displayPosition = displayPos;
    diractionA = diractionPinA;
    diractionB = diractionPinB;
    direction = startDirection;
    _minTickDelayInMs = minTickDelayMs;
    _maxPosition = maxPos;
    _paused = paused;
    _tickPinTurnOfDelay = 400;
    _lastTick = 0;
    pinMode(diractionA, OUTPUT);
    pinMode(diractionB, OUTPUT);
  }

  /// @brief Setzt die auf dem Ziffernblatt Angezeigte Zeit über aktuellen die
  /// Aktuelle Zeigerposition
  /// @param pos
  void setDisplayPosition(uLong pos) { _displayPosition = pos % _maxPosition; }

  /// @brief Setzt die auf dem Ziffernblatt Angezeigte Zeit über die Sekunden
  /// seit begin des Tages
  /// @param secSineDayBegin
  void setDisplayTimeSec(uLong secSinceDayBegin) {
    secSinceDayBegin %= _dayLength;
    setDisplayPosition(secSinceDayBegin / getTickDuration());
  }

  /// @brief Setzt die Anzugzeigende Zeit über die gewünschte Zeigerposition
  /// @param pos
  void setTargetPosition(uLong pos) { _targetPostion = pos % _maxPosition; }

  /// @brief Setzt die Anzuzeigende Zeit über die Sekunden seit begin des Tages
  /// @param secSinceDayBegin
  void setTargetTimeSec(uLong secSinceDayBegin) {
    secSinceDayBegin %= _dayLength;
    setTargetPosition(secSinceDayBegin / getTickDuration());
  }

  /// @brief Setzt die maximale Zeit die die Uhr anzeigen kann, z.B. 12h oder
  /// 24h
  /// @param maxTimeSec
  void setMaxTime(uLong maxTimeSec) { _maxTime = maxTimeSec; }

  /// @brief Setzt ob die Uhr pausiert sein soll oder nicht
  /// @param pause
  void setPaused(bool pause) { _paused = pause; };

  /// @brief Fragt die aktuelle Zeigerposition ab
  /// @return
  uLong getDisplayPosition() const { return _displayPosition; }

  uLong getDisplayTimeSec() const {
    return _displayPosition * getTickDuration();
  }

  uLong getTargetTimeSec() const { return _targetPostion * getTickDuration(); }

  /// @brief Fragt die gewünschte Zeigerposition ab
  /// @return
  uLong getTargetPosition() const { return _targetPostion; }

  /// @brief Berechnet die Länge eines Ticks in Sekunden
  /// @return
  uLong getTickDuration() const { return _maxTime / _maxPosition; }

  uLong getDayLengthSec() const { return _dayLength; }

  uLong getForwardTickDiff() {
    return (_maxPosition + getTargetPosition() - getDisplayPosition()) %
           _maxPosition;
  }

  uLong getBackwardTickDiff() {
    return (_maxPosition + getDisplayPosition() - getTargetPosition()) %
           _maxPosition;
  }

  uLong getForwardTickDiffSec() {
    // Berechne die Gesamtzeit in Millisekunden
    uLong totalDelayInMs =
        getForwardTickDiff() *
        (_minTickDelayInMs + _tickPinTurnOfDelay + _forwardDiffSecDelay);
    // Teile die Gesamtzeit durch 1000, um die Zeit in Sekunden zu erhalten
    return totalDelayInMs / 1000;
  }

  uLong getBackwardTickDiffSec() {
    return getBackwardTickDiff() * getTickDuration();
  }

  bool getPaused() const { return _paused; }

  void update() {
    if (_paused)
      return;
    else if (getDisplayPosition() == getTargetPosition())
      return;
    // Wenn es mehr sinn mach den Zeiger zu lassen wo er ist sollte es auch so
    // sein
    else if (getForwardTickDiffSec() > getBackwardTickDiffSec() ||
             getBackwardTickDiffSec() < 5 * 60)
      return;
    else if (millis() - _lastTick < _minTickDelayInMs)
      return;

    if (direction) {
      digitalWrite(diractionA, HIGH);
    } else {
      digitalWrite(diractionB, HIGH);
    }
    delay(_tickPinTurnOfDelay);
    digitalWrite(diractionA, LOW);
    digitalWrite(diractionB, LOW);
    direction = !direction;

    // Internen Zähler der Postion erhöhen, der setter kümmert sich automatisch
    // um die begreznung des Bereiches
    setDisplayPosition(getDisplayPosition() + 1);
    _lastTick = millis();
  };

  String timestampToStr(uLong timestamp) const {
    uLong h = timestamp / 60 / 60;     // Stunden berechnen
    uLong m = (timestamp % 3600) / 60; // Minuten berechnen
    uLong s = timestamp % 60;          // Sekunden berechnen

    // Formatierte Zeit als String zurückgeben
    return String(h < 10 ? "0" : "") + String(h) + ":" +
           String(m < 10 ? "0" : "") + String(m) + ":" +
           String(s < 10 ? "0" : "") + String(s);
  };

  String getTimeStr(TimeType type) const {
    switch (type) {
    case TimeType::Display:
      return timestampToStr(getDisplayTimeSec());
    case TimeType::Target:
      return timestampToStr(getTargetTimeSec());
      break;
    default:
      return "-";
      break;
    };
  }
};

#endif