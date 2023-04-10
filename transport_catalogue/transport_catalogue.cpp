#include "transport_catalogue.h"

void tc::TransportСatalogue::AddStop(std::string_view name, const double x, const double y) {
    stops_.push_back({std::string(name),{x,y}});
    stopname_to_stop_.emplace(stops_.back().name, & stops_.back());
    routes_list_1[ & stops_.back()];
}

void tc::TransportСatalogue::AddRoute(const bool ringed, const std::string& given_name, const std::vector <const Stop*> stops) {
    Route route;
    route.is_ringed = ringed;
    route.name = given_name;
    for (auto & stop: stops) {
        route.routes_stops.push_back(const_cast <Stop*> (stop));
    }
    routes_.push_back(route);
    routename_to_route_.emplace(routes_.back().name, &routes_.back());
    for (auto& stop: stops) {
        routes_list_1[GetPtrStop(stop -> name)].insert(GetPtrRoute(given_name));
    }
}

const tc::Stop* tc::TransportСatalogue::GetPtrStop(const std::string& name) const {
    if (stopname_to_stop_.count(name) == 0) {
        return nullptr; //throw std::invalid_argument("No such route");
    } else {
        return stopname_to_stop_.at(name);
    }
}

const tc::Route * tc::TransportСatalogue::GetPtrRoute(const std::string& name) const {
    if (routename_to_route_.count(name) == 0) {
        return nullptr; //throw std::invalid_argument("No such route");
    } else {
        return routename_to_route_.at(name);
    }
}

const tc::Route tc::TransportСatalogue::FindRoute(const std::string& name) const {
    if (routename_to_route_.count(name) == 0) {
        return {};
    } else {
        return *routename_to_route_.at(name);
    }
}

const tc::Stop tc::TransportСatalogue::FindStop(const std::string& name) const {

    if (stopname_to_stop_.count(name) == 0) {
        return {};
    } else {

        return *stopname_to_stop_.at(name);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const tc::RouteData tc::TransportСatalogue::RouteInfo(const std::string& route) const {
    tc::RouteData info = {};
    if (routename_to_route_.count(route) == 0) return info; //проверка на пустоту
    auto route_ptr = routename_to_route_.at(route);
    double geo_length = 0.0; //географическое расстояние

    for (long unsigned int i = 0; i < route_ptr -> routes_stops.size() - 1; i++) {
        geo_length += ComputeDistance(route_ptr -> routes_stops[i] -> xy,
            route_ptr -> routes_stops[i + 1] -> xy);
    }

    size_t stop_count = 0;
    //Поправка для некольцевого маршрута
    if (!route_ptr -> is_ringed) {
        geo_length *= 2;
        stop_count = route_ptr -> routes_stops.size() * 2 - 1;
    } else {
        stop_count = route_ptr -> routes_stops.size();
    }

    std::unordered_set <std::string_view> unique_stops;
    //Уникальные остановки
    for (auto stop: route_ptr -> routes_stops) {
        unique_stops.emplace(stop -> name);
    }

    double fact_length = 0; //факт. длина маршрута

    if (route_ptr -> is_ringed) {
        //Фактическая длина кольцевого маршрута
        for (size_t i = 0; i < route_ptr -> routes_stops.size() - 1; i++) {
            auto a = route_ptr -> routes_stops[i];
            auto b = route_ptr -> routes_stops[i + 1];
            fact_length += GetDistance(a, b);
        }
    } else {
        //Фактическая длина некольцевого маршрута
        for (size_t i = 0; i < route_ptr -> routes_stops.size() - 1; i++) {
            auto a = route_ptr -> routes_stops[i];
            auto b = route_ptr -> routes_stops[i + 1];
            fact_length += GetDistance(a, b);
        }
        for (size_t i = route_ptr -> routes_stops.size() - 1; i > 0; i--) {
            auto a = route_ptr -> routes_stops[i];
            auto b = route_ptr -> routes_stops[i - 1];
            fact_length += GetDistance(a, b);
        }
    }
    return {
        stop_count,
        unique_stops.size(),
        fact_length,
        fact_length / geo_length
    };
}

const std::unordered_set <const tc::Route*> &tc::TransportСatalogue::GetRoutesListForStop(const std::string& name) const {
        if (GetPtrStop(name) == nullptr) { //throw std::invalid_argument("No such stop"); 
            static std::unordered_set <
                const tc::Route * > a;
            return a;
        }
        return routes_list_1.at(GetPtrStop(name));
    }

void tc::TransportСatalogue::SetDistance(const Stop* from,
    const Stop* to, double distance) {
    distances_[{from, to}] = distance;
}

double tc::TransportСatalogue::GetDistance(const Stop* from, const Stop* to) const {
    std::pair <const tc::Stop* , const tc::Stop*> a;
    a.first = from;
    a.second = to;

    if (distances_.count(a) == 0) {
        a.first = to;
        a.second = from;
        if (distances_.count(a) == 0) {
            return 100000; //return 0
        } else {
            return distances_.at(a);
        }
    } else {
        return distances_.at(a);
    }
}

std::vector <tc::Route*> tc::TransportСatalogue::GetRoutesNames() {
    std::vector <tc::Route* > routes;
    for (auto& route: routes_) {
        routes.push_back(&route);

    }
    std::sort(routes.begin(), routes.end(), [](const tc::Route * left, const tc::Route * right) {
        return left->name < right->name;
    });
    return routes;
}

std::vector <tc::Stop*> tc::TransportСatalogue::GetStopsNames() {
    std::vector <tc::Stop*> stops;
    for (auto& stop: stops_) {
        stops.push_back(&stop);

    }

    std::sort(stops.begin(), stops.end(), [](const tc::Stop* left, const tc::Stop * right) {
        return left->name < right->name;
    });
    return stops;
}