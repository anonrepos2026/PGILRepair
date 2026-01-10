#include "RSPROP.hh"
//should add the uplink/downlink BW constraints
//TODO 1 : helper selection considering the asymmetric bandwidth
//TODO 2 : PIELINE construction

RSPROP::RSPROP(int n, int k, int w, int opt, vector<string> param) {
  _n = n;
  _k = k;
  _w = w;
  _opt = opt;

  _m = _n - _k;
  memset(_encode_matrix, 0, (_k+_m)*_m*sizeof(int));
}

ECDAG* RSPROP::Encode() {
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

ECDAG* RSPROP::Decode(vector<int> from, vector<int> to) {
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
ECDAG* RSPROP::PipeDecode(vector<vector<int>> from, vector<int> to) {
  ECDAG* ecdag = new ECDAG();

  generate_matrix(_encode_matrix, _n, _k, 8);

  int i, j, k, h,l; //Temporary int var.
  
    int _k_cnt = 0;
    int _select_matrix[_k*_k];

    for (i=0; i< from.size(); i++) 
    {
      for(j = from[i].size()-1 ; j >= 0 ; j--)
      {
        int sidx = from[i][j];
        memcpy(_select_matrix + _k_cnt * _k,
              _encode_matrix + sidx * _k,
        sizeof(int) * _k);
        _k_cnt++;
      }
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
      // prepare data and coef
      deque<int> dataqueue;
      deque<int> coefqueue;
      //IMPORTANT: Reversed ordering
      for (j=0; j<from.size(); j++) 
      {

        for(k=from[j].size()-1; k >= 0  ; k--)
        {
          dataqueue.push_back(from[j][k]);
          coefqueue.push_back(_coef_vector[_k_cnt]);
          _k_cnt++;
        }
      }

	int imidx = _k + _m;
	int toadd;
        //Check the ordering (first depth, first in)
        for (j = from.size()-1; j >= 0 ; j--) 
        {
          vector<int> datav;
          vector<int> coefv;
	  int _k_leaf_num = (j==0) ? from[j].size() : from[j].size()+1;
          //Note that the highest depth has only leaf nodes, and thus, the exact size should be conisdered. 	
          for(k=0; k < _k_leaf_num ; k++)
          {
            //read the last value by pop back
            int tmpd(dataqueue.back());
            int tmpc(coefqueue.back());
            dataqueue.pop_back();
            coefqueue.pop_back();
            datav.push_back(tmpd);
            coefv.push_back(tmpc);
          }
		
          if (j == 0) 
          {
             //toadd = ridx;
	    //toadd = imidx;
	    ecdag->Join(ridx, datav, coefv);

	   // coefffv.push_back(1);

           // ecdag->Join(ridx, dataffv, coefffv);
          } 
          else 
          {  
	    //datav.push_back(from[j-1][0]);
            //coefv.push_back(1);
            
            toadd = imidx++; 
            ecdag->Join(toadd, datav, coefv);
            ecdag->BindY(toadd, datav[datav.size()-1]);  //Before:  ecdag->BindY(toadd, datav[datav.size()-1]);
          }


          
          dataqueue.push_back(toadd);
          coefqueue.push_back(1);  // Accumulated results may be considered as 1*cumul+(Others)
        }
      }
      //Its okay to make a combined graph? It would be not..
      //Depreciated(original codes)
      /*
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
  
  */
  return ecdag;
} 


//Pipedecode
ECDAG* RSPROP::PivotDecode(vector<vector<int>> from, vector<int> to) {
} 



void RSPROP::Place(vector<vector<int>>& group){}

void RSPROP::generate_matrix(int* matrix, int rows, int cols, int w) {
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

