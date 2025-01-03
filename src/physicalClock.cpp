#include <Arduino.h>
#include <avr/io.h>
using uLong = uLong;

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
    PhysicalClock(uLong displayPos, uint8_t diractionPinA, uint8_t diractionPinB, bool startDirection = true, uLong minTickDelayMs = 800, uLong maxPos = 720) {
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
    void setDisplayPosition(uLong pos) {
        _displayPosition = pos % _maxPosition;
    }

    /// @brief Setzt die auf dem Ziffernblatt Angezeigte Zeit über die Sekunden seit begin des Tages
    /// @param secSineDayBegin 
    void setDisplayTimeSec(uLong secSinceDayBegin){
        secSinceDayBegin %= dayLength;
        setDisplayPosition(secSinceDayBegin / getTickDuration());
    }
    
    /// @brief Setzt die Anzugzeigende Zeit über die gewünschte Zeigerposition
    /// @param pos 
    void setTargetPosition(uLong pos){
        _targetPostion = pos % _maxPosition;
    }
    
    /// @brief Setzt die Anzuzeigende Zeit über die Sekunden seit begin des Tages
    /// @param secSinceDayBegin 
    void setTargetTimeSec(uLong secSinceDayBegin){
        secSinceDayBegin %= dayLength;
        setTargetPosition(secSinceDayBegin / getTickDuration());
    }

    /// @brief Setzt die maximale Zeit die die Uhr anzeigen kann, z.B. 12h oder 24h
    /// @param maxTimeSec 
    void setMaxTime(uLong maxTimeSec){
        maxTime = maxTimeSec;
    }

    /// @brief Fragt die aktuelle Zeigerposition ab
    /// @return 
    uLong getRealPosition() const {
        return _displayPosition;
    }

    uLong getDisplayTimeSec() const {
        return _displayPosition * getTickDuration();
    }

    uLong getTargetTimeSec() const {
        return _targetPostion * getTickDuration();
    }

    /// @brief Fragt die gewünschte Zeigerposition ab
    /// @return 
    uLong getTargetPosition() const {
        return _targetPostion;
    }

    /// @brief Berechnet die Länge eines Ticks in Sekunden
    /// @return 
    uLong getTickDuration() const {
        return maxTime / _maxPosition;
    }

    void update(){
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

    String getTimeStr(TimeType type) {
        uLong timestamp = 0;
        switch (type){
            case TimeType::Display:
                timestamp = getDisplayTimeSec();
                break;
            case TimeType::Target:
                timestamp = getTargetTimeSec();
            default:
                break;
        };

        uLong h = timestamp / 60 / 60; // Stunden berechnen
        uLong m = timestamp % 3600 / 60; // Minuten berechnen

        String formattedTime = "";

        // Stunden formatieren
        if (h < 10) {
            formattedTime += "0"; // Füge eine führende Null hinzu, wenn h < 10
        }
        formattedTime += String(h) + ":";

        // Minuten formatieren
        if (m < 10) {
            formattedTime += "0"; // Füge eine führende Null hinzu, wenn m < 10
        }
        formattedTime += String(m);

        return formattedTime;
}

};