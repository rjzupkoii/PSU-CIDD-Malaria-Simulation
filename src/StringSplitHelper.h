//
// Created by Nguyen Tran on 1/26/2018.
//

#ifndef POMS_STRINGSPLITHELPER_H
#define POMS_STRINGSPLITHELPER_H

std::vector <std::string> &split(const std::string &s, char delim, std::vector <std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        boost::algorithm::trim(item);
        elems.push_back(item);
    }
    return elems;
}

std::vector <std::string> split(const std::string &s, char delim) {
    std::vector <std::string> elems;
    return split(s, delim, elems);
}

#endif //POMS_STRINGSPLITHELPER_H
