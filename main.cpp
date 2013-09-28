#include "dialog.hpp"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
#if QT_VERSION < 0x050000
	QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
#endif

    QApplication a(argc, argv);
    Dialog w;
    w.show();

    return a.exec();
}
