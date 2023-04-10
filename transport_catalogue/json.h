#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <variant>
#include <initializer_list>

namespace json {

class Node;
// Объявления Dict и Array
using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;

using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;
          using namespace std::literals;  //std::variant для хранения различных значений в узле.
        
    
// Эта ошибка выбрасывается при ошибках парсинга JSON
class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

Node LoadString(std::istream& input);
    
//Узел JSON-файла    
class Node {

public:
    //Конструкторы
    Node() = default;    
    Node(Array value);
    Node(std::nullptr_t value);
    Node(Dict value);
    Node(bool value);
    Node(int value);
    Node(double value);
    Node(std::string value);
    
    //Геттеры
    const Value& GetValue() const ;
    Value& GetValue() ;
    
    //Проверка типа значения
    bool IsInt() const ;
    bool IsDouble() const ;
    bool IsPureDouble() const ;
    bool IsBool() const ;
    bool IsString() const;
    bool IsNull() const ;
    bool IsArray() const ; 
    bool IsMap() const ;
    
    //Возврат в зависимости от типа данных
    const Array& AsArray() const ;
    const Dict& AsMap() const ;
    bool AsBool() const;
    int AsInt() const ;
    double AsDouble() const;
    const std::string& AsString() const;
 
//операторы
    bool operator==( const Node& rhs) const {
        return GetValue() == rhs.GetValue();
    }        
    bool operator!=(const Node& rhs) const {
        return GetValue() != rhs.GetValue();
    }      
private:
    Value value_ = nullptr;
    
}; //node end

//Класс JSON-документа
class Document {
public:
    explicit Document(Node root);
    const Node& GetRoot() const;

    bool operator==( [[maybe_unused]]const Document& rhs) const {
        return GetRoot() == rhs.GetRoot();
    }        
    bool operator!=([[maybe_unused]]const Document& rhs) const {
     return GetRoot() != rhs.GetRoot();
    }    
private:
    Node root_;
};

Document Load(std::istream& input);

void Print(const Document& doc, std::ostream& output);

}  // namespace json


