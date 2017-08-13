/*
 * Created by yuan pingzhou on 8/9/17.
 */
#ifndef REVERSEDINDEXSERVICE_FILELOADER_HPP
#define REVERSEDINDEXSERVICE_FILELOADER_HPP

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>

#include <json/json.h>
#include <json/value.h>

class FileLoader{

private:
	std::string DataFile = "";
	std::string FieldFile = "";

public:

	FileLoader(std::string df, std::string ff): DataFile(df),FieldFile(ff){}

	static boost::optional<FileLoader*> GetInstance(std::string DataFile, std::string FieldFile){
	  // TODO, need to be written into log
	  // TODO, more assertions need to be done
	  if(DataFile.empty() || FieldFile.empty()){
		return boost::none;
	  }
	  else {
		return new FileLoader(DataFile, FieldFile);
	  }
	}

	void Load(Json::Value& result){

	  std::map<std::string, std::set<std::string>> fields;
	  // load field data
	  std::ifstream ff(FieldFile);
	  std::string line;
	  while(std::getline(ff, line)){
		std::vector<std::string> KVParts(2);
		boost::split(KVParts, line, boost::is_any_of(":"));
		std::vector<std::string> VParts;
		boost::split(VParts, KVParts[1], boost::is_any_of(","));
		fields.insert(std::pair<std::string, std::set<std::string>>(KVParts[0], std::set<std::string>(VParts.begin(), VParts.end())));
	  }
	  ff.close();

	  // load ad data
	  std::vector<std::string> header;
	  std::ifstream df(DataFile);
	  bool FirstLine = true;
	  while(std::getline(df, line)){
		if(FirstLine){
		  FirstLine = false;
		  boost::split(header, line, boost::is_any_of(","));
		  continue;
		}
		Json::Value doc;
		Json::Value conditions;

		std::vector<std::string> KVParts;
		boost::split(KVParts, line, boost::is_any_of(","));

		std::vector<std::string> values;
		copy(KVParts.begin() + 1, KVParts.end(), std::back_inserter(values));

		for(int i = 0;i < values.size();i++){

		  Json::Value condition;

		  std::vector<std::string> attrs(3);
		  boost::split(attrs, values[i], boost::is_any_of(":"));

		  for(int j = 0;j < attrs.size();j++){
			std::string FieldName = header[j + 1];

			std::string FieldStr = attrs[j];
			Json::Value FieldList;

			std::set<std::string> FieldValues;
			int ValueIndex = 0;
			if(boost::iequals(FieldStr, "unknown")){
//			  FieldValues = fields.at(FieldName);
//			  for(auto& iter : FieldValues){
//				// add field value
//				FieldList[ValueIndex++] = iter;
//			  }
			}
			else{
			  boost::split(FieldValues, FieldStr, boost::is_any_of("/"));
			  for(auto& iter : FieldValues){
				// add field value
				FieldList[ValueIndex++] = iter;
			  }
			}
			// add field
			if(FieldList.isNull() == false) {
			  condition[FieldName] = FieldList;
			}
		  }

		  // add single condition
		  conditions.append(condition);
		}
		// add doc
		doc['id'] = KVParts[0];
		doc['conditions'] = conditions;

		result.append(doc);
	  }
	}
};

#endif //REVERSEDINDEXSERVICE_FILELOADER_HPP
