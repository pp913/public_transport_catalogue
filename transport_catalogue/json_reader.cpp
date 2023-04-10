#include "json_reader.h"


//Наполнение транспортного справочника данными из JSON,

using namespace std::literals;

void JSONReader::SetCatalogue() {
    
    auto bases = document_.at("base_requests");

    for (auto base : bases.AsArray()){
        if (base.AsMap().at("type").AsString() == "Stop"){
            catalogue.AddStop(base.AsMap().at("name").AsString(), 
                              base.AsMap().at("latitude").AsDouble(), 
                              base.AsMap().at("longitude").AsDouble());
        }
    }

    for (auto base : bases.AsArray()){
        if (base.AsMap().at("type").AsString() == "Stop"){
            
            for (auto distance : base.AsMap().at("road_distances").AsMap()){
                catalogue.SetDistance(catalogue.GetPtrStop(base.AsMap().at("name").AsString()), 
                                  catalogue.GetPtrStop(distance.first), 
                                  distance.second.AsDouble());
            } 
        }
    }    
    
    for (auto base : bases.AsArray()){
        if (base.AsMap().at("type").AsString() == "Bus"){
            
            bool ringed = false;
            if (base.AsMap().at("is_roundtrip").AsBool() == true) ringed = true;
            
            std::vector<const tc::Stop*> vec_ptr;
            
             for (auto stop : base.AsMap().at("stops").AsArray()){
                 
                  vec_ptr.push_back(catalogue.GetPtrStop(stop.AsString()));
                 
             }
            
            catalogue.AddRoute(ringed, base.AsMap().at("name").AsString(), vec_ptr);
            
        }
    }        
}



void JSONReader::SetRenderer() {
//теперь заполняем рендерер       
    auto render_settings = document_.at("render_settings"); 
   
    //потом надо сделать сеттеры.
    renderer.SetWidth(render_settings.AsMap().at("width").AsDouble());
    renderer.SetHeight(render_settings.AsMap().at("height").AsDouble());
    renderer.SetLineWidth(render_settings.AsMap().at("line_width").AsDouble());
    renderer.SetPadding(render_settings.AsMap().at("padding").AsDouble());

    renderer.SetUnderlayerWidth(render_settings.AsMap().at("underlayer_width").AsDouble());

    //текст остановок
    renderer.SetBusLabeldx(render_settings.AsMap().at("bus_label_offset").AsArray()[0].AsDouble());
    renderer.SetBusLabeldy(render_settings.AsMap().at("bus_label_offset").AsArray()[1].AsDouble());    
    renderer.SetBusFontSize(render_settings.AsMap().at("bus_label_font_size").AsDouble());
    
    renderer.SetStopRadius(render_settings.AsMap().at("stop_radius").AsDouble());
    renderer.SetStopFontSize(render_settings.AsMap().at("stop_label_font_size").AsDouble());
    renderer.SetStopLabeldx(render_settings.AsMap().at("stop_label_offset").AsArray()[0].AsDouble());
    renderer.SetStopLabeldy(render_settings.AsMap().at("stop_label_offset").AsArray()[1].AsDouble()); 
    
    //филлер
    if(render_settings.AsMap().at("underlayer_color").IsString()) {
        renderer.SetUnderlayerColor(render_settings.AsMap().at("underlayer_color").AsString());
    }
    
    //rgb
        if(render_settings.AsMap().at("underlayer_color").IsArray() && render_settings.AsMap().at("underlayer_color").AsArray().size() == 3 ) {
        
        std::string rgb = "rgb(";
            
        rgb += std::to_string(render_settings.AsMap().at("underlayer_color").AsArray()[0].AsInt());
        rgb += ",";
        rgb += std::to_string(render_settings.AsMap().at("underlayer_color").AsArray()[1].AsInt());
        rgb += ",";    
        rgb += std::to_string(render_settings.AsMap().at("underlayer_color").AsArray()[2].AsInt());
        rgb += ")";
        renderer.SetUnderlayerColor(rgb);
    }    
    
    //rgba
    if(render_settings.AsMap().at("underlayer_color").IsArray() && render_settings.AsMap().at("underlayer_color").AsArray().size() == 4 ) {
        
        std::string rgb = "rgba(";
        
        rgb += std::to_string(render_settings.AsMap().at("underlayer_color").AsArray()[0].AsInt());
        rgb += ",";
        rgb += std::to_string(render_settings.AsMap().at("underlayer_color").AsArray()[1].AsInt());
        rgb += ",";    
        rgb += std::to_string(render_settings.AsMap().at("underlayer_color").AsArray()[2].AsInt());
        rgb += ",";        
        std::ostringstream f3;
            f3 << render_settings.AsMap().at("underlayer_color").AsArray()[3].AsDouble();
        rgb += f3.str();
        rgb += ")";
        renderer.SetUnderlayerColor(rgb);
    }    
    
    //палитра
    //аналогично переводим цвет в строковой формат
    for (auto& color_type : render_settings.AsMap().at("color_palette").AsArray()) {
       
        //string
        if(color_type.IsString()) {
             renderer.AddToPallete(color_type.AsString());
        }
    
        //rgb
            if(color_type.IsArray() && color_type.AsArray().size() == 3 ) {
        
            std::string rgb = "rgb(";
        
            rgb += std::to_string(color_type.AsArray()[0].AsInt());
            rgb += ",";
            rgb += std::to_string(color_type.AsArray()[1].AsInt());
            rgb += ",";    
            rgb += std::to_string(color_type.AsArray()[2].AsInt());
            rgb += ")";
             renderer.AddToPallete(rgb);
        
        }    

        //rgba
        if(color_type.IsArray() && color_type.AsArray().size() == 4 ) {
        
            std::string rgb = "rgba(";
        
            rgb += std::to_string(color_type.AsArray()[0].AsInt());
            rgb += ",";
            rgb += std::to_string(color_type.AsArray()[1].AsInt());
            rgb += ",";    
            rgb += std::to_string(color_type.AsArray()[2].AsInt());
            rgb += ",";
            std::ostringstream f3;
                f3 << color_type.AsArray()[3].AsDouble();
            rgb += f3.str();
            rgb += ")";
            renderer.AddToPallete(rgb);
        }  
 
    }
}



