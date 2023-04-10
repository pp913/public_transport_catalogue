
#pragma once
#include "json.h"

#include <iostream>

using Dict = std::map<std::string, json::Node>;
using Array = std::vector<json::Node>;
using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;
using namespace std::literals;

namespace json {
    
//Класс Builder
//Для формирования ответа на запрос к справочнику.
class Builder{
    
    struct KeyItemContext;
    struct ValueItemContext ;
    struct DictItemContext;  
    struct ArrItemContext ;
         
    
    struct KeyItemContext {
        KeyItemContext(json::Builder& a) : ref_(a) {};
      
        DictItemContext Value(Value a) {
            return DictItemContext((ref_).Value(a));
        }
      
        DictItemContext StartDict(){
             return (ref_).StartDict();
          
        } 

        ArrItemContext StartArray(){
            return (ref_).StartArray();
        }

        Builder& ref_;
    };
    

    struct DictItemContext {
        DictItemContext(json::Builder& a) : ref_(a) {};
        Builder& ref_;
        
        KeyItemContext Key(std::string s){
            return (ref_).Key(s);
        }  

        Builder& EndDict() {
            return (ref_).EndDict();
        }
    };
    
    
    
    struct ArrItemContext {
        ArrItemContext(json::Builder& a) : ref_(a) {};
            
        ArrItemContext Value(Value a) {
            return ArrItemContext((ref_).Value(a));
        }
      
        DictItemContext StartDict(){
            return (ref_).StartDict();
          
        } 

        ArrItemContext StartArray(){
            return (ref_).StartArray();
        } 
 
        Builder& EndArray() {
            return (ref_).EndArray();
        }   
            
        Builder& ref_;

    };
    

public:
    
    KeyItemContext Key(std::string s) ;
    Builder& Value( Value a);
    DictItemContext StartDict();
    ArrItemContext StartArray();
    Builder& EndDict();
    Builder& EndArray();
 
    json::Node Build();
    
private:

    std::string last_chain_ = "none";
    std::vector < std::string> last_key_;
    json::Node root_;   
    std::vector<json::Node*> nodes_stack_ = {&root_};

};
    
}//namespace json end