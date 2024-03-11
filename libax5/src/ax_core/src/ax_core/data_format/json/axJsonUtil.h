#pragma once

#include "axJsonSerializer.h"
#include "axJsonDeserializer.h"

class axJsonValue;

struct axJsonUtil : public axStaticClass {
	template<class T> static void readFile(axStrView filename, T& obj);
	template<class T> static void writeFile(axStrView filename, T& obj, bool createDir, bool logResult = true);
	template<class T> static void writeFileIfChanged(axStrView filename, T& obj, bool createDir, bool logResult = true);

	template<class T> static void serialize  (axIString& json, T& obj);
	template<class T> static void deserialize(axStrView  json, T& obj, axStrView filenameForErrorMessage);

	template<class T> static void stringify(axIString& outJson, const T& obj);
};

template<class T> inline
void axJsonUtil::readFile(axStrView filename, T& obj) {
	axTempString8 json;
	axFile::readUtf8(filename, json);
	axJsonDeserializer de(json, filename);
	de.io(obj);
}

template<class T> inline
void axJsonUtil::writeFile(axStrView filename, T& obj, bool createDir, bool logResult) {
	axTempString json;
	serialize(json, obj);
	axFile::writeFile(filename, json, createDir, logResult);
}

template<class T> inline
void axJsonUtil::writeFileIfChanged(axStrView filename, T& obj, bool createDir, bool logResult) {
	axTempString json;
	serialize(json, obj);
	axFile::writeFileIfChanged(filename, json, createDir, logResult);
}

template<class T> inline
void axJsonUtil::serialize(axIString& outJson, T& obj) {
	axJsonSerializer se(outJson);
	se.io(obj);
}

template<class T> inline
void axJsonUtil::deserialize(axStrView json, T& obj, axStrView filenameForErrorMessage) {
	axJsonDeserializer se(json, filenameForErrorMessage);
	se.io(obj);
}

template<class T> inline
void axJsonUtil::stringify(axIString& json, const T& obj) {
	axJsonWriter wr(json);
	wr.writeValue(obj);
}
