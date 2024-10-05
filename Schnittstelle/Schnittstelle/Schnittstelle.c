#include <windows.h>
#include <stdio.h>

int main() {
    HANDLE hSerial;
    DCB dcbSerialParams = { 0 };
    COMMTIMEOUTS timeouts = { 0 };
    FILE* file;

    // Öffnen der Logdatei zum Schreiben entweder neu erstellen oder als Anhang öffnen und weiter beschreiben
    if (fopen_s(&file, "C:\\Users\\Josef\\Desktop\\communication_log.txt", "a") != 0) {
        perror("Error opening file");
        return 1;
    }

    // COM-Port öffnen mit angepassten COM-Port
    hSerial = CreateFileA(
        "\\\\.\\COM5", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hSerial == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Error opening COM port\n");
        fclose(file);
        return 1;
    }

    // Serielle Schnittstellenparameter konfigurieren
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    GetCommState(hSerial, &dcbSerialParams);
    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    SetCommState(hSerial, &dcbSerialParams);

    // Zeitüberschreitungen einstellen
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    SetCommTimeouts(hSerial, &timeouts);

    // Variablenwert setzen durch Senden von Nachrichten
    char input[50];
    DWORD bytes_written, bytes_read;
    char buffer[50];  // Buffer für empfangene Nachrichten

    while (1) {
        printf("Geben Sie eine Berechnung ein (z.B. '3*3' oder '5/2'): ");
        fgets(input, sizeof(input), stdin);

        // Nachricht senden
        WriteFile(hSerial, input, strlen(input), &bytes_written, NULL);

        // Antwort lesen
        if (ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytes_read, NULL)) {
            buffer[bytes_read] = '\0';  // Null-terminiere den Empfang

            // Zeige das Ergebnis an
            printf("Ergebnis vom Arduino: %s\n", buffer);

            // Eingaben und Antworten in die Datei schreiben
            fprintf(file, "Eingabe: %s", input);  // Eingabe
            fprintf(file, "Antwort: %s\n", buffer);  // Antwort
            fflush(file);  // Sicherstellen, dass Daten sofort geschrieben werden
        }
        else {
            fprintf(stderr, "Fehler beim Lesen vom Arduino\n");
        }
    }

    CloseHandle(hSerial); // Serielle Schnittstelle schließen
    fclose(file); // Datei schließen
    return 0;
}
