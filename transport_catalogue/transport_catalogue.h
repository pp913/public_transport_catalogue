#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <deque>
#include <algorithm>

#include "geo.h" //Координаты
#include "domain.h" //Маршруты и остановки

namespace tc {


    //Класс транспортного справочника.
class TransportСatalogue {
public:    
   void AddStop(std::string_view name, const double x, const double y);
   void AddRoute(const bool ringed, const std::string& given_name, const std::vector<const Stop*> stops);
   const Stop* GetPtrStop(const std::string& name) const;
   const Route* GetPtrRoute(const std::string& name) const;
    
   const Route FindRoute(const std::string& name) const;
   const Stop FindStop(const std::string& name) const;
    
    //для вывода информации о маршруте
   const tc::RouteData RouteInfo(const std::string& route) const;       
    
    //Для вывода остановок
    const std::unordered_set<const tc::Route*>& GetRoutesListForStop(const std::string& name) const;
    
    //Для добавления расстояний
    void SetDistance(const Stop* from, const Stop* to, double distance);
    
    //Для получения расстояний
    double GetDistance(const Stop* from, const Stop* to) const;
    
    std::vector<Route*> GetRoutesNames();
    std::vector<Stop*> GetStopsNames();
    
private:
    std::deque<Route> routes_;
    std::deque<Stop> stops_;
    
    std::unordered_map<std::string_view, Route*> routename_to_route_;
    std::unordered_map<std::string_view, Stop*> stopname_to_stop_;
    
    //для вывода остановок
    std::unordered_map<const tc::Stop*, std::unordered_set<const tc::Route*>> routes_list_1;
    
    //Для хранения расстояний
    std::unordered_map<std::pair<const tc::Stop*, const tc::Stop*>, double, pair_hash> distances_;
    
};


} // TC end