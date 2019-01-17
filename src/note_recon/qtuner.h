#ifndef QTUNER_H
#define QTUNER_H

#include <QMainWindow>
#include <QAudio>

namespace Ui {
class QTuner;
}

class QTuner : public QMainWindow
{
    Q_OBJECT

public:
    QByteArray data;
    explicit QTuner(QWidget *parent = nullptr);
    ~QTuner();

private slots:
    void on_getFreq_clicked();

private:
    Ui::QTuner *ui;
};

#endif // QTUNER_H
