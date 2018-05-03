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
    this->connect(ui.pushButtonStream, SIGNAL(clicked()), this, SLOT(sendStream()));
    this->connect(ui.pushButtonRequest, SIGNAL(clicked()), this, SLOT(sendRequest()));
    this->connect(ui.pushButtonSgmk, SIGNAL(clicked()), this, SLOT(sendSgmk()));
    this->connect(ui.pushButtonToolx, SIGNAL(clicked()), this, SLOT(sendToolx()));
    this->connect(ui.pushButtonQuit, SIGNAL(clicked()), this, SLOT(sendQuit()));
    this->connect(ui.pushButtonCapture, SIGNAL(clicked()), this, SLOT(sendCapture()));
    this->connect(ui.pushButtonLogPos, SIGNAL(clicked()), this, SLOT(logPositionClick()));
    this->connect(ui.pushButtonAverage, SIGNAL(clicked()), this, SLOT(calcAverage()));
}

MyClass::~MyClass()
{
	
}

void MyClass::udpBroadcast()
{
    QString content = "TrackerServerIP:192.168.1.100";// + getHostIpAddress();
    m_pUdpServer->writeDatagram(content.toLocal8Bit(), QHostAddress::Broadcast, 2000);
}
//获取本机IP
QString MyClass::getHostIpAddress()
{
    QString strIpAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // 获取第一个本主机的IPv4地址
    int nListSize = ipAddressesList.size();
    for (int i = 0; i < nListSize; ++i)
    {
           if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
               ipAddressesList.at(i).toIPv4Address()) {
               strIpAddress = ipAddressesList.at(i).toString();
               break;
           }
     }
     // 如果没有找到，则以本地IP地址为IP
     if (strIpAddress.isEmpty())
        strIpAddress = QHostAddress(QHostAddress::LocalHost).toString();
     return strIpAddress;
}

void MyClass::startTcpserver()
{
	m_tcpServer = new QTcpServer(this);

    m_tcpServer->listen(QHostAddress::Any, 1999); //监听任何连上1234端口的ip

	connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(newConnect())); //新连接信号触发，调用newConnect()槽函数，这个跟信号函数一样，其实你可以随便取。  
}

void MyClass::newConnect()
{
	m_tcpSocket = m_tcpServer->nextPendingConnection(); //得到每个连进来的socket  
	connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(readMessage())); //有可读的信息，触发读函数槽  

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(sendMessage()));
	timer->start(10000);
}

void MyClass::readMessage() //读取信息  
{
	//    ui->textEdit_rec->te  
	QByteArray qba = m_tcpSocket->readAll(); //读取  
	qDebug() << qba;
    QString ss = QVariant(qba).toString();
    recPos = ss + "\r\n";
    recPosCount ++;

    if(logPositionSet)
        logPosition();

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
/*
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
    */
}

void MyClass::logPositionClick()
{
    logPositionSet = true;
}

void MyClass::calcAverage()
{
    QString xtag = "x";
    QString ytag = "y";
    QString ztag = "z";
    QString rtag = "rms";
    QString repeat_tag = "vepvsp";
    QFile fileRead("d:\\Dual105Log1_cut.txt");
    if(!fileRead.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ui.textEditDisp->setText("calc average: open file failed.");
        return;
    }

    QFile fileWrite("d:\\Dual105Log1_average.txt");
    if(!fileWrite.open(QIODevice::WriteOnly | QIODevice::Append))  // Append means add text to the file, not rewrite it
    {
        ui.textEditDisp->setText("calc average: open file failed.");
        return;
    }
    QTextStream txtOutput(&fileWrite);

    qreal array[2550][3] = {0};
    qreal array_average[255][3] = {0};
    QTextStream txtInput(&fileRead);
    int line_number = 0;

/*
    while (!txtInput.atEnd()) {
        QString ss = txtInput.readLine();
        int repeat_tag_num = ss.indexOf(repeat_tag);
        if(repeat_tag_num > 0) {
            QString cut_str = ss.mid(0,repeat_tag_num+3);
            txtOutput << cut_str + "\r\n"<< endl;
        } else {
            txtOutput << ss + "\r\n" << endl;
        }

    }
*/
    while (!txtInput.atEnd()) {
        QString ss = txtInput.readLine();
        int xpos = ss.indexOf(xtag);
        if(xpos > 0) {
            QString ss_from_x;
            ss_from_x = ss.mid(xpos);

            int ypos = ss_from_x.indexOf(ytag);
            int zpos = ss_from_x.indexOf(ztag);
            int rpos = ss_from_x.indexOf(rtag);
            //  +1: to eliminate "x";
            QString xcoordinate = ss_from_x.mid(1, ypos - 1);
            QString ycoordinate = ss_from_x.mid(ypos + 1, zpos - (ypos + 1));
            QString zcoordinate = ss_from_x.mid(zpos + 1, rpos - (zpos + 1));
            //txtOutput << xcoordinate + " " << endl;
            //txtOutput << ycoordinate + " "<< endl;
            //txtOutput << zcoordinate + "\r\n"<< endl;

            double val_x = xcoordinate.toDouble();
            double val_y = ycoordinate.toDouble();
            double val_z = zcoordinate.toDouble();

            array[line_number][0] = val_x;
            array[line_number][1] = val_y;
            array[line_number][2] = val_z;

            ++line_number;
            ui.textEditDisp->setText(QVariant(line_number).toString());
        }
    }

    double sum_x = 0,sum_y = 0,sum_z = 0;
    int j = 0;
    for(int i = 0; i < 2550; i++) {

        sum_x += array[i][0];
        sum_y += array[i][1];
        sum_z += array[i][2];

        if((i+1)%10 == 0) {
            array_average[j][0] = sum_x / 10.0f;
            array_average[j][1] = sum_y / 10.0f;
            array_average[j++][2] = sum_z / 10.0f;  //QString::number(data,'f',10);

            QString str_x = QString::number(sum_x / 10.0f,'f',6);  // f 表示非科学记数法  6表示小数点后保留6位
            QString str_y = QString::number(sum_y / 10.0f,'f',6);
            QString str_z = QString::number(sum_z / 10.0f,'f',6);
            txtOutput << str_x + " "<< endl;
            txtOutput << str_y + " "<< endl;
            txtOutput << str_z + "\r\n"<< endl;

            sum_x = 0,sum_y = 0,sum_z = 0;
        }
    }
    fileRead.close();
    fileWrite.close();
    ui.textEditDisp->setText("calc average finished.");
}

