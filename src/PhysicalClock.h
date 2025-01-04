#ifndef PHYSICALCLOCK_H
#define PHYSICALCLOCK_H

#include <Arduino.h>
#include <avr/io.h>

using uLong = unsigned long;

enum TimeType{
    Target,
    Display
};

class PhysicalClock {
private:
    /// @brief The amount of Seconds a entiry day has
    const uLong dayLength = 24UL * 60 * 60;

    uLong _displayPosition;
    uLong _targetPostion;
    uLong _minTickDelayInMs;
    uLong _lastTick; 
    uLong _maxPosition;
    /// @brief Die Maximale Zeit die in Sekunden angegeben werden kann, z.B. 24h, oder 12h
    uLong maxTime = 24UL * 60 * 60;
    /// @brief Die Aktuelle Richtiung die vom Motortreiber genutzt wird um die Uhr ticken zu lassen
    bool direction;
    // Gpio pin für die erste richtung des Motortreibers
    uint8_t diractionA;
    // Gpio pin für die andere richtung des Motortreibers
    uint8_t diractionB;

public:
    // Konstruktor
    PhysicalClock(uLong displayPos, uint8_t diractionPinA, uint8_t diractionPinB, bool startDirection = true, uLong minTickDelayMs = 800, uLong maxPos = 720);

    /// @brief Setzt die auf dem Ziffernblatt Angezeigte Zeit über aktuellen die Aktuelle Zeigerposition
    /// @param pos 
    void setDisplayPosition(uLong pos);

    /// @brief Setzt die auf dem Ziffernblatt Angezeigte Zeit über die Sekunden seit begin des Tages
    /// @param secSineDayBegin 
    void setDisplayTimeSec(uLong secSinceDayBegin);
    
    /// @brief Setzt die Anzugzeigende Zeit über die gewünschte Zeigerposition
    /// @param pos 
    void setTargetPosition(uLong pos);
    
    /// @brief Setzt die Anzuzeigende Zeit über die Sekunden seit begin des Tages
    /// @param secSinceDayBegin 
    void setTargetTimeSec(uLong secSinceDayBegin);

    /// @brief Setzt die maximale Zeit die die Uhr anzeigen kann, z.B. 12h oder 24h
    /// @param maxTimeSec 
    void setMaxTime(uLong maxTimeSec);

    /// @brief Fragt die aktuelle Zeigerposition ab
    /// @return 
    uLong getDisplayPosition() const;

    uLong getDisplayTimeSec() const;

    uLong getTargetTimeSec() const;

    /// @brief Fragt die gewünschte Zeigerposition ab
    /// @return 
    uLong getTargetPosition() const;

    /// @brief Berechnet die Länge eines Ticks in Sekunden
    /// @return 
    uLong getTickDuration() const;

    void update();

    String getTimeStr(TimeType type) const;

};

#endif