#include "optimizeRun.h"

optimizeRun::optimizeRun(parsProblem* atn_problem, parsAlgorithm* algorithm, QProcess* &optRunP) : p(optRunP),
_atn_problem(atn_problem), _algorithm(algorithm){
	_optimize_path = dataPool::global::getGCurrentOptimizePath();
}

void optimizeRun::run(){
	//update file to DEA4AD, include problem,algorithm,global configure infomation
	bool is_success = true;
	is_success &= dataPool::copyFile(QString("%1/global_conf.json").arg(_optimize_path),
		QString("%1/global_conf.json").arg(dataPool::global::getGDEA4ADPath()));
	is_success &= dataPool::copyFile(QString("%1/%2_conf.json").arg(_optimize_path).arg(_algorithm->name),
		QString("%1/%2_conf.json").arg(_algorithm->path).arg(_algorithm->name));
	is_success &= dataPool::copyFile(QString("%1/%2_conf.json").arg(_optimize_path).arg(_atn_problem->name),
		QString("%1/%2_conf.json").arg(_atn_problem->path).arg(_atn_problem->name));
	if (!is_success) {
		qCritical(0, QString("Error"), QString("error: update files to DEA4AD failed."));
		return;
	}
    //start以子进程方式打开新进程；而execute以阻塞方式打开进程；startDetached以隔离方式打开，与打开它的进程无关。
    //添加'-i'参数，'-i'表示的是对标准输入不是终端的情况下，依然强制输出
    p->start("python.exe", QStringList() << QString("%1/start.py").arg(dataPool::global::getGDEA4ADPath()) << "AntennaLDO");
    //p.pid();0
    //sleep(10);
    //qDebug() << p.state();
    //QMessageBox::critical(0, QString("Error"),str);

    //qDebug() << p->processId();
    p->setReadChannel(p->StandardOutput);
    //p.setReadChannel(p.StandardError);

    //很重要，否则刚开始state状态为NotRunning，导致无法进入到循环体内
    if(! p->waitForStarted())
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
    if(!p->waitForFinished(-1))
        exit(1);
   // qDebug() << "finifshed";
    //QString str = QString::fromLocal8Bit(p.readAllStandardOutput());
}
