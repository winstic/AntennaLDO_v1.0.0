#include <QFile>
#include <QTextStream>
#include <QString>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include "global.h"
#include "parseJson.h"

QJsonObject parseJson::getJsonObj(const QString &path) {
	QJsonObject obj;
	QFile file(path);
	if (!file.open(QFile::ReadOnly | QFile::Text))
		qCritical("Cannot read file: '%s'", qUtf8Printable(path));
	else {
		QByteArray byteArray = file.readAll();
		file.close();
		QJsonParseError jsonError;
		QJsonDocument jsonDocument = QJsonDocument::fromJson(byteArray, &jsonError);
		if (jsonError.error == QJsonParseError::NoError) {
			if (jsonDocument.isObject()) {
				obj = jsonDocument.object();
			}
		}
	}
	return obj;
}

QJsonObject parseJson::getSubJsonObj(const QJsonObject &obj, const QString &key) {
	QJsonObject subObj;
	if (obj.contains(key)) {
		QJsonValue value = obj.value(key);
		if (value.isObject())
			subObj = value.toObject();
	}
	return subObj;
}

bool parseJson::write(const QString &path, QJsonObject* obj) {
	if (obj == nullptr) {
		qCritical("json object is empty when update to file");
		return false;
	}
	QFile file(path);
	if (!file.open(QFile::WriteOnly)) {
		qCritical("Cannot write file: '%s'", qUtf8Printable(path));
		return false;
	}
	QJsonDocument jsonDocument;
	jsonDocument.setObject(*obj);
	QByteArray byteArray = jsonDocument.toJson(QJsonDocument::Indented);
	QTextStream out(&file);
	out.setCodec("UTF-8");
	out << QString(byteArray);
	file.close();
	qInfo("write file: '%s' successed.", qUtf8Printable(path));
	return true;
}

void parseJson::updateConfigFile(const QString& key, const QString& value) {
	const QString config_json_file = "config.json";
	QJsonObject conf_obj = getJsonObj(config_json_file);
	if (conf_obj.isEmpty()) {
		qCritical("something wrong about config.json.");
		exit(1);
	}
	conf_obj.insert(key, value);
	write(config_json_file, &conf_obj);
}
