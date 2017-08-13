#include "utils/FileLoader.hpp"
#include "index/InversedIndex.hpp"

#include <set>
#include <iostream>
#include <json/json.h>
#include <json/value.h>

int main() {
  std::cout << "===== Reversed Index Service =====" << std::endl;

  std::string DataFile = "/Users/yuanpingzhou/project/workspace/c++/LightDNF/data/sample.dat";
  std::string FieldFile = "/Users/yuanpingzhou/project/workspace/c++/LightDNF/data/fields.dat";
  boost::optional<FileLoader*> fl = FileLoader::GetInstance(DataFile, FieldFile);
  if(fl){
    //std::map<std::string, std::vector<std::string>> data;
    Json::Value data;
    fl.get()->Load(data);
	// insert
    InversedIndex* ri = new InversedIndex();
    for(int i = 0;i < data.size();i++){
      //std::cout << data[i]['id'].asString() << std::endl;
      //std::cout << data[i]['conditions'].toStyledString() << std::endl;
	  ri->Insert(data[i]);
    }
	// query
    Json::Value query;
    Json::Value result;
    Json::Value age;
	Json::Value area;
    age[0] = "1";
	age[1] = "3";
    area[0] = "100200";
    query["audience_age"] = age;
	query["audience_area"] = area;

    ri->Select(query, result);

    std::cout << std::endl << " result " << result['doc'] << std::endl;
  }
  return 0;
}