void MyClass::logPosition()
{
    static unsigned int logCount = 1;
    static unsigned int logPosCount = 1;
    static unsigned int recPosLastCount;

    QFile f("d:\\Dual105Log.txt");
    if(f.open(QIODevice::WriteOnly | QIODevice::Append))  // Append means add text to the file, not rewrite it
    {

        QTextStream txtOutput(&f);

        if(recPosCount != recPosLastCount) {
            recPosLastCount = recPosCount;
            txtOutput << recPos << endl;
        }

        if(logCount % 10 == 0) {
            logPositionSet = false;
            QString currentPos = "Log position " + QVariant(logPosCount).toString() + " finished.\r\n";
            txtOutput << currentPos << endl;
            ui.textEditDisp->setText(currentPos);
            logPosCount ++;
        }

        logCount ++;

    }


    //QString s1("123");
    //quint32 n1(234);

    //txtOutput << s1 << endl;
    //txtOutput << n1 << endl;

    f.close();
}

void MyClass::sendMessage() //发送信息  
{
	//broadcast "heartbeat"
	//QString strMesg = "heartbeat";
	//m_tcpSocket->write(strMesg.toStdString().c_str(), strlen(strMesg.toStdString().c_str())); //发送  
	//qDebug() << "timerout\n";
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

void MyClass::sendStream()
{
    //CMD: "stream" (string type)   to VISTEKO Tracker to enter stream mode.
    QString strMesg = "stream";
    qDebug() << "\n Stream mode on.";
    m_tcpSocket->write(strMesg.toStdString().c_str(), strlen(strMesg.toStdString().c_str()));
}

void MyClass::sendRequest()
{
    //CMD: "request" (string type)   to VISTEKO Tracker to enter request mode.
    QString strMesg = "request";
    qDebug() << "\n Request mode on.";
    m_tcpSocket->write(strMesg.toStdString().c_str(), strlen(strMesg.toStdString().c_str()));
}

void MyClass::sendRequestOnce()
{
    //CMD: "reqonce" (string type)   to VISTEKO Tracker to get one result.
    QString strMesg = "reqonce";
    qDebug() << "\n Request one result.";
    m_tcpSocket->write(strMesg.toStdString().c_str(), strlen(strMesg.toStdString().c_str()));
}

void MyClass::sendSgmk()
{
    //CMD: "sgmk" (string type)   to VISTEKO Tracker to get single marker result.
    QString strMesg = "sgmk";
    qDebug() << "\n Get single marker result.";
    m_tcpSocket->write(strMesg.toStdString().c_str(), strlen(strMesg.toStdString().c_str()));
}

void MyClass::sendToolx()
{
    //CMD: "tool01" (string type)   to VISTEKO Tracker to get tool 01 result.
    QString strMesg = "tool01";
    qDebug() << "\n Get tool 01 result.";
    m_tcpSocket->write(strMesg.toStdString().c_str(), strlen(strMesg.toStdString().c_str()));
}

void MyClass::sendQuit()
{
    //CMD: "quit" (string type)   to VISTEKO Tracker to quit tcp connection.
    QString strMesg = "quit";
    qDebug() << "\n Quit tcp connection.";
    m_tcpSocket->write(strMesg.toStdString().c_str(), strlen(strMesg.toStdString().c_str()));
}

void MyClass::sendCapture()
{
    //CMD: "capture" (string type)   to VISTEKO Tracker to capture image.
    QString strMesg = "capture";
    qDebug() << "\n Capture image.";
    m_tcpSocket->write(strMesg.toStdString().c_str(), strlen(strMesg.toStdString().c_str()));
}
