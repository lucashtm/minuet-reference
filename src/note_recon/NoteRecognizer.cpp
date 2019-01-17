#include "NoteRecognizer.h"

namespace {
    
    const int REAL = 0;
    const int IMAG = 1;
    AudioFile<double> audioFile;

    QString notes[] = {
        QStringLiteral("C"),
        QStringLiteral("C#"),
        QStringLiteral("D"),
        QStringLiteral("D#"),
        QStringLiteral("E"),
        QStringLiteral("F"),
        QStringLiteral("F#"),
        QStringLiteral("G"),
        QStringLiteral("G#"),
        QStringLiteral("A"),
        QStringLiteral("A#"),
        QStringLiteral("B")
    };
}

double getFrequency(fftw_complex* samples, int nsamples, double length, int sampleRate){
  double resolution = sampleRate/(length*sampleRate);
  fftw_complex out[nsamples];
  fftw_plan plan = fftw_plan_dft_1d(nsamples, samples, out, FFTW_FORWARD, FFTW_ESTIMATE);
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

fftw_complex* getAudioSamplesFromFile(QString filename){
  audioFile.load (filename);
  int nsamples = audioFile.getNumSamplesPerChannel();
  fftw_complex spectrum[nsamples];
  for(int i = 0; i < nsamples; i++){
      spectrum[i][REAL] = audioFile.samples[0][i];
      spectrum[i][IMAG] = 0;
  }
  return spectrum;
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
