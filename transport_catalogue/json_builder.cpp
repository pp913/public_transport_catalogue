#include "json_builder.h"

using Dict = std::map<std::string, json::Node>;
using Array = std::vector<json::Node>;
using Value =
    std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;
using namespace std::literals;

namespace json {

struct ValueSolution {

    json::Node operator()(std::nullptr_t a) const {
        return json::Node(a);
    }
    json::Node operator()(Array a) const {
        return json::Node(a);
    }
    json::Node operator()(Dict a) const {
        return json::Node(a);
    }
    json::Node operator()(bool a) const {
        return json::Node(a);
    }
    json::Node operator()(int a) const {
        return json::Node(a);
    }
    json::Node operator()(double a) const {
        return json::Node(a);
    }
    json::Node operator()(std::string a) const {
        return json::Node(a);
    }
};

Builder::KeyItemContext Builder::Key(std::string s) {
    if (last_chain_ == "key" ||
            !(std::holds_alternative<Dict>(nodes_stack_.back()->GetValue()))) {
        throw std::logic_error("Key after key"s);
    }

    last_key_.push_back(s);

    last_chain_ = "key";
    return KeyItemContext(*this);
}

Builder& Builder::Value(json::Value a) {
    if (!(last_chain_ == "none" || last_chain_ == "key" ||
            (std::holds_alternative<Array>(nodes_stack_.back()->GetValue())))) {
        throw std::logic_error("not after key value or array element"s);
    }

    json::Node* node = nullptr;

    node = new json::Node(std::visit(ValueSolution{}, a));

    if (last_chain_ == "none") {
        nodes_stack_.push_back(node);
        root_ = *node;
    }

    if (std::holds_alternative<Dict>(nodes_stack_.back()->GetValue())) {
        std::get<json::Dict>(nodes_stack_.back()->GetValue())[last_key_.back()] =
            *node;  

    } else

        if (std::holds_alternative<Array>(nodes_stack_.back()->GetValue())) {
            std::get<json::Array>(nodes_stack_.back()->GetValue())
            .push_back(*node);  
        }

    last_chain_ = "value";
    return *this;
}

Builder::DictItemContext Builder::StartDict() {
    if (!(last_chain_ == "none" || last_chain_ == "key" ||
            (std::holds_alternative<Array>(nodes_stack_.back()->GetValue())))) {
        throw std::logic_error("not after key value or array element"s);
    }

    if (last_chain_ == "none") {  //начальный узел

        root_ = json::Node(Dict{});

        nodes_stack_.push_back(&root_);

        last_chain_ = "start_dict";
        return *this;

    } else {
        if (std::holds_alternative<Dict>(nodes_stack_.back()->GetValue())) {
            Node& node =
                std::get<Dict>(nodes_stack_.back()->GetValue())[last_key_.back()] =
                    Dict{};
            nodes_stack_.push_back(&node);
        } else

            if (std::holds_alternative<Array>(nodes_stack_.back()->GetValue())) {
                Node& node =
                    std::get<Array>(nodes_stack_.back()->GetValue()).emplace_back(Dict{});
                nodes_stack_.push_back(&node);
            }

        last_chain_ = "start_dict";
        return DictItemContext(*this);
    }
}


Builder::ArrItemContext Builder::StartArray() {
    if (!(last_chain_ == "none" || last_chain_ == "key" ||
            (std::holds_alternative<Array>(nodes_stack_.back()->GetValue())))) {
        throw std::logic_error("not after key value or array element"s);
    }

    if (last_chain_ == "none") {  //начальный узел

        // Node& node
        root_ = json::Node(Array{});

        nodes_stack_.push_back(&root_);

        last_chain_ = "start_arr";
        return *this;

    } else {
        if (std::holds_alternative<Dict>(nodes_stack_.back()->GetValue())) {
            Node& node =
                std::get<Dict>(nodes_stack_.back()->GetValue())[last_key_.back()] =
                    Array{};
            nodes_stack_.push_back(&node);
        } else

            if (std::holds_alternative<Array>(nodes_stack_.back()->GetValue())) {
                Node& node = std::get<Array>(nodes_stack_.back()->GetValue())
                             .emplace_back(Array{});
                nodes_stack_.push_back(&node);
            }

        last_chain_ = "start_arr";
        return ArrItemContext(*this);
    }
}


Builder& Builder::EndDict() {

    if (!(std::holds_alternative<Dict>(nodes_stack_.back()->GetValue()))) {
        throw std::logic_error("End of another container"s);
    }

    if (last_chain_ == "start_dict") {
        nodes_stack_.pop_back();
        return *this;
    }

    last_key_.pop_back();
    nodes_stack_.pop_back();

    last_chain_ = "end_dict";
    return *this;
}


Builder& Builder::EndArray() {

    if (!(std::holds_alternative<Array>(nodes_stack_.back()->GetValue()))) {
        throw std::logic_error("End of another container"s);
    }

    if (last_chain_ == "start_arr") {
        nodes_stack_.pop_back();
        return *this;
    }

    nodes_stack_.pop_back();

    last_chain_ = "end_arr";
    return *this;
}


json::Node Builder::Build() {
    if (last_chain_ == "none") {
        throw std::logic_error("Build after construct"s);
    }

    // root_ = *nodes_stack_.front();
    return root_;
}
}