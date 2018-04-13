#include "MainWindow.h"
#include "messagebox.h"
#include "ui_MainWindow.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMetaObject>
#include <QtConcurrent/QtConcurrent>
#include "imarisSkel.cpp"
#include <cstring>

namespace skelgenerator {
    MainWindow::MainWindow(QWidget *parent) :
            QMainWindow(parent),
            ui(new Ui::MainWindow) {
        ui->setupUi(this);
        connectSignals();

    }

    void MainWindow::connectSignals() {

        // One skel
        connect(&watcher, SIGNAL(finished()), this, SLOT(finished_task()));
        connect(ui->one_skel_api_button, &QPushButton::released,
                [this]() { open_one_dialog(*(ui->one_api_vrml_input)); });
        connect(ui->one_skel_basal_button, &QPushButton::released,
                [this]() { open_many_dialog(*(ui->one_basales_vrml_input)); });
        connect(ui->one_skel_swc_button, &QPushButton::released,
                [this]() { save_dialog(*(ui->one_skel_swc_input), "*"); });
        connect(ui->one_skel_run_button, &QPushButton::released, [this]() {
            run("one", ui->outputFormatComboBox->currentText(), ui->connectionThresholdSpinBox->value());
        });
        connect(ui->many_skel_button, &QPushButton::released, [this]() { open_dir_dialog(*(ui->many_skel_input)); });
        connect(ui->many_swc_button, &QPushButton::released, [this]() { open_dir_dialog(*(ui->many_swc_input)); });
        connect(ui->many_vrml_run_button, &QPushButton::released, [this]() {
            run("many", ui->outputFormatComboBox->currentText(), ui->connectionThresholdSpinBox->value());
        });
    }

    void MainWindow::message_dialog(std::string msg, QMessageBox::Icon type) {
        QString QMsg = QString::fromStdString(msg);
        QMessageBox msgBox;
        msgBox.setText(QMsg);
        msgBox.setIcon(type);
        msgBox.exec();
    }

    void MainWindow::open_dir_dialog(QLineEdit &target) {
        QDir dir = QFileDialog::getExistingDirectory(this, "Open Folder", lastDir);
        QString path = dir.absolutePath();
        target.setText(path);
        lastDir = path;

    }

    void MainWindow::open_one_dialog(QLineEdit &target) {
        QString file = QFileDialog::getOpenFileName(this, "Open Image", lastDir, tr("Files (*.vrml *.wrl)"));
        target.setText(file);
        lastDir = QFileInfo(file).absoluteDir().absolutePath();

    }


    void MainWindow::save_dialog(QLineEdit &target, std::string filter) {
        QString QFilter = QString::fromStdString(filter);
        QString file = QFileDialog::getSaveFileName(this, "Save File", lastDir, QFilter);
        target.setText(file);
        lastDir = QFileInfo(file).absoluteDir().absolutePath();
    }

    void MainWindow::open_many_dialog(QLineEdit &target) {
        QStringList files = QFileDialog::getOpenFileNames(this, "Open Image", lastDir, tr("Files (*.vrml *.wrl)"));
        QString text;
        int size = files.size();
        for (int i = 0; i < size - 1; i++) {
            text.append(files.at(i) + ";");
        }
        text.append(files[size - 1]);
        target.setText(text);
        lastDir = QFileInfo(files[0]).absoluteDir().absolutePath();
    }

    void MainWindow::showProgressDialog() {
        procesingDialog = new QProgressDialog("Operation in progress", "Cancel", 0, 0);
        procesingDialog->setValue(0);
        procesingDialog->setCancelButton(0);
        procesingDialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
        procesingDialog->exec();
    }


    void MainWindow::showWarningDialog(int sobrantes, int &newThreshold, std::string &neuronName) {
        MessageBox *msgBox;

        std::string msg;
        if (neuronName != "") {
            msgBox = new MessageBox(30, true);
            msg = "The neuron \"" + neuronName + "\" has " + std::to_string(sobrantes) +
                  " segments that have not been connected and therefore will be ignored.";
        } else {
            msgBox = new MessageBox();
            msg = "This neuron has " + std::to_string(sobrantes) +
                  " segments that have not been connected and therefore will be ignored.";
        }
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
        msgBox->setInformativeText(
                "Do you want to process the neuron again changing the \"Connection Threshold\" or ignore the missing segments?");
        msgBox->setText(QString::fromStdString(msg));
        QPushButton *changeButton = msgBox->addButton(tr(" Change Threshold "), QMessageBox::NoRole);
        QPushButton *ignoreButton = msgBox->addButton(tr("Ignore"), QMessageBox::NoRole);
        msgBox->setDefaultButton(changeButton);
        msgBox->exec();

        if (msgBox->clickedButton() == changeButton) {
            QInputDialog inputDialog;
            inputDialog.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
            inputDialog.setLabelText("New Connection Threshold");
            inputDialog.setTextValue("Insert new value");
            inputDialog.setInputMode(QInputDialog::IntInput);
            inputDialog.setIntRange(0, 40);
            inputDialog.setIntStep(1);
            inputDialog.setIntValue(newThreshold);
            inputDialog.exec();
            newThreshold = inputDialog.intValue();


        } else {
            newThreshold = -1;
            logFileEm.lock();
            if (msgBox->clickedButton() != ignoreButton) {
                logFile << "- The neuron \"" << neuronName << "\" has " << sobrantes
                        << " segments that have been automatically ignored\n";
            } else {
                logFile << "- The neuron \"" << neuronName << "\" has " << sobrantes
                        << " segments that have been ignored\n";
            }
            logFileEm.unlock();

        }

    }

