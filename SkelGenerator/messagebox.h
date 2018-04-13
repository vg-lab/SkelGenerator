#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QShowEvent>
#include <QMessageBox>
namespace skelgenerator {


    class MessageBox : public QMessageBox {
        int timeout;
        std::string msg;
        bool autoClose;
    public:
        MessageBox(int timeout, bool autoClose);

        void showEvent(QShowEvent *event);

        void timerEvent(QTimerEvent *event);

        int exec();

        MessageBox();
    };
}

#endif // MESSAGEBOX_H
