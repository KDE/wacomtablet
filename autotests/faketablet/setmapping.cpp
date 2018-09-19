#include <iostream>

#include "kded/xinputadaptor.h"
#include "kded/xsetwacomadaptor.h"
#include "common/screenspace.h"
#include "common/property.h"

#include <QApplication>
#include <QObject>
#include <QTimer>
#include <QThread>

using namespace Wacom;

class Task : public QObject
{
    Q_OBJECT
public:
    Task(QObject *parent = 0) : QObject(parent) {}

public slots:
    void run()
    {
        QString device = QString::fromLatin1("Wacom Bamboo One S Pen stylus");

        Wacom::XinputAdaptor xi(device);
        XsetwacomAdaptor xsetwacom(device);

        auto defmode = xsetwacom.getProperty(Property::Mode);
        //auto defmapping = xi.getProperty(Property::ScreenSpace);

        Wacom::ScreenSpace space(QLatin1String("speedx0.5x0.5"));

        xsetwacom.setProperty(Property::Mode, QString::fromLatin1("relative"));
        xi.setProperty(Property::ScreenSpace, space.toString());
        std::cout << "Mapping set\n";

        QThread::sleep(10);

        xsetwacom.setProperty(Property::Mode, defmode);

        std::cout << "Mapping reset\n";

        emit finished();
    }

signals:
    void finished();
};

int main( int argc, char **argv )
{
    QApplication a(argc, argv);

    Task *task = new Task(&a);
    QObject::connect(task, SIGNAL(finished()), &a, SLOT(quit()));
    QTimer::singleShot(0, task, SLOT(run()));

    return a.exec();
}

#include "setmapping.moc"
