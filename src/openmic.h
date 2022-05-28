#ifndef OPENMIC_H
#define OPENMIC_H

#include <QObject>

class OpenMic : public QObject
{
    Q_OBJECT
public:
    explicit OpenMic(QObject *parent = nullptr);

signals:

};

#endif // OPENMIC_H