    void MainWindow::procesSkel(std::string api, std::vector <std::string> basals, std::string outputFile,
                                QString outputFormat, int connectionThreshold, std::string name) {
        outputFile = outputFile.substr(0, outputFile.find_last_of("."));
        std::string swcFile = outputFile;
        bool ignore = false;
        swcFile.append(".swc");
        int newThreshold = connectionThreshold;
        int sobrantes = 1000;
        sobrantes = processSkel(api, basals, swcFile, newThreshold);
        while (sobrantes > 0 && !ignore) {
            QMetaObject::invokeMethod(this, "showWarningDialog", Qt::BlockingQueuedConnection,
                                      Q_ARG(int, sobrantes),
                                      Q_ARG(int & , newThreshold),
                                      Q_ARG(std::string & , name));


            ignore = newThreshold < 0;
            if (!ignore) {
                sobrantes = processSkel(api, basals, swcFile, newThreshold);
            }

        }
        if (outputFormat != "SWC") {
            QProcess process;
            QStringList arguments;
            std::string extension = extensions.at(outputFormat.toStdString());
            std::string outFile2 = outputFile;
            outFile2.append(extension);
            arguments << QString::fromStdString(swcFile) << QString::fromStdString(outFile2) << outputFormat;
            process.start(program, arguments);
            process.waitForFinished();
            std::remove(swcFile.c_str());
        }

    }

    void MainWindow::run(std::string type, QString outputFormat, float connectionThreshold) {
        if (type == "one") {
            QString basalFiles = ui->one_basales_vrml_input->text();
            std::string api = ui->one_api_vrml_input->text().toStdString();
            std::string outputFile = ui->one_skel_swc_input->text().toStdString();
            if (basalFiles == "" || outputFile == "") {
                message_dialog("basal and output files are nedeed", QMessageBox::Warning);
            } else {

                std::vector <std::string> basals;
                for (QString &basal: basalFiles.split(';')) {
                    basals.push_back(basal.toStdString());
                }

                QFuture<void> future = QtConcurrent::run([=]() {
                    procesSkel(api, basals, outputFile, outputFormat, connectionThreshold, "");
                });
                watcher.setFuture(future);

                showProgressDialog();

            }
        } else if (type == "many") {
            std::string srcFolder = ui->many_skel_input->text().toStdString();
            std::string dstFolder = ui->many_swc_input->text().toStdString();
            if (srcFolder == "" || dstFolder == "") {
                message_dialog("All dirs are nedeed", QMessageBox::Warning);
            } else {

                QFuture<void> future = QtConcurrent::run([=]() {
                    proccesMultiple(srcFolder, dstFolder, outputFormat, connectionThreshold);
                });

                watcher.setFuture(future);
                showProgressDialog();
            }

        }
    }

    void MainWindow::finished_task() {
        procesingDialog->setMaximum(1);
        procesingDialog->setValue(1);
        message_dialog("Task finished.", QMessageBox::Information);
    }

    void MainWindow::proccesMultiple(std::string srcFolder, std::string dstFolder, QString outFormat,
                                     float connectionTreshold) {
        QThreadPool pool;
        cout << "----------Number Threads: " << QThread::idealThreadCount() * 2 << " -------------" << std::flush;
        logFile.open(dstFolder + "/log.txt", std::ios::out);
        pool.setMaxThreadCount(QThread::idealThreadCount() * 2);
        QString QsrcFolder = QString::fromStdString(srcFolder);
        QDirIterator itDir(QsrcFolder, QDir::AllEntries | QDir::NoDotAndDotDot);
        while (itDir.hasNext()) {
            QDir dir = itDir.next();
            QDirIterator itFiles(dir.absolutePath(), QDir::AllEntries | QDir::NoDotAndDotDot);
            std::string apiFile;
            std::vector <std::string> basalFiles;
            while (itFiles.hasNext()) {
                QFileInfo info = itFiles.next();
                QString path = info.absoluteFilePath();
                if (info.isFile()) {
                    if (path.contains("api", Qt::CaseInsensitive)) {
                        apiFile = path.toStdString();
                    } else if (path.contains("basal", Qt::CaseInsensitive)) {
                        basalFiles.emplace_back(path.toStdString());
                    }
                } else {
                    QDirIterator itBasalFiles(path, QDir::AllEntries | QDir::NoDotAndDotDot);
                    while (itBasalFiles.hasNext()) {
                        basalFiles.emplace_back(itBasalFiles.next().toStdString());
                    }
                }

            }
            std::string resultFile = dstFolder;
            std::string name = dir.dirName().toStdString();
            resultFile.append("/").append(name);
            QFuture<void> future = QtConcurrent::run(&pool, [=]() {
                procesSkel(apiFile, basalFiles, resultFile, outFormat, connectionTreshold, name);
            });
        }

        pool.waitForDone();
        logFile.close();
    }


    MainWindow::~MainWindow() {
        delete ui;
    }
}
