#ifndef MYCLASS_H
#define MYCLASS_H

#include <QtWidgets/QMainWindow>
#include "ui_myclass.h"
#include <QtNetwork>
#include <QTcpServer>
#include <QTextStream>
QT_BEGIN_NAMESPACE
class QLabel;
class QPushButton;
class QTcpServer;
QT_END_NAMESPACE

class MyClass : public QMainWindow
{
	Q_OBJECT

public:
	MyClass(QWidget *parent = 0);
	~MyClass();

private slots:
	void startTcpserver();
	void sendMessage();
	void sendStartTracking();
	void sendStopTracking();
	void newConnect();
	void readMessage();
    void udpBroadcast();
    void sendCapture();
    void sendStream();
    void sendRequest();
    void sendRequestOnce();
    void sendQuit();
    void sendSgmk();
    void sendToolx();
    void logPosition();
    void logPositionClick();
    QString getHostIpAddress();
private:
	Ui::MyClassClass ui;
	QTcpServer *m_tcpServer;
	QTcpSocket *m_tcpSocket;
	QTimer *timer;
    QUdpSocket *m_pUdpServer;
    QString recPos;
    unsigned int recPosCount;
    bool logPositionSet = false;
};

#endif // MYCLASS_H
