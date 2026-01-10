#include "RSPIVOT.hh"
//should add the uplink/downlink BW constraints
//TODO 1 : helper selection considering the asymmetric bandwidth
//TODO 2 : PIELINE construction

RSPIVOT::RSPIVOT(int n, int k, int w, int opt, vector<string> param) {
  _n = n;
  _k = k;
  _w = w;
  _opt = opt;

  _m = _n - _k;
  memset(_encode_matrix, 0, (_k+_m)*_m*sizeof(int));
}

ECDAG* RSPIVOT::Encode() {
  ECDAG* ecdag = new ECDAG();
  vector<int> data;
  vector<int> code;
  for (int i=0; i<_k; i++) data.push_back(i);
  for (int i=_k; i<_n; i++) code.push_back(i);

  generate_matrix(_encode_matrix, _n, _k, 8);
  for (int i=0; i<_m; i++) {
    vector<int> coef;
    for (int j=0; j<_k; j++) {
      coef.push_back(_encode_matrix[(i+_k)*_k+j]);
    }
    ecdag->Join(code[i], data, coef);
  }
  if (code.size() > 0) ecdag->BindX(code);
  return ecdag;
}

ECDAG* RSPIVOT::Decode(vector<int> from, vector<int> to) {
  ECDAG* ecdag = new ECDAG();


  generate_matrix(_encode_matrix, _n, _k, 8);

  int _select_matrix[_k*_k];
  for (int i=0; i<_k; i++) {
    int sidx = from[i];
    memcpy(_select_matrix + i * _k,
           _encode_matrix + sidx * _k,
	   sizeof(int) * _k);
  }
  
  int _invert_matrix[_k*_k];
  jerasure_invert_matrix(_select_matrix, _invert_matrix, _k, _k);

  int tmpname = _k + _m;

  for (int i=0; i<to.size(); i++) {
    int ridx = to[i];
    int _select_vector[_k];
    memcpy(_select_vector,
           _encode_matrix + ridx * _k,
	   _k * sizeof(int));
    int* _coef_vector = jerasure_matrix_multiply(
             _select_vector, _invert_matrix, 1, _k, _k, _k, 8);
    // prepare data and coef
    deque<int> dataqueue;
    deque<int> coefqueue;
    for (int j=0; j<_k; j++) {
      dataqueue.push_back(from[j]);
      coefqueue.push_back(_coef_vector[j]);
    }

    while(dataqueue.size()>=2) {
      vector<int> datav;
      vector<int> coefv;

      for (int j=0; j<2; j++) {
        int tmpd(dataqueue.front());
	dataqueue.pop_front();
	int tmpc(coefqueue.front());
	coefqueue.pop_front();
	datav.push_back(tmpd);
	coefv.push_back(tmpc);
      }
      int toadd;
      if (dataqueue.size() == 0) {
        toadd = ridx;
      } else {
        toadd = tmpname++;
      }
      ecdag->Join(toadd, datav, coefv);
      ecdag->BindY(toadd, datav[1]);
      dataqueue.push_front(toadd);
      coefqueue.push_front(1);
    }
  }
  return ecdag;
} 

//Pipedecode
ECDAG* RSPIVOT::PipeDecode(vector<vector<int>> from, vector<int> to) {
} 

#include<map>
//Pipedecode
ECDAG* RSPIVOT::PivotDecode(vector<vector<int>> from, vector<int> to) {
  ECDAG* ecdag = new ECDAG();
  //local vector 선언 => 중복이 나왔을 때 memcpy를 하지 않는 방식
  vector<int> helpers;
  unordered_map<int, int> nodeidx;

  for(int i = from.size()-1; i >= 0; i--)
  {
    for(int j = from[i].size()-1; j >= 0; j--)
    {
      if(!(i==0 && j==0) && find(helpers.begin(), helpers.end(), from[i][j]) == helpers.end())
      {
        helpers.push_back(from[i][j]);
        nodeidx[from[i][j]] = helpers.size()-1;
      }
    }
  }

  generate_matrix(_encode_matrix, _n, _k, 8);

  int i, j, k, h,l; //Temporary int var.
  
    int _k_cnt = 0;
    int _select_matrix[_k*_k];

    for (i=0; i<helpers.size(); i++) 
    {
        int sidx = helpers[i];
        memcpy(_select_matrix + _k_cnt * _k,
              _encode_matrix + sidx * _k,
        sizeof(int) * _k);
        _k_cnt++;
      }
    _k_cnt = 0;

    int _invert_matrix[_k*_k];
    jerasure_invert_matrix(_select_matrix, _invert_matrix, _k, _k);

    //Constructing the inverse matrix for repair(coefficient)
    for (i=0; i<to.size(); i++) {
      int ridx = to[i];
      int _select_vector[_k];
      memcpy(_select_vector,
            _encode_matrix + ridx * _k,
      _k * sizeof(int));
      int* _coef_vector = jerasure_matrix_multiply(
              _select_vector, _invert_matrix, 1, _k, _k, _k, 8);
        int imidx = _k + _m;
        int toadd;
        unordered_map<int, int> imidx_map;
        for(j=0;j<helpers.size();j++)
        {
          imidx_map[helpers[j]] = helpers[j];
        }
        //Check the ordering (first depth, first in)
        for (j = from.size()-1; j >= 0 ; j--) 
        {
          vector<int> row = from[j];

          vector<int> datav;
          vector<int> coefv;
          
          for(int h : row)
          {
            if(h==from[0][0]) continue;

            datav.push_back(imidx_map[h]);
            if(h != imidx_map[h])
            {
              coefv.push_back(1);
            }
            else
            {
              coefv.push_back(_coef_vector[nodeidx[h]]);
            }
          }
          if(j == 0)
          {
            toadd = ridx;
            ecdag->Join(toadd, datav, coefv);
          }
          else
          {
            toadd = imidx++;
            imidx_map[datav[0]] = toadd;

            ecdag->Join(toadd, datav, coefv);
            ecdag->BindY(toadd, datav[0]);
          }
        }
      }
  return ecdag;
}

void RSPIVOT::Place(vector<vector<int>>& group){}

void RSPIVOT::generate_matrix(int* matrix, int rows, int cols, int w) {
  int k = cols;
  int n = rows;
  int m = n - k;
  memset(matrix, 0, rows * cols *sizeof(int));
  for(int i=0; i<k; i++) {
    matrix[i*k+i] = 1;
  }
  
  for (int i=0; i<m; i++) {
    int tmp = 1;
    for (int j=0; j<k; j++) {
      matrix[(i+k)*cols+j] = tmp;
      tmp = Computation::singleMulti(tmp, i+1, w);
    }
  }
}

