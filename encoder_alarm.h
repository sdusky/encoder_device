#ifndef ENCODER_ALARM_H
#define ENCODER_ALARM_H

#include <QObject>

class encoder_alarm : public QObject
{
    Q_OBJECT
public:
    explicit encoder_alarm(QObject *parent = 0);

signals:

public slots:

};

#endif // ENCODER_ALARM_H
