#include <QCoreApplication>
#include "com2mqtt.h"
#include <iostream>

com2mqtt::com2mqtt(QObject *parent, QStringList args) : QObject(parent)
{
  std::cout << "MQTT Host = " << args.at(1).toLocal8Bit().constData() << std::endl;
  std::cout << "MQTT Topic= " << args.at(2).toLocal8Bit().constData() << std::endl;
  std::cout << "SerialPort= " << args.at(3).toLocal8Bit().constData() << std::endl;
  std::cout << "Reg Text  = " << args.at(4).toLocal8Bit().constData() << std::endl;
  std::cout << "Rep Text  = " << args.at(5).toLocal8Bit().constData() << std::endl;
  strHost = args.at(1);   //引数１：MQTT接続先IP("xxx.xxx.xxx.xxx");
  strTopic = args.at(2);  //引数２：MQTTトピック("topic/");
  strSerial = args.at(3); //引数３：シリアルポート名("COMxx" or "/dev/ttyUSBx");
  strReg = args.at(4);    //引数４：対象データ条件("::.*:ct=([0-9|A-F]+):.*:tm=([0-9]+):hu=([0-9]+):at=([0-9]+)");
  strRep = args.at(5);    //引数５：対象データ変換後("\\1,\\2,\\3,\\4");
}

void com2mqtt::run()
{
  //QMQTT のオブジェクトを設定する
  publisher = new QMQTT::Client(QHostAddress(strHost), 1883, NULL);
  publisher->connectToHost();

  //シリアル通信のオブジェクトを設定する
  serial = new QSerialPort(NULL);
  connect(serial, &QSerialPort::readyRead, this, &com2mqtt::readData);

  serial->setPortName(strSerial);
  serial->setBaudRate(QSerialPort::Baud115200);
  serial->setDataBits(QSerialPort::Data8);
  serial->setParity(QSerialPort::NoParity);
  serial->setStopBits(QSerialPort::OneStop);
  serial->setFlowControl(QSerialPort::NoFlowControl);

  //バッファを初期化する
  readBuff.clear();

  //シリアル通信を開始する
  serial->open(QIODevice::ReadWrite);
}

void com2mqtt::readData()
{
  //受信したデータを取得する
  QByteArray data = serial->readAll();
  QString result;
  QRegExp reg;

  for (int i = 0; i < data.size(); i ++) {
    // 処理対象の一文字取り出す
      switch(data.at(i)) {
        case '\n':
          //行単位の処理
          //std::cout << readBuff.data() << std::endl;
          result = QString(readBuff);
          reg = QRegExp(strReg);
          //変換条件に一致する場合
          if (reg.exactMatch(result))
            {
              //変換してパブリッシュする
              result.replace(reg, strRep);
              std::cout << result.toStdString() << std::endl;
              publisher->publish(QMQTT::Message(0, strTopic, result.toUtf8()));
            }
          readBuff.clear();
          break;
          ;;
        case '\r':
          //無視
          break;
          ;;
        default:
          //通常の文字
          readBuff.append(data.at(i));
          break;
        }
  }
}
