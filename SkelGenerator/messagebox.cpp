#include "messagebox.h"
#include <QAbstractButton>

namespace skelgenerator {

    MessageBox::MessageBox() {
        this->timeout = 0;
        this->autoClose = false;
    }

    MessageBox::MessageBox(int timeout, bool autoClose) {
        this->timeout = timeout;
        this->autoClose = autoClose;
    }

    void MessageBox::showEvent(QShowEvent *event) {
        if (autoClose) {
            this->startTimer(1000);
        }
    }

    void MessageBox::timerEvent(QTimerEvent *event) {
        timeout--;
        std::string msg =
                this->msg + "\n\n This dialog will be closed and those segments will be automatically ignored in " +
                std::to_string(timeout) + " seconds.";
        this->setInformativeText(QString::fromStdString(msg));
        this->update();
        this->repaint();
        if (timeout <= 0) {
            this->done(0);

        }

    }

    int MessageBox::exec() {
        this->msg = this->informativeText().toStdString();
        if (autoClose) {
            this->setInformativeText(QString::fromStdString(
                    msg + "\n\n This dialog will be closed and those segments will be automatically ignored in " +
                    std::to_string(timeout) + " seconds."));
        }
        return QMessageBox::exec();

    }
}