void JSONReader::ReadStats() {
    //запросы к справочнику
    //Обработка stat_request
    auto stats = document_.at("stat_requests"); 

    
    std::vector<json::Node> core_array; // массив основного узла json.out

    
    for (auto stat : stats.AsArray()) {
        std::map<std::string, json::Node> element; // элемент-словарь массива основного узла
 
        if (stat.AsMap().at("type").AsString() == "Bus"){
        
            auto route_name = catalogue.FindRoute(stat.AsMap().at("name").AsString()).name;
            if (route_name.empty()) {
                std::string a1 = "not found";    
                element["request_id"] = stat.AsMap().at("id").AsInt();
                element["error_message"] = a1;

            } else {

            tc::RouteData info = catalogue.RouteInfo(stat.AsMap().at("name").AsString());
                element["curvature"] = info.curvature;
                element["request_id"] = stat.AsMap().at("id").AsInt()  ;
                element["route_length"] =  info.length ;
                element["stop_count"] =  static_cast<int> (info.stop_count) ;
                element["unique_stop_count"] = static_cast<int> ( info.unique_count) ;
                    
            }    
        }

        if (stat.AsMap().at("type").AsString() == "Stop"){

            auto stop_name = catalogue.FindStop(stat.AsMap().at("name").AsString()).name;

            auto routes_list = catalogue.GetRoutesListForStop(stat.AsMap().at("name").AsString()); ///!

            if (catalogue.GetPtrStop(stat.AsMap().at("name").AsString()) == nullptr) {
                std::string a1 = "not found";
                element["request_id"] = stat.AsMap().at("id").AsInt();
                element["error_message"] = a1;
            } else {
                std::set<std::string> sorted;
                for (auto& route : routes_list) {
                    sorted.insert(route->name);
                }
                
                std::vector<json::Node> buses_array;
                    for (auto name : sorted) {
                        json::Node node1(name);
                        buses_array.push_back(node1);
                    }
                    element["request_id"] = stat.AsMap().at("id").AsInt();
                    element["buses"] = buses_array;
                }

        }
        
        if (stat.AsMap().at("type").AsString() == "Map"){
            std::ostringstream stream1;
            renderer.Render(catalogue.GetRoutesNames(), stream1);
            std::string str = stream1.str();
            element["request_id"] = stat.AsMap().at("id").AsInt();;
            element["map"] = str;
        }
        
    //узел с запросом
        json::Node element_node(element);
        core_array.push_back(element_node);    
        
    }
    
	//json::Node core_node(core_array);
	//json::Document doc(core_node);

    json::Document doc(
        json::Builder{}
            .Value(core_array)
        .Build()
    );

	json::Print(doc, std::cout);
    
    //stat_request end

}    