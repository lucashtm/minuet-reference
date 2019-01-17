#include <fftw3.h>
#include "AudioFile.h"
#include <cmath>
#include <QString>
#include <QByteArray>

class NoteRecognizer {

    double getFrequency(QByteArray* bytes);
    QString noteFromFreq(double freq);

    private:
    int keyFromFreq(double freq);
    double freqFromKey(int key);
}
