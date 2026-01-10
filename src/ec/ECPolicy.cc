#include "ECPolicy.hh"


//ECPolicy::ECPolicy(string id, string classname, int n, int k, int w, bool locality, int opt, vector<string> param) {
ECPolicy::ECPolicy(string id, string classname, int n, int k, int w, int opt, vector<string> param) {
  _id = id;
  _classname = classname;
  _n = n;
  _k = k;
  _w = w;

//  _locality = locality;
  _opt = opt;
  _param = param;
}

ECPolicy::ECPolicy(string id, string classname, int n, int k, int w, int l, int opt, int* ul_con, int* dl_con, vector<string> param) {
  _id = id;
  _classname = classname;
  _n = n;
  _k = k;
  _w = w;
  _l = l;

//  _locality = locality;
  _opt = opt;
  _param = param;

  for(int i = 0 ; i < n ; i++)
  {
    _ul_cons[i] = ul_con[i];
    _dl_cons[i] = dl_con[i];
  }
}


ECBase* ECPolicy::createECClass() {
  ECBase* toret;
  if (_classname == "BUTTERFLY64") {
//    toret = new BUTTERFLY64(_n, _k, _w, _locality, _opt, _param);
    toret = new BUTTERFLY64(_n, _k, _w, _opt, _param);
  } else if (_classname == "DRC643") {
//    toret = new DRC643(_n, _k, _w, _locality, _opt, _param);
    toret = new DRC643(_n, _k, _w, _opt, _param);
  } else if (_classname == "DRC963") {
//    toret = new DRC963(_n, _k, _w, _locality, _opt, _param);
    toret = new DRC963(_n, _k, _w, _opt, _param);
  } else if (_classname == "IA") {
//    toret = new IA(_n, _k, _w, _locality, _opt, _param);
    toret = new IA(_n, _k, _w, _opt, _param);
  } else if (_classname == "RSCONV") {
//    toret = new RSCONV(_n, _k, _w, _locality, _opt, _param);
    toret = new RSCONV(_n, _k, _w, _opt, _param);
  } else if (_classname == "RSBINDX") {
//    toret = new RSBINDX(_n, _k, _w, _locality, _opt, _param);
    toret = new RSBINDX(_n, _k, _w, _opt, _param);
  } else if (_classname == "RSPIPE") {
//    toret = new RSPIPE(_n, _k, _w, _locality, _opt, _param);
    toret = new RSPIPE(_n, _k, _w, _opt, _param);
  } else if (_classname == "RSPPR") {
//    toret = new RSPPR(_n, _k, _w, _locality, _opt, _param);
    toret = new RSPPR(_n, _k, _w, _opt, _param);
  } else if (_classname == "WASLRC") {
//    toret = new WASLRC(_n, _k, _w, _locality, _opt, _param);
    toret = new WASLRC(_n, _k, _w, _opt, _param);
  } else if (_classname == "RSPROP") {
     toret = new RSPROP(_n, _k, _w, _opt, _param);
  } else if (_classname == "RSPIVOT") {
     toret = new RSPIVOT(_n, _k, _w, _opt, _param);
  } else {
    cout << "unrecognized code, use default RSCONV" << endl;
//    toret = new RSCONV(_n, _k, _w, _locality, _opt, _param);
    toret = new RSCONV(_n, _k, _w, _opt, _param);
  }
  return toret;
}

string ECPolicy::getPolicyId() {
  return _id;
}

int ECPolicy::getN() {
  return _n;
}

int ECPolicy::getK() {
  return _k;
}

int ECPolicy::getW() {
  return _w;
}

int ECPolicy::getl() {
  return _l;
}


bool ECPolicy::getLocality() {
  return _locality;
}

int ECPolicy::getOpt() {
  return _opt;
}

int ECPolicy::getULconst(int i)
{
  return _ul_cons[i];
}
int ECPolicy::getDLconst(int i)
{
  return _dl_cons[i];
}
