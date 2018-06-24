#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <string>
#include <QProgressDialog>
#include <QThreadPool>
#include <QMutex>
#include <QFutureWatcher>
#include <QMessageBox>
#include <fstream>
#include <ui_MainWindow.h>

namespace skelgenerator {


    class MainWindow : public QMainWindow {
        Q_OBJECT

        QMutex logFileEm;
        QString lastDir;
        std::ofstream logFile;
        QProgressDialog *procesingDialog;
        QFutureWatcher<void> watcher;
        QString const program = "NLMorphologyConverter.exe";
        std::map <std::string, std::string> extensions = {{"NeurolucidaASC",        ".asc"},
                                                          {"NeurolucidaDAT",        ".dat"},
                                                          {"NeurolucidaASCClassic", ".asc"},
                                                          {"NeurolucidaDATClassic", ".dat"},
        };
        int nsec;

    public:
        explicit MainWindow(QWidget *parent = 0);

        ~MainWindow();

    private:
        Ui::MainWindow *ui;

        void connectSignals();

        void message_dialog(std::string msg, QMessageBox::Icon type);

        void proccesMultiple(std::string srcFolder, std::string dstFolder, QString outFormat, float connectionTreshold);

        void showProgressDialog();

        void procesSkel(std::string api, std::vector <std::string> basals, std::string outputFile, QString outputFormat,
                        int connectionThreshold, std::string name);

    private slots:
        void open_one_dialog(QLineEdit & target);

        void save_dialog(QLineEdit &target, std::string filter);

        void open_many_dialog(QLineEdit &target);

        void run(std::string type, QString outputFormat, float connectionThreshold);

        void finished_task();

        void open_dir_dialog(QLineEdit &target);

        void showWarningDialogReaminingSegments(int sobrantes, int &newThreshold, std::string &neuronName);

        void showWarningDialogIncorrectConnections(int &newThreshold, std::string &neuronName);

    };

#endif // MAINWINDOW_H
}
