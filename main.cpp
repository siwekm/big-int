#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cstdint>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <functional>
#include <stdexcept>
using namespace std;

class CBigInt
{
  public:
    // default constructor
    CBigInt(){
      neg = false;
      number.push_back(0);
    }
    // int constructor
    CBigInt(const int n){
      int x = n;
      if(x<0){
        neg = true;
      }else{
        neg = false;
      }
      if(x==0){
        number.insert(number.begin(), 0);
        return;
      }
      while(x!=0){
        number.insert(number.begin(), abs(x%10));
        x/=10;
      }
    }

    CBigInt(const char *x){
      int i=0;
      if(x[0]=='-'){
        neg = true;
        i++;
      }else{
        neg = false;
      }
      for(; (long unsigned int)i < strlen(x); i++){
        if(x[i] >= '0' && x[i] <= '9'){
          number.push_back(x[i]-'0');
        }else{
          throw std::invalid_argument("Not a digit");
        }
      }
      removeZeros();
    }
    CBigInt& operator= (const CBigInt& other){
      number = other.number;
      neg = other.neg;
      return *this;
      }
    CBigInt& operator+= (const CBigInt& other){add(other); return *this;}
    CBigInt& operator*= (const CBigInt& other){multiply(other); return *this;}

    friend CBigInt operator+ (const CBigInt& a, const CBigInt& b){return CBigInt(a)+=b;}
    friend CBigInt operator* (const CBigInt& a, const CBigInt& b){return CBigInt(a)*=b;}

    //comparing functions
    friend bool operator< (const CBigInt& a, const CBigInt& b){
      if(a.neg && !b.neg){
        return true;
      }else if(!a.neg && b.neg){
        return false;
      }
      if(a.number.size() < b.number.size()){
        return true;
      }else if(a.number.size() > b.number.size()){
        return false;
      }else{
        auto jt = b.number.begin();
        for(auto it = a.number.begin(); it < a.number.end(); it++){
          if((*it) < (*jt)){
            if(!a.neg && !b.neg){
              return true;
            }else{
              return false;
            }
          }else if((*it) > (*jt)){
            if(!a.neg && !b.neg){
              return false;
            }else{
              return true;
            }
          }
          jt++;
        }
        return false;
      }
    }
    friend bool operator== (const CBigInt& a, const CBigInt& b){return (!(a < b) && !(b < a));}
    friend bool operator!= (const CBigInt& a, const CBigInt& b){return !(a == b);}
    friend bool operator>  (const CBigInt& a, const CBigInt& b){return b < a;}
    friend bool operator<= (const CBigInt& a, const CBigInt& b){return a < b || a == b;}
    friend bool operator>= (const CBigInt& a, const CBigInt& b){return a > b || a == b;}

    // output operator <<
     friend std::ostream& operator<< (std::ostream& os, const CBigInt& n){
        if(n.neg){
          os<<"-";
        }
        for(auto it = n.number.begin(); it < n.number.end(); it++){
          os << (*it);
        }
        return os;
     }
    // input operator >>
    friend std::istream& operator>> (std::istream& is, CBigInt& n){
      CBigInt tmp;
      tmp.number.clear();
      std::string s;
      is>>s;

      long unsigned int i=0;
      if(s[i] == '-'){
        tmp.neg = true;
        i++;
      }
      for(; i < s.length(); i++){
        if(s[i] >= '0' && s[i] <= '9'){
              tmp.number.push_back(s[i]-'0');
        }else{
          break;
        }
      }
      if(tmp.number.size()==0){
        is.setstate(std::ios::failbit);    
        return is;
      }
      tmp.removeZeros();
      n = tmp;

      return is;
    }
  private:
    bool neg=false;
    std::vector<int> number;

