#ifndef _PEERS_DICT_H
#define _PEERS_DICT_H

#include <map>

struct PeersDict {
    std::map<int, int> tids;
    PeersDict() {};
    PeersDict(const int, const int);
};

extern PeersDict g_pdict;

#endif // for #ifndef _PEERS_DICT_H
