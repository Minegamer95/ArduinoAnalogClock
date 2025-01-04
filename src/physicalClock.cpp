#include "PhysicalClock.h"

PhysicalClock::PhysicalClock(uLong displayPos, uint8_t diractionPinA, uint8_t diractionPinB, bool startDirection, uLong minTickDelayMs, uLong maxPos) {
    _displayPosition = displayPos;
    diractionA = diractionPinA;
    diractionB = diractionPinB;
    direction = startDirection;
    _minTickDelayInMs = minTickDelayMs;
    _maxPosition = maxPos;
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
    secSinceDayBegin %= dayLength;
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
    secSinceDayBegin %= dayLength;
    setTargetPosition(secSinceDayBegin / getTickDuration());
}

/// @brief Setzt die maximale Zeit die die Uhr anzeigen kann, z.B. 12h oder 24h
/// @param maxTimeSec 
void PhysicalClock::setMaxTime(uLong maxTimeSec){
    maxTime = maxTimeSec;
}

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
    return maxTime / _maxPosition;
}

void PhysicalClock::update(){
    if(direction)
    {
        digitalWrite(diractionA, HIGH);
    }
    else{
        digitalWrite(diractionB, HIGH);
    }
    delay(400);
    digitalWrite(diractionA, LOW);
    digitalWrite(diractionB, LOW);
    direction = !direction;
}

String PhysicalClock::getTimeStr(TimeType type) const {
    uLong timestamp = 0;
    switch (type){
        case TimeType::Display:
            timestamp = getDisplayTimeSec();
            break;
        case TimeType::Target:
            timestamp = getTargetTimeSec();
            break;
        default:
            return "-";
            break;
    };
    uLong h = timestamp / 60 / 60; // Stunden berechnen
    uLong m = (timestamp % 3600) / 60; // Minuten berechnen
    uLong s = timestamp % 60; // Sekunden berechnen

    // Formatierte Zeit als String zurückgeben
    return String(h < 10 ? "0" : "") + String(h) + ":" +
           String(m < 10 ? "0" : "") + String(m) + ":" +
           String(s < 10 ? "0" : "") + String(s);
}