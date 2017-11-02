#include "../Utility/parsejson.h"
#include <QTextStream>
#include "designRun.h"

designRun::designRun(parsProblem* atn_problem, QJsonObject& obj) : _atn_problem(_atn_problem), _obj(obj){
	_design_path = dataPool::global::getGCurrentDesignPath();
}

bool designRun::registerHfssVars(){
    //copy hfss file in designDir from projectDir
	if (!dataPool::copyFile(QString("%1/%2.hfss").arg(dataPool::global::getGWorkingProjectPath()).arg(_atn_problem->name),
		QString("%1/%2.hfss").arg(_design_path).arg(_atn_problem->name)))
		return false;
    vbsVars["hfsspath"] = QString("%1/%2").arg(_design_path).arg(_atn_problem->name);
    vbsVars["hfssname"] = _atn_problem->name;
	QJsonObject vars_value_obj, frequency_obj, far_field_obj;
	vars_value_obj = parseJson::getSubJsonObj(_obj, "varsValue");
	frequency_obj = parseJson::getSubJsonObj(_obj, "FreSetting");
	far_field_obj = parseJson::getSubJsonObj(_obj, "ThetaPhiStep");
    if(vars_value_obj.isEmpty() || frequency_obj.isEmpty() || far_field_obj.isEmpty()){
		qCritical("something wrong in file '%s/%s_conf.json'", qUtf8Printable(_design_path), qUtf8Printable(_atn_problem->name));
        //QMessageBox::critical(0, QString("Error"), QString("something wrong in file [%1/%2_conf.json]").arg(_design_path).arg(_atn_problem->name));
        return false;
    }
    for(QJsonObject::iterator iter = vars_value_obj.begin(); iter != vars_value_obj.end(); ++ iter){
        vbsVars[iter.key()] = iter.value().toString().trimmed();
    }
    QStringList strList;
    strList = dataPool::str2list(frequency_obj.value("FreStart").toString().trimmed());
    vbsVars["FreStart"] = M2GHz(strList[0]);
    strList = dataPool::str2list(frequency_obj.value("FreEnd").toString().trimmed());
    vbsVars["FreEnd"] = M2GHz(strList[0]);
    strList = dataPool::str2list(frequency_obj.value("FreNumber").toString().trimmed());
    vbsVars["FreNumber"] = strList[0];
    strList = dataPool::str2list(frequency_obj.value("SweepType").toString().trimmed());
    vbsVars["SweepType"] = strList[0];
    strList = dataPool::str2list(frequency_obj.value("PM").toString().trimmed());
    vbsVars["PM"] = strList[0];

    vbsVars["Freq"] = QString::number((vbsVars["FreStart"].toDouble() + vbsVars["FreEnd"].toDouble()) / 2.0);
    for(QJsonObject::iterator iter = far_field_obj.begin(); iter != far_field_obj.end(); ++ iter){
        strList = dataPool::str2list(iter.value().toString().trimmed());
        vbsVars[iter.key()] = strList[0];
    }
    return true;
}

bool designRun::updateVbs(){
    //read all infomation from vbsFile
	QString vbs_path = QString("%1/%2_design.vbs").arg(_design_path).arg(_atn_problem->name);
    QFile inFile(vbs_path);
    if(!inFile.open(QFile::ReadOnly | QFile::Text)){
		qCritical("Cannot read file '%s'", qUtf8Printable(vbs_path));
        //QMessageBox::critical(0, QString("Error"), QString("error: Cannot read file %1").arg(vbs_path));
        return false;
    }
    QTextStream in(&inFile);
    QString str = in.readAll();
    inFile.close();

    //update vbs file(assign paramters)
    QFile outFile(vbs_path);
	outFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QString temp;
    for(QMap<QString, QString>::iterator iter = vbsVars.begin(); iter != vbsVars.end(); ++iter){
        temp = QString("#%1#").arg(iter.key());
        str.replace(temp, iter.value());
    }
    QTextStream out(&outFile);
    out << str;
    outFile.close();
    return true;
}

QString designRun::M2GHz(QString mhz){
    double doubleGHz = mhz.toDouble() / 1000.0;
    return QString::number(doubleGHz);
}

void designRun::run(){
	bool is_already = true;
	is_already &= registerHfssVars();
	is_already &= updateVbs();
	if (!is_already) return;
    QProcess p(0);
    //p.execute("hfss", QStringList() << "-RunScriptAndExit -Ng" << vbsPath);
    p.execute("hfss", QStringList() << "-RunScript" << QString("%1/%2_design.vbs").arg(_design_path).arg(_atn_problem->name));
    /*if(! p.waitForStarted()){
        QMessageBox::critical(0, QString("Error"), QString("this process can not be called."));
        p.write("quit");
        p.kill();
        return;
    }*/
    p.waitForFinished();
    //qDebug() << QString::fromLocal8Bit(p.readAllStandardError());
}