    void absZero(){
      if(number.size()==1){
        if((*number.begin())==0){
          neg=false;
        }
      }
      return;
    }
    void removeZeros(){
      bool removed=false;
      do{
        auto it = number.begin();
        if(it==number.end()-1){
          return;
        }
        if((*it)==0){
          number.erase(it);
          removed=true;
        }else{
          removed=false;
          return;
        }
      }while(removed);
    }
    //addition and substraction
    void add(const CBigInt& a){
      
      bool minus = false;

      //sign
      if(a.number.size() > number.size()){
        if((a.neg && neg) || (a.neg && !neg)){
          minus = true;
        }else{
          minus = false;
        }

        int diff = a.number.size()-number.size();
        //add digits
        auto jt = a.number.end()-1;
        for(auto it = number.end()-1; it >= number.begin(); it--){
          if((!neg && !a.neg) || (neg && a.neg)){
            *it = *it + *jt;
          }else{
            *it = *jt-*it;
          }
          jt--;
        }
        //copy higher order of magnitude digits
        for(auto it = a.number.begin()+diff-1; it >= a.number.begin(); it--){
          number.insert(number.begin(),*it);
        }
      }else if(a.number.size() < number.size()){  //SHOULD BE OK
      //this size is bigger 
        if((a.neg && neg) || (!a.neg && neg)){
          minus = true;
        }else{
          minus = false;
        }
        //adding digits
        if(a.number.end() != a.number.begin() && number.end() != number.begin()){
          auto jt = number.end()-1;
          for(auto it = a.number.end()-1; it >= a.number.begin(); it--){
            if((!neg && !a.neg) || (neg && a.neg)){
              *jt = *it + *jt;
            }
            else{
              *jt = *jt-*it;
            }
            jt--;
          } 
        }
      }else{
      //size is equal to other
        bool thisSmaller=false;

        //find if a is smaller
        auto jt = a.number.begin();
        for(auto it = number.begin(); it < number.end(); it++){
            if((*it) < (*jt)){
              thisSmaller = true;
              break;
            }else{
              thisSmaller=false;
              break;
            }
            jt++;
        }

        //set sign
        if((a.neg && neg) || (thisSmaller && a.neg && !neg) || (!thisSmaller && neg && !a.neg)){
            minus = true;
          }else{
            minus = false;
        }
        //additoion/subtraction
        if(a.number.end() != a.number.begin() && number.end() != number.begin()){
          auto jt = number.end()-1;
          for(auto it = a.number.end()-1; it >= a.number.begin(); it--){
           if((!neg && !a.neg) || (neg && a.neg)){
              *jt = *it + *jt;
             }else if((!thisSmaller && neg && !a.neg)){
              *jt = *jt-*it;
             }else{
              *jt = *it-*jt;
            }
            jt--;
          }
        }

      }

      //shifting the 1s to the next order of magnitude
      if(a.number.end() != a.number.begin() && number.end() != number.begin()){
        for(auto it = number.end()-1; it > number.begin(); it--){
          if(*it >= 10 ){
            *it-=10;
            *(it-1)+=1;
          }
          if(*it < 0){
            *it=10+(*it);
            if(neg && a.neg){
              *it*=-1;
            }
            *(it-1)-=1;
          }
        }
      }
      
      if(*number.begin()<0){
        *number.begin()*=-1;
      }
      removeZeros();
      neg = minus;
      absZero();
    }
    void multiply(const CBigInt& a){
      CBigInt tmp(0);
      tmp.number.resize(number.size()+a.number.size(), 0);
      int ordi=0;
      int num;

      //calculate multiplication
      for(auto it = number.end()-1; it >= number.begin(); it--){
        int ordj=0;
        for (auto jt = a.number.end()-1; jt >= a.number.begin(); jt--){
          num=abs((*it)*(*jt));
          *(tmp.number.end()-1-ordi-ordj)+=num%10;
          *(tmp.number.end()-2-ordi-ordj)+=num/10;
          ordj+=1;
        }
        ordi+=1;
      }
      //shifting all num > 10 to higher order
      bool chng;
      do{
        chng = false;
        for(auto it = tmp.number.end()-1; it >= tmp.number.begin(); it--){
          if((*it) >= 10){
            *it-=10;
            *(it-1)+=1;
            chng= true;
          }
        }
      }while(chng);

      //remove first 0 if neccessary
      if(*(tmp.number.begin())==0){
        tmp.number.erase(tmp.number.begin());
      }
      
      number = tmp.number;
      //set sign
      if((neg && !a.neg) || (!neg && a.neg)){
        neg = true;
      }else{
        neg = false;
      }
      removeZeros();
      absZero();
    }
};

