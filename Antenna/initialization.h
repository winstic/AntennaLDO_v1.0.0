#pragma once
#include "../Utility/macrodefined.h"
#include "../Utility/global.h"
#include "../Utility/parseJson.h"

void setProblemParameters(QJsonObject& obj) {		
	QJsonObject mpro_obj;
	for (QJsonObject::iterator iter = obj.begin(); iter != obj.end(); ++iter) {
		parsProblem tmp;
		mpro_obj = iter.value().toObject();
		tmp.name = iter.key();
		tmp.id = mpro_obj.value("id").toString().trimmed();
		tmp.path = mpro_obj.value("path").toString().trimmed();
		tmp.pImage = mpro_obj.value("image").toString().trimmed();
		if (tmp.pImage == "" || tmp.pImage == NULL)
			tmp.pImage = "./images/antenna.png";
		tmp.info = mpro_obj.value("info").toString().trimmed();
		tmp.type = mpro_obj.value("type").toString().trimmed().toInt();
		tmp.oper = mpro_obj.value("oper").toString().trimmed();
		//设置默认频率为最大频点（如果有）
		QJsonObject problem_obj = parseJson::getJsonObj(QString("%1/%2_conf.json").arg(tmp.path).arg(tmp.name));
		QJsonObject frequency_obj = parseJson::getSubJsonObj(problem_obj, "FreSetting");
		QStringList fre_end_list = dataPool::str2list(frequency_obj.value("FreEnd").toString().trimmed());
		double fre_end = 0.01;
		for (int i = 0; i < fre_end_list.size(); ++i) {
			if (fre_end < fre_end_list[i].toDouble())
				fre_end = fre_end_list[i].toDouble();
		}
		tmp.max_frequency = fre_end;
		dataPool::global::g_problems.push_back(tmp);
	}
}

void setAlgorithmParameters(QJsonObject& obj) {	
	QJsonObject malg_obj;
	for (QJsonObject::iterator iter = obj.begin(); iter != obj.end(); ++iter) {
		parsAlgorithm tmp;
		malg_obj = iter.value().toObject();
		tmp.name = iter.key();
		tmp.id = malg_obj.value("id").toString().trimmed();
		tmp.path = malg_obj.value("path").toString().trimmed();
		tmp.info = malg_obj.value("info").toString().trimmed();
		tmp.oper = malg_obj.value("oper").toString().trimmed();
		dataPool::global::g_algorithms.push_back(tmp);
	}
}

void setProNdAlgAssociateParameters(QJsonObject& obj) {
	alg4pro massociate;
	QJsonObject mbind_obj;
	for (QJsonObject::iterator iter = obj.begin(); iter != obj.end(); ++iter) {
		mbind_obj = iter.value().toObject();
		QString alg_id = mbind_obj.value("algorithmID").toString().trimmed();
		QString pro_id = mbind_obj.value("problemID").toString().trimmed();
		parsAlgorithm* algorithm = dataPool::global::getAlgorithmByID(alg_id);
		parsProblem* problem = dataPool::global::getProblemByID(pro_id);
		if(algorithm->oper == "i" && problem->oper == "i")
			massociate = qMakePair(alg_id, pro_id);
		dataPool::global::g_associates[massociate] = iter.key();
	}
}

void setConfigParameters(QJsonObject& obj) {
	dataPool::global::setGDefaultProjectPath(obj.value(DEFAULT_PROJECT_PATH).toString().trimmed());
	dataPool::global::setGWindowWidth(obj.value(MAIN_WINDOW_WIDTH).toString().trimmed().toInt());
	dataPool::global::setGWindowHeight(obj.value(MAIN_WINDOW_HEIGHT).toString().trimmed().toInt());
}

void loadData() {
	//使用相对路径
	const QString DEA4AD_path = "DEA4AD/trunk";
	const QString config_json_file = "config.json";
	const QString problem_json_file = "Utility/problems.json";
	const QString algorithm_json_file = "Utility/algorithms.json";
	const QString associate_json_file = "Utility/associates.json";

	dataPool::global::setGDEA4ADPath(DEA4AD_path);
	dataPool::global::setGConfigFile(config_json_file);
	
	QJsonObject conf_obj, pro_obj, alg_obj, associate_obj;
	conf_obj = parseJson::getJsonObj(config_json_file);
	pro_obj = parseJson::getJsonObj(problem_json_file);
	alg_obj = parseJson::getJsonObj(algorithm_json_file);
	associate_obj = parseJson::getJsonObj(associate_json_file);

	if (conf_obj.isEmpty()) {
		qCritical("something wrong about config.json.");
		exit(1);
	}
	if (pro_obj.isEmpty()) {
		qCritical("something wrong about problems.json.");
		exit(1);
	}
	if (alg_obj.isEmpty()) {
		qCritical("something wrong about algorithms.json.");
		exit(1);
	}
	if (associate_obj.isEmpty()) {
		qCritical("something wrong about associates.json.");
		exit(1);
	}

	qInfo("initialize config parameters....");
	setConfigParameters(conf_obj);
	qInfo("initialize problems parameters....");
	setProblemParameters(pro_obj);
	qInfo("initialize algorithms parameters....");
	setAlgorithmParameters(alg_obj);
	qInfo("initialize associate parameters....");
	setProNdAlgAssociateParameters(associate_obj);
}