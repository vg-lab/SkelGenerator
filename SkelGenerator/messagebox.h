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
        MessageBox(int timeout, bool autoClose,QWidget* parent = 0);

        void showEvent(QShowEvent *event) override;

        void timerEvent(QTimerEvent *event) override;

        int exec() override;

        explicit MessageBox(QWidget* parent = 0);
    };
}

#endif // MESSAGEBOX_H
