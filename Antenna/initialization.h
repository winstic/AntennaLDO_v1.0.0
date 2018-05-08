#pragma once
#pragma execution_character_set("utf-8")
#include <QtAlgorithms>
#include "../Utility/macrodefined.h"
#include "../Utility/global.h"
#include "../Utility/parseJson.h"

//register problems into json file
void registerProblems(QString text_file, QString DEA4AD_path) {
	QFile file(text_file);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qCritical("not exist or can not open problems.txt.");
		exit(1);
	}
	QTextStream in(&file);
	QString line = in.readLine();
	while (!line.isNull()) {
		if (line.simplified().compare("##flagsBegin") == 0) {
			line = in.readLine();
			while (!line.isNull() && !(line.simplified().compare("##flagsEnd") == 0)) {
				QStringList type_list = line.split(":", QString::SkipEmptyParts);
				if (type_list.length() == 2) {
					dataPool::global::g_flags.insert(type_list[0].simplified(), type_list[1].simplified());
				}
				else {
					qCritical("<problems.txt> wrong register infomation %s(too many ':' or others)", qUtf8Printable(line));
				}	
				line = in.readLine();
			}
			if (line.isNull()) break;
		}
		if (line.simplified().compare("##begin") == 0) {
			line = in.readLine();
			while (!line.isNull() && !(line.simplified().compare("##end") == 0)) {
				QRegExp re("[ ,\t]");
				QStringList problem_list = line.split(re, QString::SkipEmptyParts);
				if (problem_list.length() == 2) {
					QString name, type, problem_path, problem_image;
					name = problem_list[0].simplified();
					type = problem_list[1].simplified();
					problem_path = QString("%1/problem/%2").arg(DEA4AD_path).arg(name);
					QDir problem_dir(problem_path);
					if (!problem_dir.exists()) {
						qCritical("%s 问题文件夹未添加到DEA4AD框架内。", qUtf8Printable(name));
						line = in.readLine();
						continue;
					}
					problem_image = "./images/antenna.png";
					QString png_path = QString("%1/%2.png").arg(problem_path).arg(name);
					QString jpg_path = QString("%1/%2.jpg").arg(problem_path).arg(name);
					QFile png_image(png_path);
					QFile jpg_image(jpg_path);
					if (png_image.exists())
						problem_image = png_path;
					else if (jpg_image.exists())
						problem_image = jpg_path;

					parsProblem tmp;
					tmp.name = name;
					tmp.path = problem_path;
					tmp.pImage = problem_image;
					if (dataPool::global::g_flags.contains(type))
						tmp.info = QString("%1; ").arg(dataPool::global::g_flags[type]);
					tmp.type = type;
					tmp.oper = "i";
					//设置默认频率为最大频点（如果有）
					QJsonObject problem_obj = parseJson::getJsonObj(QString("%1/%2_conf.json").arg(tmp.path).arg(tmp.name));
					QJsonObject frequency_obj = parseJson::getSubJsonObj(problem_obj, "FreSetting");
					QStringList fre_start_list = dataPool::str2list(frequency_obj.value("FreStart").toString().simplified());
					QStringList fre_end_list = dataPool::str2list(frequency_obj.value("FreEnd").toString().simplified());
					int length = fre_start_list.size();
					if (fre_end_list.size() != length) {
						qCritical("%s天线问题json文件频率数据设置有误, 请仔细核对。", qUtf8Printable(name));
						return;
					}
					for (int i = 0; i < length; ++i) {
						freRange fr(fre_start_list[i].toDouble(), fre_end_list[i].toDouble());
						tmp.frequencyRanges.append(fr);
					}
					tmp.fillMaxFrequency();
					dataPool::global::g_problems.push_back(tmp);
				}
				else {
					qCritical("<problems.txt> wrong register infomation %s", qUtf8Printable(line));
				}

				line = in.readLine();
			}
			if (line.isNull()) break;
		}
		line = in.readLine();
	}
	file.close();
	qSort(dataPool::global::g_problems.begin(), 
		dataPool::global::g_problems.end(),
		[](parsProblem a, parsProblem b) {return a.name.toUpper() < b.name.toUpper(); });
}

