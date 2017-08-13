/*
 * Created by yuan pingzhou on 8/10/17.
 */

#ifndef REVERSEDINDEXSERVICE_REVERSEDINDEX_HPP
#define REVERSEDINDEXSERVICE_REVERSEDINDEX_HPP

#include <iostream>
#include <set>
#include <unordered_map>
#include <json/json.h>
#include <json/value.h>

#include "Container.hpp"

class InversedIndex{

private:

	std::vector<DOC*> docs;
	std::vector<CONJUNCTION*> conjunctions;
	std::vector<ASSIGNMENT*> assignments;

	void FilterWithConjunctionSize(int qry_conj_size, std::vector<int>& results);

	std::string Assignment2Doc(int idx);

	CONJUNCTION* Assignment2Conjunction(int idx);

	std::string Conjunction2Doc(CONJUNCTION* conj);

	void MatchAssignment(std::vector<int>& AssiIndexs, const ASSIGNMENT& qry, std::unordered_map<long, int>& ConjCount);

public:

	InversedIndex(){}

	void Insert(const Json::Value& doc);

	void Select(const Json::Value& query, Json::Value& result);
};

// filter with conjunction size
void InversedIndex::FilterWithConjunctionSize(int qry_conj_size, std::vector<int>& results){

  for(int i = 0;i < assignments.size();i++){
	if(assignments[i]->conj_size <= qry_conj_size){
	  results.push_back(i);
	}
  }

  return;
}

// get belong doc with assignment index
std::string InversedIndex::Assignment2Doc(int idx){
  return ((DOC*)((CONJUNCTION*)assignments[idx]->conj_belong)->doc_belong)->id;
}

// get belong junction with assignment index
CONJUNCTION* InversedIndex::Assignment2Conjunction(int idx){
  return (CONJUNCTION*)assignments[idx]->conj_belong;
}

// get belong doc with junction index
std::string InversedIndex::Conjunction2Doc(CONJUNCTION* conj){
  return ((DOC*)conj->doc_belong)->id;
}

// match assignment precisely
void InversedIndex::MatchAssignment(std::vector<int>& AssiIndexs, const ASSIGNMENT& qry, std::unordered_map<long, int>& ConjCount){
  //
  for(auto& idx : AssiIndexs){
	if(boost::iequals(assignments[idx]->name, qry.name) &&
	   boost::iequals(assignments[idx]->value, qry.value) &&
	   (assignments[idx]->relation >= qry.relation)){
	  long conj = (long)Assignment2Conjunction(idx);
	  auto it = ConjCount.find(conj);
	  if(it != ConjCount.end()){
		it->second++;
	  }
	  else {
		ConjCount.insert(std::pair<long, int>(conj, 1));
	  }
	}
  }

  return;
}

// insert
void InversedIndex::Insert(const Json::Value& doc){
  // doc
  DOC* ptr_doc = new DOC;
  ptr_doc->id = doc['id'].asString();
  ptr_doc->size = doc['conditions'].size();
  docs.push_back(ptr_doc);

  for(int i = 0;i < doc['conditions'].size();i++) {
	Json::Value ConjJson = doc['conditions'][i];
	// conjunction
	CONJUNCTION* ptr_conj = new CONJUNCTION;
	ptr_conj->doc_belong = (long) ptr_doc;
	ptr_conj->size = ConjJson.size();
	conjunctions.push_back(ptr_conj);

	for(auto& FieldName : ConjJson.getMemberNames()){
	  for(auto& FieldValue : ConjJson[FieldName]){
		// assignment
		ASSIGNMENT* ptr_assi = new ASSIGNMENT;
		ptr_assi->name = FieldName;
		ptr_assi->value = FieldValue.asString();
		ptr_assi->relation = ConjJson[FieldName].size();
		ptr_assi->conj_size = ptr_conj->size;
		ptr_assi->conj_belong = (long)ptr_conj;
		assignments.push_back(ptr_assi);
		//
		//std::cout << ptr_assi->name << "," << ptr_assi->value << "," << ptr_assi->relation
		//		  << "," << ptr_assi->conj_size << "," << ptr_assi->conj_belong << std::endl;
	  }
	}
  }

  return;
}

// select
void InversedIndex::Select(const Json::Value& query, Json::Value& result){
  // check
  std::cout << std::endl << "----- Summary ------" << std::endl;
  std::cout << "doc size " << docs.size() << std::endl;
  std::cout << "conjunction size " << conjunctions.size() << std::endl;
  std::cout << "assignment size " << assignments.size() << std::endl;

  Json::Value condition = query;
  // filter with conjunction size, candidates' conj_size must not be greater than that of query
  std::vector<int> LessConjSizeAssi;
  FilterWithConjunctionSize(condition.size(), LessConjSizeAssi);
  // check
  std::cout << std::endl <<  "After fitlered by size of conjunction " << condition.size() << "," << LessConjSizeAssi.size() << " remained ..." << std::endl;
  for(auto& idx : LessConjSizeAssi){
	std::cout<< assignments[idx]->name << "," << assignments[idx]->value << "," << assignments[idx]->relation
			 << "," << Assignment2Doc(idx) << std::endl;
  }

  std::unordered_map<std::string, int> DocCount;
  for(auto& FieldName : condition.getMemberNames()) {
	//
	std::unordered_map<long, int> ConjCount;
	for(auto &FieldValue : condition[FieldName]) {
	  // assignment
	  ASSIGNMENT qry_assi;
	  qry_assi.name = FieldName;
	  qry_assi.value = FieldValue.asString();
	  qry_assi.relation = condition[FieldName].size();
	  qry_assi.conj_size = condition.size();
	  // match assignment precisely
	  MatchAssignment(LessConjSizeAssi, qry_assi, ConjCount);
	}
	// remove conjunctions whose count is less than size of assignment
	for(auto it : ConjCount){
	  if(it.second == condition[FieldName].size()){
		std::string doc = Conjunction2Doc((CONJUNCTION*)it.first);
		auto it = DocCount.find(doc);
		if(it != DocCount.end()){
		  it->second++;
		}
		else{
		  DocCount.insert(std::pair<std::string, int>(doc, 1));
		}
	  }
	}
  }
  // remove docs whose count is less than size of conjunction
  Json::Value DocList;
  for(auto it : DocCount){
	if(it.second == condition.size()){
	  DocList.append(it.first);
	}
  }
  result['doc'] = DocList;

  return;
}
#endif //REVERSEDINDEXSERVICE_REVERSEDINDEX_HPP
