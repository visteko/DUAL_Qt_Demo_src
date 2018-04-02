#include "myclass.h"

MyClass::MyClass(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	startTcpserver();
    m_pUdpServer = new QUdpSocket();
	//this->connect(ui.pushButton_connect, SIGNAL(clicked()), this, SLOT(startTcpserver()));
	this->connect(ui.pushButton_start, SIGNAL(clicked()), this, SLOT(sendStartTracking()));
	this->connect(ui.pushButton_stop, SIGNAL(clicked()), this, SLOT(sendStopTracking()));
    this->connect(ui.pushButtonBroadcast, SIGNAL(clicked()), this, SLOT(udpBroadcast()));
}

MyClass::~MyClass()
{
	
}

void MyClass::udpBroadcast()
{
    QString content = "TrackerServerIP:" + getHostIpAddress();
    m_pUdpServer->writeDatagram(content.toLocal8Bit(), QHostAddress::Broadcast, 2000);
}
//��ȡ����IP
QString MyClass::getHostIpAddress()
{
    QString strIpAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // ��ȡ��һ����������IPv4��ַ
    int nListSize = ipAddressesList.size();
    for (int i = 0; i < nListSize; ++i)
    {
           if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
               ipAddressesList.at(i).toIPv4Address()) {
               strIpAddress = ipAddressesList.at(i).toString();
               break;
           }
     }
     // ���û���ҵ������Ա���IP��ַΪIP
     if (strIpAddress.isEmpty())
        strIpAddress = QHostAddress(QHostAddress::LocalHost).toString();
     return strIpAddress;
}

void MyClass::startTcpserver()
{
	m_tcpServer = new QTcpServer(this);

    m_tcpServer->listen(QHostAddress::Any, 2234); //�����κ�����1234�˿ڵ�ip

	connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(newConnect())); //�������źŴ���������newConnect()�ۺ�����������źź���һ������ʵ��������ȡ��  
}

void MyClass::newConnect()
{
	m_tcpSocket = m_tcpServer->nextPendingConnection(); //�õ�ÿ����������socket  
	connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(readMessage())); //�пɶ�����Ϣ��������������  

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(sendMessage()));
	timer->start(10000);
}

void MyClass::readMessage() //��ȡ��Ϣ  
{
	//    ui->textEdit_rec->te  
	QByteArray qba = m_tcpSocket->readAll(); //��ȡ  
	qDebug() << qba;
	QString ss = QVariant(qba).toString();
	//ui.textEdit_rec->setText(ss);

	QString xtag = "x";
	QString ytag = "y";
	QString ztag = "z";
	QString rtag = "rms";
	QString q0tag = "q0";
	QString q1tag = "q1";
	QString q2tag = "q2";
	QString q3tag = "q3";
	QString etag = "vep";
	int xpos = ss.indexOf(xtag); 
	QString ss_from_x;
	ss_from_x = ss.mid(xpos);

	int ypos = ss_from_x.indexOf(ytag);
	int zpos = ss_from_x.indexOf(ztag);
	int rpos = ss_from_x.indexOf(rtag);
	int q0pos = ss_from_x.indexOf(q0tag);
	int q1pos = ss_from_x.indexOf(q1tag);
	int q2pos = ss_from_x.indexOf(q2tag);
	int q3pos = ss_from_x.indexOf(q3tag);
	int epos = ss_from_x.indexOf(etag);
	//  +1: to eliminate "x";
	QString xcoordinate = ss_from_x.mid(1, ypos - 1);
	QString ycoordinate = ss_from_x.mid(ypos + 1, zpos - (ypos + 1));
	QString zcoordinate = ss_from_x.mid(zpos + 1, rpos - (zpos + 1));
	QString rms_error = ss_from_x.mid(rpos + 3, q0pos - (rpos + 3));
	QString q0 = ss_from_x.mid(q0pos + 2, q1pos - (q0pos + 2));
	QString q1 = ss_from_x.mid(q1pos + 2, q2pos - (q1pos + 2));
	QString q2 = ss_from_x.mid(q2pos + 2, q3pos - (q2pos + 2));
	QString q3 = ss_from_x.mid(q3pos + 2, epos - (q3pos + 2));

	ui.label_valx->setText(xcoordinate);
	ui.label_valy->setText(ycoordinate);
	ui.label_valz->setText(zcoordinate);
	ui.label_vale->setText(rms_error);
	ui.label_valq0->setText(q0);
	ui.label_valqx->setText(q1);
	ui.label_valqy->setText(q2);
	ui.label_valqz->setText(q3);

	//show double, NOT string
	double val_x = xcoordinate.toDouble();
	double val_y = ycoordinate.toDouble();
	double val_z = zcoordinate.toDouble();
	double val_rms = rms_error.toDouble();
	double val_q0 = q0.toDouble();
	double val_q1 = q1.toDouble();
	double val_q2 = q2.toDouble();
	double val_q3 = q3.toDouble();
	qDebug("\n x: %f  y: %f  z: %f  rms: %f  q0: %f  q1: %f  q2: %f  q3: %f", val_x, val_y, val_z, val_rms, val_q0, val_q1, val_q2, val_q3);
}

void MyClass::sendMessage() //������Ϣ  
{
	//broadcast "heartbeat"
	//QString strMesg = "heartbeat";
	//m_tcpSocket->write(strMesg.toStdString().c_str(), strlen(strMesg.toStdString().c_str())); //����  
	//qDebug() << "timerout\n";
}

void MyClass::sendStartTracking() //���Ϳ�ʼ��λ׷������
{
	//CMD: "start" (string type)  to VISTEKO Tracker to get started.
	QString strMesg = "start";


	qDebug() << "\n Start Tracking ...";
	m_tcpSocket->write(strMesg.toStdString().c_str(), strlen(strMesg.toStdString().c_str())); //���� 
}

void MyClass::sendStopTracking() //����ֹͣ��λ׷������
{
	//CMD: "stop" (string type)   to VISTEKO Tracker to get stopped.
	QString strMesg = "stop";
	qDebug() << "\n Stop Tracking ...";
	m_tcpSocket->write(strMesg.toStdString().c_str(), strlen(strMesg.toStdString().c_str())); //���� 
}
