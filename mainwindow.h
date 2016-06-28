#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtSerialPort/QSerialPort>
#include <QProcess>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void loadPorts();
    void browseFile();
    void browseTestFile();
    void doUpload();
    void doGetMac();
    void doTest();
    void closeSerialPort();
    void openSerialPort();
    void readData();

private:
    Ui::MainWindow *ui;

    void setInputsDisabled(bool disabled);
    void startProcess(QString program, QStringList arguments, bool isTest = false);

    void handleSerialError(QSerialPort::SerialPortError error);

    QSerialPort *serial;
    QProcess *process;
    QString filePath;
    QString testFilePath;
};

#endif // MAINWINDOW_H
