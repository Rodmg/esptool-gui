#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtWidgets>
#include <QtSerialPort/QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    serial = new QSerialPort(this);

    // Fill baud box
    ui->baudBox->clear();
    ui->baudBox->addItem(QStringLiteral("1200"), QSerialPort::Baud1200);
    ui->baudBox->addItem(QStringLiteral("2400"), QSerialPort::Baud2400);
    ui->baudBox->addItem(QStringLiteral("4800"), QSerialPort::Baud4800);
    ui->baudBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->baudBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->baudBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->baudBox->addItem(QStringLiteral("57600"), QSerialPort::Baud57600);
    ui->baudBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    ui->baudBox->addItem(QStringLiteral("256000"), QSerialPort::Baud256000);
    ui->baudBox->addItem(QStringLiteral("460800"), QSerialPort::Baud460800);
    ui->baudBox->setCurrentIndex(7); // Default 115200

    ui->filePathText->setDisabled(true);
    ui->testPathText->setDisabled(true);
    ui->closeSerialBtn->setDisabled(true);
    ui->outputText->setReadOnly(true);

    loadPorts();

    filePath = "";
    testFilePath = "";

    connect(ui->reloadBtn, &QPushButton::clicked,
            this, &MainWindow::loadPorts);
    connect(ui->browseBtn, &QPushButton::clicked,
            this, &MainWindow::browseFile);
    connect(ui->browseTestBtn, &QPushButton::clicked,
            this, &MainWindow::browseTestFile);
    connect(ui->uploadBtn, &QPushButton::clicked,
            this, &MainWindow::doUpload);
    connect(ui->testBtn, &QPushButton::clicked,
            this, &MainWindow::doTest);

    // Serial port setup
    connect(serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
            this, &MainWindow::handleSerialError);
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);

    connect(ui->openSerialBtn, &QPushButton::clicked,
            this, &MainWindow::openSerialPort);
    connect(ui->closeSerialBtn, &QPushButton::clicked,
            this, &MainWindow::closeSerialPort);

    // Fetch previous settings
    QCoreApplication::setOrganizationName("makerlab.mx");
    QCoreApplication::setOrganizationDomain("makerlab.mx");
    QCoreApplication::setApplicationName("esptool-gui");
    QSettings settings;

    if(settings.contains("settings/port"))
    {
        int portIndex = settings.value("settings/port").toInt();
        ui->portBox->setCurrentIndex(portIndex);
    }

    if(settings.contains("settings/baud"))
    {
        int baudIndex = settings.value("settings/baud").toInt();
        ui->baudBox->setCurrentIndex(baudIndex);
    }

    if(settings.contains("settings/file"))
    {
        filePath = settings.value("settings/file").toString();
        ui->filePathText->setText(filePath);
    }

    if(settings.contains("settings/testFile"))
    {
        testFilePath = settings.value("settings/testFile").toString();
        ui->testPathText->setText(testFilePath);
    }

    connect(ui->portBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        [=](int index)
        {
            QSettings settings;
            settings.setValue("settings/port", index);
        });

    connect(ui->baudBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        [=](int index)
        {
            QSettings settings;
            settings.setValue("settings/baud", index);
        });

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadPorts()
{
    ui->portBox->clear();

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->portBox->addItem(info.portName(), info.systemLocation());
    }
}

void MainWindow::browseFile()
{
    QSettings settings;
    const QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
    {
        filePath = fileName;
        ui->filePathText->setText(filePath);
        settings.setValue("settings/file", filePath);
    }
}

void MainWindow::browseTestFile()
{
    QSettings settings;
    const QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
    {
        testFilePath = fileName;
        ui->testPathText->setText(testFilePath);
        settings.setValue("settings/testFile", testFilePath);
    }
}

void MainWindow::doUpload()
{
    closeSerialPort();
    ui->outputText->clear();
    ui->outputText->appendPlainText("Uploading...\n");
    QString appPath = QCoreApplication::applicationDirPath();
    QString program = appPath + "/tool-esptool/esptool";
    QStringList arguments;
    arguments << "-vv" << "-cd" << "ck" << "-cb" << ui->baudBox->currentText() << "-cp" << ui->portBox->currentData().toString() << "-cf" << filePath;

    startProcess(program, arguments);
}

