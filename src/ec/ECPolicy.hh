#ifndef _ECPOLICY_HH_
#define _ECPOLICY_HH_

#include "BUTTERFLY64.hh"
#include "DRC643.hh"
#include "DRC963.hh"
#include "ECBase.hh"
#include "IA.hh"
#include "RSBINDX.hh"
#include "RSCONV.hh"
#include "RSPIPE.hh"
#include "RSPPR.hh"
#include "RSPIVOT.hh"
#include "RSPROP.hh"
#include "WASLRC.hh"
#include "../inc/include.hh"

#define N_MAX 20 //Maximum number of storage nodes in erasure-coded DSSs
//(UL, DL constraints can be declared with the parameters)
#define BW_MAX 100

using namespace std;

class ECPolicy {
  private:
    string _id;
    string _classname;
    int _n;
    int _k;
    int _w;
    bool _locality = false;
    int _opt;
    int _l; // # of repair path

    int _ul_cons[N_MAX] ={BW_MAX,};
    int _dl_cons[N_MAX]= {BW_MAX,};

    vector<string> _param;
  public:
//    ECPolicy(string id, string classname, int n, int k, int w, bool locality, int opt, vector<string> param);
    ECPolicy(string id, string classname, int n, int k, int w, int opt, vector<string> param);
    ECPolicy(string id, string classname, int n, int k, int w, int l, int opt, int* ul_con, int* dl_con, vector<string> param);
    ECBase* createECClass();
    string getPolicyId();
    int getN();
    int getK();
    int getW();
    bool getLocality();
    int getOpt();
    int getl();

    int getULconst(int i);
    int getDLconst(int i);
};

#endif
