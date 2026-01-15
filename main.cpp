#include <QApplication>
#include <QTextCodec>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);


    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    QString filePath = QCoreApplication::applicationDirPath() + "/contacts.txt";
    ContactBook book(filePath);
    book.loadFromFile();

    MainWindow w;
    w.show();

    return a.exec();
}
