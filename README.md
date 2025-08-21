// *****************************************************************

// Projekt: Intervalltimer für Fitnesstraining
// created 04.11.2023 by micronano (micronano@gmx-topmail.de)
// 
// Beschreibung: 3 Timer mit je 2 Intervallen einstellbar
//               Timereinstellungen werden intern gespeichert und 
//               gehen somit nicht verloren
// Intervallzeiten bis 245sek getrennt einstellbar 
// nach Ablauf von Intervall1 erfolgt 1 Piep, nach Pausezeit 2 Piepse.
// nach Ablauf von Intervall2 erfolgen 2 Piepser.
// Nach Ablauf des Timers erfolgen 5 Piepser.
// Zweck: zB 1min trainieren, 20s Pause, 1min trainieren, 20s Pause ... 
//        Pausezeiten reduzieren den Trainigstimer nicht
// Taste rot: Timer start/stop und Werte im Einstellungsmodus erhöhen
// Taste blau: Werte im Einstellungsmodus reduzieren
// Taste grün: Timer wählen / länger gedrückt: Timer einstellen
// Einstellung bleiben nach Stromunterbrechungen erhalten
// Stromverbrauch: ca 30mA
// verwendete Pins beim ESP12-F:
// int btn_rt          = 12; 
// int btn_bl          = 13;  
// int btn_set         = 14;  
// int buzz            = 15;
// https://wokwi.com/projects/380378934614223873
// *******************************************************************