// Get ESP8266 MAC only works with esptool.py, not esptool-ck
void MainWindow::doGetMac()
{
    closeSerialPort();
    ui->outputText->clear();
    ui->outputText->appendPlainText("Getting MAC...\n");
    QString program = "/usr/local/bin/esptool.py";
    QStringList arguments;
    arguments << "-b" << ui->baudBox->currentText() << "-p" << ui->portBox->currentData().toString() << "read_mac";

    startProcess(program, arguments);
}

void MainWindow::doTest()
{
    closeSerialPort();
    ui->outputText->clear();
    ui->outputText->appendPlainText("Uploading Test Firmware...\n");
    QString appPath = QCoreApplication::applicationDirPath();
    QString program = appPath + "/tool-esptool/esptool";
    QStringList arguments;
    arguments << "-vv" << "-cd" << "ck" << "-cb" << ui->baudBox->currentText() << "-cp" << ui->portBox->currentData().toString() << "-cf" << testFilePath;

    startProcess(program, arguments, true);
}

void MainWindow::setInputsDisabled(bool disabled)
{
    ui->baudBox->setDisabled(disabled);
    ui->portBox->setDisabled(disabled);
    ui->reloadBtn->setDisabled(disabled);
    ui->uploadBtn->setDisabled(disabled);
    ui->browseBtn->setDisabled(disabled);
    ui->browseTestBtn->setDisabled(disabled);
    ui->testBtn->setDisabled(disabled);
}

void MainWindow::startProcess(QString program, QStringList arguments, bool isTest)
{
    process = new QProcess(this);

    connect(process, &QProcess::started, this, [=]()
    {
        setInputsDisabled(true);
    });

    connect(process, &QProcess::readyReadStandardError, this, [=]()
    {
        ui->outputText->insertPlainText(process->readAllStandardError());
        ui->outputText->verticalScrollBar()->setValue(ui->outputText->verticalScrollBar()->maximum());
    });

    connect(process, &QProcess::readyReadStandardOutput, this, [=]()
    {
        ui->outputText->insertPlainText(process->readAllStandardOutput());
        ui->outputText->verticalScrollBar()->setValue(ui->outputText->verticalScrollBar()->maximum());
    });

    connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, [=](int exitCode, QProcess::ExitStatus exitStatus)
    {
        ui->outputText->appendPlainText(QString("Finished with exit code ") + QString::number(exitCode));
        setInputsDisabled(false);
        if(isTest)
        {
            ui->outputText->appendPlainText("\nTest Firmware upload finished. Press 'Open Serial' and press the config button on the Device (if any) to start testing.");
        }
    });

    connect(process, static_cast<void(QProcess::*)(QProcess::ProcessError)>(&QProcess::errorOccurred), this, [=](QProcess::ProcessError error)
    {
        ui->outputText->appendPlainText( QString("An error occurred: ") + error );
        setInputsDisabled(false);
    });

    process->start(program, arguments);
}

void MainWindow::handleSerialError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }
}

void MainWindow::closeSerialPort()
{
    if (serial->isOpen())
    {
        serial->close();
        ui->openSerialBtn->setDisabled(false);
        ui->closeSerialBtn->setDisabled(true);
        ui->outputText->appendPlainText("__________________________________________________________\nSerial port closed.");
    }
}

void MainWindow::openSerialPort()
{
    serial->setPortName(ui->portBox->currentText());
    serial->setBaudRate(ui->baudBox->currentData().toInt());
    //serial->setDataBits(p.dataBits);
    //serial->setParity(p.parity);
    //serial->setStopBits(p.stopBits);
    //serial->setFlowControl(p.flowControl);
    if (serial->open(QIODevice::ReadWrite)) {
        ui->openSerialBtn->setDisabled(true);
        ui->closeSerialBtn->setDisabled(false);
        ui->outputText->appendPlainText("\nSerial port open:\n__________________________________________________________\n");
    } else {
        QMessageBox::critical(this, tr("Error"), serial->errorString());
        ui->outputText->appendPlainText( tr("Open error"));
    }
}

void MainWindow::readData()
{
    QByteArray data = serial->readAll();
    ui->outputText->insertPlainText(data);
    ui->outputText->verticalScrollBar()->setValue(ui->outputText->verticalScrollBar()->maximum());
}
