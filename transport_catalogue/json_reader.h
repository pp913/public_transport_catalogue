#pragma once


#include "request_handler.h"
#include "transport_catalogue.h"
#include "json.h"
#include "json_builder.h"
#include "map_renderer.h"

#include <set>



class JSONReader {

    public:
    JSONReader(tc::TransportСatalogue& catalogue_, renderer::MapRenderer& renderer_) : catalogue(catalogue_), renderer(renderer_) {}
    
    //Функции для прохода по справочнику.
    void SetCatalogue();
    void SetRenderer();
    void ReadStats();
    
    
    
    void ReadJSON( std::istream& in){
        document_ = json::Load(in).GetRoot().AsMap();
    }
    
    private:
    
    tc::TransportСatalogue& catalogue;
    renderer::MapRenderer& renderer;
    std::map<std::string, json::Node> document_;
    
};
