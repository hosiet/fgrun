#ifndef apt_dat_h_included
#define apt_dat_h_included

#include <vector>
#include <set>
#include <string>

/**
 * 
 */
struct apt_dat_t
{
    std::string id_;
    std::string name_;
    std::vector< std::string > runways_;
    std::set< std::string > parking_;
    int type_;
    float lon_;
    float lat_;
};

#endif // apt_dat_h_included