void registerAlgorithms(QString text_file, QString DEA4AD_path) {
	QFile file(text_file);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qCritical("not exist or can not open algorithms.txt.");
		exit(1);
	}
	QTextStream in(&file);
	QString line = in.readLine();
	while (!line.isNull()) {
		if (line.simplified().compare("##flagsBegin") == 0) {
			line = in.readLine();
			while (!line.isNull() && !(line.simplified().compare("##flagsEnd") == 0)) {
				QStringList type_list = line.split(":", QString::SkipEmptyParts);
				if (type_list.length() == 2) {
					dataPool::global::g_flags.insert(type_list[0].simplified(), type_list[1].simplified());
				}
				else {
					qCritical("<algorithms.txt> wrong register infomation %s(too many ':' or others)", qUtf8Printable(line));
				}
				line = in.readLine();
			}
			if (line.isNull()) break;
		}
		if (line.simplified().compare("##begin") == 0) {
			line = in.readLine();
			while (!line.isNull() && !(line.simplified().compare("##end") == 0)) {
				QRegExp re("[ ,\t]");
				QStringList algorithm_list = line.split(re, QString::SkipEmptyParts);
				if (algorithm_list.length() == 2) {
					QString name, type, algorithm_path;
					name = algorithm_list[0].simplified();
					type = algorithm_list[1].simplified();
					algorithm_path = QString("%1/algorithm/%2").arg(DEA4AD_path).arg(name);
					QDir algorithm_dir(algorithm_path);
					if (!algorithm_dir.exists()) {
						qCritical("%s 算法文件夹未添加到DEA4AD框架内。", qUtf8Printable(name));
						line = in.readLine();
						continue;
					}

					parsAlgorithm tmp;
					//tmp.id = QUuid::createUuid().toString();
					tmp.name = name;
					tmp.path = algorithm_path;					
					QStringList info_list = type.split("&", QString::SkipEmptyParts);
					for (unsigned int i = 0; i < info_list.size(); ++i) {
						if (dataPool::global::g_flags.contains(info_list[i]))
							tmp.info += QString("%1; ").arg(dataPool::global::g_flags[info_list[i]]);
					}						
						
					tmp.type = type;
					tmp.oper = "i";
					dataPool::global::g_algorithms.push_back(tmp);
				}
				else {
					qCritical("<algorithms.txt> wrong register infomation %s", qUtf8Printable(line));
				}

				line = in.readLine();
			}
			if (line.isNull()) break;
		}
		line = in.readLine();
	}
	file.close();
	qSort(dataPool::global::g_algorithms.begin(),
		dataPool::global::g_algorithms.end(),
		[](parsAlgorithm a, parsAlgorithm b) {return a.name.toUpper() < b.name.toUpper(); });
}

/*void setProblemParameters(QJsonObject& obj) {		
	QJsonObject mpro_obj;
	for (QJsonObject::iterator iter = obj.begin(); iter != obj.end(); ++iter) {
		parsProblem tmp;
		mpro_obj = iter.value().toObject();
		tmp.name = iter.key();
		tmp.path = mpro_obj.value("path").toString().simplified();
		tmp.pImage = mpro_obj.value("image").toString().simplified();
		tmp.info = mpro_obj.value("info").toString().simplified();
		tmp.type = mpro_obj.value("type").toString().simplified();
		tmp.oper = mpro_obj.value("oper").toString().simplified();
		tmp.max_frequency = mpro_obj.value("maxFrequency").toString().simplified().toDouble();
		dataPool::global::g_problems.push_back(tmp);
	}
}

void setAlgorithmParameters(QJsonObject& obj) {	
	QJsonObject malg_obj;
	for (QJsonObject::iterator iter = obj.begin(); iter != obj.end(); ++iter) {
		parsAlgorithm tmp;
		malg_obj = iter.value().toObject();
		tmp.name = iter.key();
		tmp.path = malg_obj.value("path").toString().simplified();
		tmp.info = malg_obj.value("info").toString().simplified();
		tmp.type = malg_obj.value("type").toString().simplified();
		tmp.oper = malg_obj.value("oper").toString().simplified();
		dataPool::global::g_algorithms.push_back(tmp);
	}
}
*/

void setProNdAlgAssociateParameters(QJsonObject& obj) {
	alg4pro massociate;
	QJsonObject mbind_obj;
	for (QJsonObject::iterator iter = obj.begin(); iter != obj.end(); ++iter) {
		mbind_obj = iter.value().toObject();
		QString alg_name = mbind_obj.value("algorithm").toString().simplified();
		QString pro_name = mbind_obj.value("problem").toString().simplified();
		parsAlgorithm* algorithm = dataPool::global::getAlgorithmByName(alg_name);
		parsProblem* problem = dataPool::global::getProblemByName(pro_name);
		if (problem == nullptr || algorithm == nullptr) {
			continue;
		}
		if (algorithm->oper == "i" && problem->oper == "i") {
			massociate = qMakePair(alg_name, pro_name);
			dataPool::global::g_associates[massociate] = iter.key();
		}
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
	const QString DEA4AD_problems_text_file = "DEA4AD/trunk/problem/problems.txt";
	const QString DEA4AD_algorithms_text_file = "DEA4AD/trunk/algorithm/algorithms.txt";
	const QString config_json_file = "config.json";
	//const QString problem_json_file = "Utility/problems.json";
	//const QString algorithm_json_file = "Utility/algorithms.json";
	const QString associate_json_file = "Utility/associates.json";	

	dataPool::global::setGDEA4ADPath(DEA4AD_path);
	dataPool::global::setGConfigFile(config_json_file);
	
	qInfo("register problems....");
	registerProblems(DEA4AD_problems_text_file, DEA4AD_path);
	qInfo("register algorithms....");
	registerAlgorithms(DEA4AD_algorithms_text_file, DEA4AD_path);

	QJsonObject conf_obj, associate_obj;
	conf_obj = parseJson::getJsonObj(config_json_file);
	//pro_obj = parseJson::getJsonObj(problem_json_file);
	//alg_obj = parseJson::getJsonObj(algorithm_json_file);
	associate_obj = parseJson::getJsonObj(associate_json_file);

	qInfo("initialize config parameters....");
	setConfigParameters(conf_obj);
	//qInfo("initialize problems parameters....");
	//setProblemParameters(pro_obj);
	//qInfo("initialize algorithms parameters....");
	//setAlgorithmParameters(alg_obj);
	qInfo("initialize associate parameters....");
	setProNdAlgAssociateParameters(associate_obj);
}