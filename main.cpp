#include <QCoreApplication>
#include <QTimer>
#include "com2mqtt.h"

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  com2mqtt runner(&a, a.arguments());

  QTimer::singleShot(0, &runner, SLOT(run()));

  return a.exec();
}
