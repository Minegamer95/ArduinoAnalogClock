#include <Arduino.h>
#include <avr/io.h>

class PhysicalClock {
private:
    /// @brief The amount of Seconds a entiry day has
    const int dayLength = 24*60*60;

    int realPosition;
    int targetPostion;
    const int maxPosition = 720;
    /// @brief Die Maximale Zeit die in Sekunden angegeben werden kann, z.B. 24h, oder 12h
    int maxTime = 24*60*60;
    /// @brief Die Aktuelle Richtiung die vom Motortreiber genutzt wird um die Uhr ticken zu lassen
    bool direction;
    // Gpio pin für die erste richtung des Motortreibers
    uint8_t diractionA;
    // Gpio pin für die andere richtung des Motortreibers
    uint8_t diractionB;

public:
    // Konstruktor
    PhysicalClock(int realPos, uint8_t diractionPinA, uint8_t diractionPinB, bool startDirection = true) {
        realPosition = realPos;
        diractionA = diractionPinA;
        diractionB = diractionPinB;
        direction = startDirection;
        pinMode(diractionA, OUTPUT);
        pinMode(diractionB, OUTPUT);
    }

    /// @brief Setzt die auf dem Ziffernblatt Angezeigte Zeit über aktuellen die Aktuelle Zeigerposition
    /// @param pos 
    void setRealPosition(int pos) {
        realPosition = pos % maxPosition;
    }

    /// @brief Setzt die auf dem Ziffernblatt Angezeigte Zeit über die Sekunden seit begin des Tages
    /// @param secSineDayBegin 
    void setDisplayTime(int secSinceDayBegin){
        secSinceDayBegin %= dayLength;
        setRealPosition(secSinceDayBegin / getTickDuration());
    }

    /// @brief Setzt die Anzugzeigende Zeit über die gewünschte Zeigerposition
    /// @param pos 
    void setTargetPosition(int pos){
        targetPostion = pos % maxPosition;
    }

    /// @brief Setzt die Anzuzeigende Zeit über die Sekunden seit begin des Tages
    /// @param secSinceDayBegin 
    void setTargetTime(int secSinceDayBegin){
        secSinceDayBegin %= dayLength;
        setTargetPosition(secSinceDayBegin / getTickDuration());
    }

    /// @brief Setzt die maximale Zeit die die Uhr anzeigen kann, z.B. 12h oder 24h
    /// @param maxTimeSec 
    void setMaxTime(int maxTimeSec){
        maxTime = maxTimeSec;
    }

    /// @brief Fragt die aktuelle Zeigerposition ab
    /// @return 
    int getRealPosition() const {
        return realPosition;
    }

    /// @brief Fragt die gewünschte Zeigerposition ab
    /// @return 
    int getTargetPosition() const {
        return targetPostion;
    }

    /// @brief Berechnet die Länge eines Ticks in Sekunden
    /// @return 
    int getTickDuration() const {
        return maxTime / maxPosition;
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

    String formatTimeFromTimestamp(int timestamp) {
        int h = timestamp / 60; // Stunden berechnen
        int m = timestamp % 60; // Minuten berechnen

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