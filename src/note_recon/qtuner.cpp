#include "qtuner.h"
#include "ui_qtuner.h"
#include <cmath>
#include <algorithm>
#include <fftw3.h>
#include <QDebug>
#include <QtMultimedia/QAudioInput>
#include <QtMultimedia/QAudioFormat>
#include <QTimer>
#include <QBuffer>
#include <qendian.h>
#include "AudioFile.h"

#define PREC 0.25
#define SR 44100

QString notes[12] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
QAudioInput* audio;
QAudioFormat format;
QByteArray data;
QBuffer buffer(&data);
AudioFile<double> audioFile;

fftw_complex* generateSignal(double* frequencies, int f_size, double sampleRate, int length);
double getMainFrequency(fftw_complex* signal);
fftw_complex* doubleArrayToFFTWComplex(std::vector<double> arr, int length);
QString noteFromFreq(double freq);
fftw_complex* bytesToComplex(QByteArray data, int length);

QTuner::QTuner(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QTuner)
{
    ui->setupUi(this);
}

QTuner::~QTuner()
{
    delete ui;
}


void QTuner::on_getFreq_clicked()
{
    audioFile.load ("/home/lucasmonteiro/kde/notes/C.wav");
//    audioFile.load ("/home/lucasmonteiro/kde/notes/D.wav");
//    audioFile.load ("/home/lucasmonteiro/kde/notes/E.wav");
//    audioFile.load ("/home/lucasmonteiro/kde/notes/F.wav");
//    format.setSampleRate(8000);
//    format.setChannelCount(1);
//    format.setSampleSize(8);
//    format.setCodec("audio/pcm");
//    format.setByteOrder(QAudioFormat::LittleEndian);
//    format.setSampleType(QAudioFormat::UnSignedInt);
//    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
//    if (!info.isFormatSupported(format)) {
//        qWarning() << "Default format not supported, trying to use the nearest.";
//        format = info.nearestFormat(format);
//    }
//    audio = new QAudioInput(format, this);
//    connect(audio, &QAudioInput::bytesReady, [this]() {
//        double freq = getMainFrequency(bytesToComplex(data, buffer.size()));
//        ui->labelFreq->setText(QString::number(freq));
//        ui->labelNote->setText(noteFromFreq(freq));
//    });
//    buffer.open(QIODevice::ReadWrite);
//    audio->start(&buffer);
    int nsamples = audioFile.getNumSamplesPerChannel();
    fftw_complex spectrum[nsamples];
    for(int i = 0; i < nsamples; i++){
        spectrum[i][REAL] = audioFile.samples[0][i];
        spectrum[i][IMAG] = 0;
    }
    double frequency = getMainFrequency(spectrum);
    ui->labelFreq->setText(QString::number(frequency));
    ui->labelNote->setText(noteFromFreq(frequency));
}

fftw_complex* doubleArrayToFFTWComplex(std::vector<double> arr, int length){
    fftw_complex out[length];
    for(int i = 0; i < length; i++){
        out[i][REAL] = arr[i];
        out[i][IMAG] = 0;
    }
    return out;
}

double getMainFrequency(fftw_complex* signal){
    double length = audioFile.getLengthInSeconds();
//    int nsamples = length*SR;
    int sr = audioFile.getSampleRate();
    double resolution = sr/(length*sr);
    int nsamples = audioFile.getNumSamplesPerChannel();
    fftw_complex out[nsamples];
    fftw_plan plan = fftw_plan_dft_1d(nsamples, signal, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);
    fftw_cleanup();
    double max = -1;
    int bin = -1;
    int nyq_limit = nsamples/2;
    for(int i = 0; i < nyq_limit; i++){
        double current = sqrt(pow(out[i][REAL], 2) + pow(out[i][IMAG], 2))*2;
        if(current > max){
            max = current;
            bin = i;
        }
    }
    return bin*resolution;
}

fftw_complex* generateSignal(double* frequencies, int f_size, double length, int nsamples){
    double t = 0;
    double step = length/nsamples;
    fftw_complex* in = fftw_alloc_complex(nsamples);
    for(int i = 0; i < nsamples; i++){
        double acc = 0;
        for(int j = 0; j < f_size; j++){
            acc += sin(frequencies[j]*2*M_PI*t);
        }
        in[i][REAL] = acc;
        in[i][IMAG] = 0;
        t += step;
    }
    return in;
}

int keyFromFreq(double freq){
    double a = 440.00;
    if(freq < 27.5 || freq > 4186.009)
        return -1;
    return round(12*log2(freq/a)+49);
}

double freqFromKey(int key){
    double a = 440.00;
    if(key < 1 || key > 88)
        return -1;
    return pow(2.0, (key-49.0)/12.0)*a;
}

QString noteFromFreq(double freq){
    int key = keyFromFreq(freq);
    if(key == -1) return "Given frequency is outside frequency range [27.5Hz, 4186.009Hz]";
    return notes[((keyFromFreq(freq)-1)+9)%12];
}

fftw_complex* bytesToComplex(QByteArray data, int length){
    const int channelBytes = format.sampleSize() / 8;
    int p = 0;
    fftw_complex* out = fftw_alloc_complex(length);
    for(int i = 0; i < length; i++){
        qint32 value;
        if (format.sampleSize() == 8 && format.sampleType() == QAudioFormat::UnSignedInt) {
            value = *reinterpret_cast<const quint8*>(data.data()[p]);
        } else if (format.sampleSize() == 8 && format.sampleType() == QAudioFormat::SignedInt) {
            value = qAbs(*reinterpret_cast<const qint8*>(data.data()[p]));
        } else if (format.sampleSize() == 16 && format.sampleType() == QAudioFormat::UnSignedInt) {
            if (format.byteOrder() == QAudioFormat::LittleEndian)
                value = qFromLittleEndian<quint16>(data.data()[p]);
            else
                value = qFromBigEndian<quint16>(data.data()[p]);
        } else if (format.sampleSize() == 16 && format.sampleType() == QAudioFormat::SignedInt) {
            if (format.byteOrder() == QAudioFormat::LittleEndian)
                value = qAbs(qFromLittleEndian<qint16>(data.data()[p]));
            else
                value = qAbs(qFromBigEndian<qint16>(data.data()[p]));
        } else if (format.sampleSize() == 32 && format.sampleType() == QAudioFormat::UnSignedInt) {
            if (format.byteOrder() == QAudioFormat::LittleEndian)
                value = qFromLittleEndian<quint32>(data.data()[p]);
            else
                value = qFromBigEndian<quint32>(data.data()[p]);
        } else if (format.sampleSize() == 32 && format.sampleType() == QAudioFormat::SignedInt) {
            if (format.byteOrder() == QAudioFormat::LittleEndian)
                value = qAbs(qFromLittleEndian<qint32>(data.data()[p]));
            else
                value = qAbs(qFromBigEndian<qint32>(data.data()[p]));
        } else if (format.sampleSize() == 32 && format.sampleType() == QAudioFormat::Float) {
            value = qAbs(*reinterpret_cast<const float*>(data.data()[p]) * 0x7fffffff); // assumes 0-1.0
        }
        p += channelBytes;
        out[i][REAL] = value;
        out[i][IMAG] = 0;
    }
    return out;
}

