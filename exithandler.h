#ifndef EXITHANDLER_H
#define EXITHANDLER_H

#include <QCoreApplication>

class ExitHandler {
private:
    static QCoreApplication *_app;
    static int _counter;
public:
    static void setup(QCoreApplication *app);
    static void run(int s);
};

#endif // EXITHANDLER_H
