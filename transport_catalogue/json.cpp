#include "json.h"

using namespace std;
using namespace std::literals;
namespace json {

    
    
Node::Node(Array value) 
        : value_(std::move(value)) {}   
Node::Node(std::nullptr_t value) 
        : value_(std::move(value)) {}
Node::Node(Dict value)
        : value_(std::move(value)) {}
Node::Node(bool value)
        : value_(std::move(value)) {}
Node::Node(int value)
        : value_(std::move(value)) {}
Node::Node(double value)
        : value_(std::move(value)) {}
Node::Node(std::string value)
        : value_(std::move(value)) {}
    
const Value& Node::GetValue() const {
    return value_;
}
     Value& Node::GetValue() { return  value_; }    
    
bool Node::IsInt() const {
    return std::holds_alternative<int>(value_);
}  

bool Node::IsDouble() const {
    return (std::holds_alternative<double>(value_) || std::holds_alternative<int>(value_));
}      
    
bool Node::IsPureDouble() const {
    return  std::holds_alternative<double>(value_);
}    
bool Node::IsBool() const {
    return std::holds_alternative<bool>(value_);
}      
    
bool Node::IsString() const {
    return std::holds_alternative<std::string>(value_);
}      
    
bool Node::IsNull() const {
    return std::holds_alternative<std::nullptr_t>(value_);
}      
    
bool Node::IsArray() const {
    return std::holds_alternative<Array>(value_);
}  
    
bool Node::IsMap() const {
    return std::holds_alternative<Dict>(value_);
}      
    
    
const Array& Node::AsArray() const {
    if (IsArray()){
        return std::get<Array>(value_); 
    } else {
        throw std::logic_error("");
    }
}

const Dict& Node::AsMap() const {
    if (IsMap()){
        return std::get<Dict>(value_); 
    } else {
        throw std::logic_error("");
    }
}
    
bool Node::AsBool() const{
    if (IsBool()){
        return std::get<bool>(value_); 
    } else {
        throw std::logic_error("");
    }
}
    
int Node::AsInt() const {
    if (IsInt()){
        return std::get<int>(value_); 
    } else {
        throw std::logic_error("");
    }
}
    
double Node::AsDouble() const{
    if (IsPureDouble()){return std::get<double>(value_);
    } else 
        if (IsInt()){
            return std::get<int>(value_) + 0;
        }
    throw std::logic_error("");
}
    
    
const std::string& Node::AsString() const {
    if (IsString()){
        return std::get<std::string>(value_); 
    } else {
        throw std::logic_error("");
    }
}
    

    
namespace {

using Number = std::variant<int, double>;
    //-
Node LoadNode(istream& input);

Node LoadNull(std::istream& input) {
         //std::cerr << "  null "  << std::endl;
    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    std::string s;
    while (true) {

        std::cerr <<  *it  << std::endl;
        
        if (it == end) break;
        const char ch = *it;
        if (ch == ',' || ch == '}' || ch == ']' || ch == '}' || ch == ' '){
            break;
        }
        

        if (ch == '\n' || ch == '\r' || ch == '\t' || ch == ' ') it++; else {
            // Просто считываем очередной символ и помещаем его в результирующую строку
                s.push_back(ch);  //if () return Node(nullptr);
        }
        ++it;
    }

    if (s == "null"s) return Node(nullptr);
    throw ParsingError("Null parsing error");
} 


Node LoadBool(std::istream& input){

    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    std::string str;
    while (true) {

        if (it == end){
            break;
        }

        const char ch = *it;

        if (ch == ',' || ch == '}' || ch == ']' || ch == '}' || ch == ' '){
            break;
        }

                
        if (ch == '\n' || ch == '\r' || ch == '\t' || ch == ' ') it++;
            // Просто считываем очередной символ и помещаем его в результирующую строку
         else str.push_back(ch);  //if () return Node(nullptr);
            ++it;       
        }

    if (str == "true"s) {//std::cerr << "  bool - " << true << std::endl;
        return Node(true);
    } else if (str == "false"s) {//std::cerr << "  bool - " << false << std::endl;
        return Node(false);
    }
    throw ParsingError("Bool parsing error");
}    

    
    
Number LoadNumber(std::istream& input) {
    using namespace std::literals;

    std::string parsed_num;

    // Считывает в parsed_num очередной символ из input
    auto read_char = [&parsed_num, &input] {
        parsed_num += static_cast<char>(input.get());
        if (!input) {
            throw ParsingError("Failed to read number from stream"s);
        }
    };

    // Считывает одну или более цифр в parsed_num из input
    auto read_digits = [&input, read_char] {
        if (!std::isdigit(input.peek())) {
            throw ParsingError("A digit is expected"s);
        }
        while (std::isdigit(input.peek())) {
            read_char();
        }
    };

    if (input.peek() == '-') {
        read_char();
    }
    // Парсим целую часть числа
    if (input.peek() == '0') {
        read_char();
        // После 0 в JSON не могут идти другие цифры
    } else {
        read_digits();
    }

    bool is_int = true;
    // Парсим дробную часть числа
    if (input.peek() == '.') {
        read_char();
        read_digits();
        is_int = false;
    }

    // Парсим экспоненциальную часть числа
    if (int ch = input.peek(); ch == 'e' || ch == 'E') {
        read_char();
        if (ch = input.peek(); ch == '+' || ch == '-') {
            read_char();
        }
        read_digits();
        is_int = false;
    }

    try {
        if (is_int) {
            // Сначала пробуем преобразовать строку в int
            try {
                return std::stoi(parsed_num);
            } catch (...) {
                // В случае неудачи, например, при переполнении,
                // код ниже попробует преобразовать строку в double
            }
        }
        //std::cerr << "  num " << parsed_num << std::endl;
        return std::stod(parsed_num);
    } catch (...) {
        throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
    }
}
    



Node LoadString(std::istream& input) {
    using namespace std::literals;

    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    std::string s;

    while (true) {
        if (it == end) {
            // Поток закончился до того, как встретили закрывающую кавычку?
            throw ParsingError("String parsing error");
        }
        const char ch = *it;
        if (ch == '"') {
            // Встретили закрывающую кавычку
            ++it;
            break;
        } else if (ch == '\\') {
            // Встретили начало escape-последовательности
            ++it;
            if (it == end) {
                // Поток завершился сразу после символа обратной косой черты
                throw ParsingError("String parsing error");
            }
            const char escaped_char = *(it);
            // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
            switch (escaped_char) {
                case 'n': s.push_back('\n'); break;
                case 't': s.push_back('\t'); break;
                case 'r': s.push_back('\r'); break;
                case '"': s.push_back('"'); break;
                case '\\': s.push_back('\\'); break;
                // Встретили неизвестную escape-последовательность
                default:throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
            }
        } else if (ch == '\n' || ch == '\r') {
            // Строковый литерал внутри- JSON не может прерываться символами \r или \n
            throw ParsingError("Unexpected end of line"s);
        } else {
            // Просто считываем очередной символ и помещаем его в результирующую строку
            s.push_back(ch);
        }
        ++it;
    }
//std::cerr << "  str - " << s << std::endl;
    return Node(move(s));
}
    
    


Node LoadArray(istream& input) {
    Array result;
    for (char c; input >> c && c != ']';) {
     //std::cerr << "  array " << c << std::endl;
        if (c != ',') {
            input.putback(c);
        }
        result.push_back(LoadNode(input));
        if (input >> c) {        
            if (c == ']') {
                break;
            } else 
                if (c != ',') { 
                    std::cerr << "ERR" << c << std::endl; 
                    throw ParsingError("array error: no ,"); 
                }// нет запятой
        } else {
            throw ParsingError("array no symb"); // Кончились символы
        }
    }
    if (input) {
        return Node(move(result));
    } else 
        throw ParsingError("array no end"); // нет закрывающей скобки
}    
    

   
Node LoadDict(std::istream& input) {
    Dict result;
    for (char c; input >> c && c != '}';) {
    if (c == '"') {
        std::string key = LoadString(input).AsString();
        if (result.count(key) > 0) {
            throw ParsingError("map parsing error: double key");
        } else 
        if (input >> c && c != ':') throw ParsingError("map parsing error: no :"); // нет двоеточия
        result[move(key)] = LoadNode(input);
    } else 
        if (c != ',' ) throw ParsingError("map parsing error: no ,"); // нет запятых
    }
    if (input) return Node{ std::move(result)}; 
    else throw ParsingError("map parsing error no end"); // нет закрывающей скобки
}  
    

Node LoadNode(istream& input) {
    char c;
    input >> c;
    if (c == '[') return LoadArray(input); else 
    if (c == '{') return LoadDict(input); else 
    if (c == '"') return LoadString(input); else 
    if (c == 't' || c == 'f'){
        input.putback(c);
        return LoadBool(input);
    } else 
    if (c == 'n') {
        input.putback(c);
        return LoadNull(input);
    } else {
        input.putback(c);
        auto num = LoadNumber(input);
        if (std::holds_alternative<int>(num)) return Node{std::get<int>(num)}; 
        else return Node{ std::get<double>(num) };
    }
}


    
struct visitor {
    std::ostream& out1;    
    void operator()([[maybe_unused]]std::nullptr_t a) const {
        out1 << "null" ;
    }

