#pragma execution_character_set("utf-8")
#include "goRun.h"

goRun::goRun(QProcess* &optRunP) : _p(optRunP){}

void goRun::run() {
	//start���ӽ��̷�ʽ���½��̣���execute��������ʽ�򿪽��̣�startDetached�Ը��뷽ʽ�򿪣�������Ľ����޹ء�
	//���'-i'������'-i'��ʾ���ǶԱ�׼���벻���ն˵�����£���Ȼǿ�����
	_p->start("python.exe", QStringList() << QString("%1/start.py").arg(dataPool::global::getGDEA4ADPath()) << "AntennaLDO");
	//p.pid();0
	//sleep(10);
	//qDebug() << p.state();
	//QMessageBox::critical(0, QString("Error"),str);

	//qDebug() << p->processId();
	_p->setReadChannel(_p->StandardOutput);
	//p.setReadChannel(p.StandardError);

	//����Ҫ������տ�ʼstate״̬ΪNotRunning�������޷����뵽ѭ������
	if (!_p->waitForStarted())
		exit(1);
	//���û��棬ʵʱ��ȡ�����ó���ı�׼���
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
