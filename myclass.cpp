#include "myclass.h"

MyClass::MyClass(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	this->connect(ui.pushButton_connect, SIGNAL(clicked()), this, SLOT(startTcpserver()));
	this->connect(ui.pushButton_start, SIGNAL(clicked()), this, SLOT(sendStartTracking()));
	this->connect(ui.pushButton_stop, SIGNAL(clicked()), this, SLOT(sendStopTracking()));
}

MyClass::~MyClass()
{
	
}

void MyClass::startTcpserver()
{
	m_tcpServer = new QTcpServer(this);

	m_tcpServer->listen(QHostAddress::Any, 1234); //监听任何连上1234端口的ip  

	connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(newConnect())); //新连接信号触发，调用newConnect()槽函数，这个跟信号函数一样，其实你可以随便取。  
}

void MyClass::newConnect()
{
	m_tcpSocket = m_tcpServer->nextPendingConnection(); //得到每个连进来的socket  
	connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(readMessage())); //有可读的信息，触发读函数槽  

}

void MyClass::readMessage() //读取信息  
{
	//    ui->textEdit_rec->te  
	QByteArray qba = m_tcpSocket->readAll(); //读取  
	qDebug() << qba;
	QString ss = QVariant(qba).toString();
	//ui.textEdit_rec->setText(ss);

	QString xtag = "x";
	QString ytag = "y";
	QString ztag = "z";
	QString rtag = "rms";
	QString etag = "vep";
	int xpos = ss.indexOf(xtag); 
	QString ss_from_x;
	ss_from_x = ss.mid(xpos);

	int ypos = ss_from_x.indexOf(ytag);
	int zpos = ss_from_x.indexOf(ztag);
	int rpos = ss_from_x.indexOf(rtag);
	int epos = ss_from_x.indexOf(etag);
	//  +1: to eliminate "x";
	QString xcoordinate = ss_from_x.mid(1, ypos - 1);
	QString ycoordinate = ss_from_x.mid(ypos + 1, zpos - (ypos + 1));
	QString zcoordinate = ss_from_x.mid(zpos + 1, rpos - (zpos + 1));
	QString rms_error = ss_from_x.mid(rpos + 3, epos - (rpos + 3));

	ui.label_valx->setText(xcoordinate);
	ui.label_valy->setText(ycoordinate);
	ui.label_valz->setText(zcoordinate);
	ui.label_vale->setText(rms_error);
}

void MyClass::sendMessage() //发送信息  
{
	//QString strMesg = ui.lineEdit_sendmessage->text();
	//qDebug() << strMesg;
	//m_tcpSocket->write(strMesg.toStdString().c_str(), strlen(strMesg.toStdString().c_str())); //发送  
}

void MyClass::sendStartTracking() //发送开始定位追踪命令
{
	//CMD: "start" (string type)  to VISTEKO Tracker to get started.
	QString strMesg = "start";


	qDebug() << "\n Start Tracking ...";
	m_tcpSocket->write(strMesg.toStdString().c_str(), strlen(strMesg.toStdString().c_str())); //发送 
}

void MyClass::sendStopTracking() //发送停止定位追踪命令
{
	//CMD: "stop" (string type)   to VISTEKO Tracker to get stopped.
	QString strMesg = "stop";
	qDebug() << "\n Stop Tracking ...";
	m_tcpSocket->write(strMesg.toStdString().c_str(), strlen(strMesg.toStdString().c_str())); //发送 
}