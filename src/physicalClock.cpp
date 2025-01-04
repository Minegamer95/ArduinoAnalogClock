#include "PhysicalClock.h"

PhysicalClock::PhysicalClock(uLong displayPos, uint8_t diractionPinA, uint8_t diractionPinB, bool startDirection, uLong minTickDelayMs, uLong maxPos, bool paused) {
    _displayPosition = displayPos;
    diractionA = diractionPinA;
    diractionB = diractionPinB;
    direction = startDirection;
    _minTickDelayInMs = minTickDelayMs;
    _maxPosition = maxPos;
    _paused = paused;
    _tickPinTurnOfDelay = 400;
    pinMode(diractionA, OUTPUT);
    pinMode(diractionB, OUTPUT);
}

/// @brief Setzt die auf dem Ziffernblatt Angezeigte Zeit über aktuellen die Aktuelle Zeigerposition
    /// @param pos 
void PhysicalClock::setDisplayPosition(uLong pos) {
    _displayPosition = pos % _maxPosition;
}

/// @brief Setzt die auf dem Ziffernblatt Angezeigte Zeit über die Sekunden seit begin des Tages
/// @param secSineDayBegin 
void PhysicalClock::setDisplayTimeSec(uLong secSinceDayBegin){
    secSinceDayBegin %= _dayLength;
    setDisplayPosition(secSinceDayBegin / getTickDuration());
}

/// @brief Setzt die Anzugzeigende Zeit über die gewünschte Zeigerposition
/// @param pos 
void PhysicalClock::setTargetPosition(uLong pos){
    _targetPostion = pos % _maxPosition;
}

/// @brief Setzt die Anzuzeigende Zeit über die Sekunden seit begin des Tages
/// @param secSinceDayBegin 
void PhysicalClock::setTargetTimeSec(uLong secSinceDayBegin){
    secSinceDayBegin %= _dayLength;
    setTargetPosition(secSinceDayBegin / getTickDuration());
}

/// @brief Setzt die maximale Zeit die die Uhr anzeigen kann, z.B. 12h oder 24h
/// @param maxTimeSec 
void PhysicalClock::setMaxTime(uLong maxTimeSec){
    _maxTime = maxTimeSec;
}

/// @brief Setzt ob die Uhr pausiert sein soll oder nicht
/// @param pause 
void PhysicalClock::setPaused(bool pause){
    _paused = pause;
};

/// @brief Fragt die aktuelle Zeigerposition ab
/// @return 
uLong PhysicalClock::getDisplayPosition() const {
    return _displayPosition;
}

uLong PhysicalClock::getDisplayTimeSec() const {
    return _displayPosition * getTickDuration();
}

uLong PhysicalClock::getTargetTimeSec() const {
    return _targetPostion * getTickDuration();
}

/// @brief Fragt die gewünschte Zeigerposition ab
/// @return 
uLong PhysicalClock::getTargetPosition() const {
    return _targetPostion;
}

/// @brief Berechnet die Länge eines Ticks in Sekunden
/// @return 
uLong PhysicalClock::getTickDuration() const {
    return _maxTime / _maxPosition;
}

uLong PhysicalClock::getDayLengthSec() const {
    return _dayLength;
}

bool PhysicalClock::getPaused() const {
    return _paused;
}

void PhysicalClock::update(){
    if (_paused)
        return;
    else if (getDisplayPosition() == getTargetPosition())
        return;
    else if (millis() - _lastTick < _minTickDelayInMs)
        return;

    if(direction)
    {
        digitalWrite(diractionA, HIGH);
    }
    else{
        digitalWrite(diractionB, HIGH);
    }
    delay(_tickPinTurnOfDelay);
    digitalWrite(diractionA, LOW);
    digitalWrite(diractionB, LOW);
    direction = !direction;

    // Internen Zähler der Postion erhöhen, der setter kümmert sich automatisch um die begreznung des Bereiches
    setDisplayPosition(getDisplayPosition()+1);
    _lastTick = millis();
}

String PhysicalClock::timestampToStr(uLong timestamp) const{
    uLong h = timestamp / 60 / 60; // Stunden berechnen
    uLong m = (timestamp % 3600) / 60; // Minuten berechnen
    uLong s = timestamp % 60; // Sekunden berechnen

    // Formatierte Zeit als String zurückgeben
    return String(h < 10 ? "0" : "") + String(h) + ":" +
           String(m < 10 ? "0" : "") + String(m) + ":" +
           String(s < 10 ? "0" : "") + String(s);
};

String PhysicalClock::getTimeStr(TimeType type) const {
    switch (type){
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