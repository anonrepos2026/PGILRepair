#ifndef _RSPROP_HH_
#define _RSPROP_HH_

#include "Computation.hh"
#include "ECBase.hh"
#include "ECDAG.hh"

#define RSPROP_N_MAX 32

using namespace std;

class RSPROP : public ECBase {
  private:
    int _m;
    int _encode_matrix[RSPROP_N_MAX * RSPROP_N_MAX];

    void generate_matrix(int* matrix, int rows, int cols, int w);
  public:
  RSPROP(int n, int k, int w, int opt, vector<string> param);
    ECDAG* Encode();
    ECDAG* Decode(vector<int> from, vector<int> to);
    ECDAG* PipeDecode(vector<vector<int>> from, vector<int> to);
    ECDAG* PivotDecode(vector<vector<int>> from, vector<int> to);
    void Place(vector<vector<int>>& group);
};

#endif
