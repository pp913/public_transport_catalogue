#pragma once
#include "geo.h"
#include <string>
#include <vector>


//Автобусные маршруты и остановки. 
namespace tc {

    struct Stop {                      
        std::string name;
        geo::Coordinates xy;
    };
    
    struct Route {                       
        bool is_ringed = false;
        std::string name;
        std::vector<tc::Stop*> routes_stops;
        
    };

    struct RouteData {
        size_t stop_count;
        size_t unique_count;
        double length;
        double curvature;
    };
    
    struct pair_hash {
        std::size_t operator()(std::pair<const tc::Stop*, const tc::Stop*> const p) const  {
            return (std::hash<const void*>()(p.first)) ^ (std::hash<const void*>()(p.second));
        }
    };   
}