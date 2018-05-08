#pragma execution_character_set("utf-8")
#include "goRun.h"

goRun::goRun(QProcess* &optRunP) : _p(optRunP){}

void goRun::run() {
	//start以子进程方式打开新进程；而execute以阻塞方式打开进程；startDetached以隔离方式打开，与打开它的进程无关。
	//添加'-i'参数，'-i'表示的是对标准输入不是终端的情况下，依然强制输出
	_p->start("python.exe", QStringList() << QString("%1/start.py").arg(dataPool::global::getGDEA4ADPath()) << "AntennaLDO");
	//p.pid();0
	//sleep(10);
	//qDebug() << p.state();
	//QMessageBox::critical(0, QString("Error"),str);

	//qDebug() << p->processId();
	_p->setReadChannel(_p->StandardOutput);
	//p.setReadChannel(p.StandardError);

	//很重要，否则刚开始state状态为NotRunning，导致无法进入到循环体内
	if (!_p->waitForStarted())
		exit(1);
	//禁用缓存，实时获取被调用程序的标准输出
	//setvbuf(stdout, (char*)NULL, _IONBF, 0);
	/*char data[10240];
	while(p->waitForReadyRead(-1)){
	//if(p.waitForReadyRead(-1)){
	//p.read(data, sizeof(data));

	p.readLine(data, sizeof(data));
	qDebug() << data;
	//}
	}*/
	if (!_p->waitForFinished(-1))
		exit(1);
	// qDebug() << "finifshed";
	//QString str = QString::fromLocal8Bit(p.readAllStandardOutput());
}
