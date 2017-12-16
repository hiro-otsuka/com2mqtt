#ifndef COM2MQTT_H
#define COM2MQTT_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <qmqtt.h>

class com2mqtt : public QObject
{
  Q_OBJECT
public:
  explicit com2mqtt(QObject *parent, QStringList args);

signals:

public slots:
  void run();
  void readData();

private:
  QSerialPort *serial;
  QByteArray readBuff;
  QMQTT::Client* publisher;

  QString strHost;
  QString strTopic;
  QString strSerial;
  QString strReg;
  QString strRep;
};

#endif // COM2MQTT_H
