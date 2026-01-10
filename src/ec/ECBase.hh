#ifndef _ECBASE_HH_
#define _ECBASE_HH_

#include "../inc/include.hh"

#include "ECDAG.hh"

using namespace std;

class ECBase {
  public:
    int _n, _k, _w;
    //bool _locality;
    int _opt;

    ECBase();
    ECBase(int n, int k, int w, int opt, vector<string> param);
    
    virtual ECDAG* Encode() = 0;
    virtual ECDAG* Decode(vector<int> from, vector<int> to) = 0;
    // Only for PG-IL(or the other specific ECDAG structure), 
    // two-dimensional vector structure by graph depth
    // First vector struct -> first depth
    virtual ECDAG* PipeDecode(vector<vector<int>> from, vector<int> to) = 0;
    virtual ECDAG* PivotDecode(vector<vector<int>> from, vector<int> to) = 0;
    virtual void Place(vector<vector<int>>& group) = 0;
};

#endif
