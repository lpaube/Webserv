#pragma once
#include <string>
#include <vector>

class LocationContent
{
public:
    LocationContent();
    ~LocationContent(){};
    std::string path;
    std::vector<std::string> loc_content;
    std::string loc_content_str;

private:
    void VectorizeLocation();
};