    void operator()([[maybe_unused]]Array a) const { //fix
        bool delimiter = false;
        out1 << '[';
        for (const auto& i : a){
            if (delimiter) out1 << ", "s;
            std::visit(visitor{out1}, i.GetValue());
            delimiter = true;
        }
        out1 << ']';
    }
     

    void operator()(Dict a) const {
        bool delimiter = false;
        out1 << '{';
        for (const auto& [key, value] : a){
            if (delimiter) out1 << ", "s;
            std::visit(visitor{out1}, Node{key}.GetValue());
            out1 << ':';
            std::visit(visitor{out1}, Node{value}.GetValue());
            delimiter = true;
        }
        out1 << '}';
    }

    void operator()(bool a) const {
        if (a == true) out1 << "true"; 
            else out1 << "false" ;
    }

    void operator()([[maybe_unused]] int a) const {
        out1 << a ;
    } 

    void operator()([[maybe_unused]]double a) const {
        out1 << a ;
    }   

    void operator()([[maybe_unused]]std::string a) const {
        out1 << "\""sv;
        for (const char c : a){
            switch (c){
                case '\r': out1 << "\\r"sv; break;
                case '"': out1 << '\\' << c; break;
                case '\t': out1 << "\t"sv; break;
                case '\\':out1 << '\\' << c; break;
                case '\n': out1 << "\\n"sv; break;
                default: out1 << c; break;
            }
        }
        out1 << "\""sv;
    }

    
}; // visitor end          
    
}  // namespace end


//-----------------------------------Node-----------------------------------------//

    
//-----------------------------------Document-----------------------------------------//
    
Document::Document(Node root)
    : root_(move(root)) {
}

const Node& Document::GetRoot() const {
    return root_;
}

//-----------------------------------Other-----------------------------------------//   

Document Load(istream& input) {
    return Document{LoadNode(input)};
}

void Print(const Document& doc, std::ostream& output) {
    std::visit(visitor{output}, doc.GetRoot().GetValue()); 
}

    
}  // namespace json