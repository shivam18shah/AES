//
//  keygen.hpp
//  HelloWorld
//
//  Created by Vasisht Sankaranrayanan on 20/11/18.
//  Copyright © 2018 Vasisht Sankaranrayanan. All rights reserved.
//

#ifndef keygen_hpp
#define keygen_hpp


#include "keygen.h"
#include <iostream>
#include <cmath>
#include<bitset>
#include <string>
using namespace std;
typedef long double ll;

class bbs{
    
    ll p, q, M, seed, actual;
    
    ll gcd(ll a, ll b){
        if(b == 0) return a;
        return gcd(b, fmod(a,b));
    }
    
public:
    
    bbs(ll p, ll q, ll s){
        this->p = p;
        this->q = q;
        this->seed = s;
        M = p*q;
        actual = s;
    }
    
    ll getrandom(){
        
        ll r = fmod(actual*actual,M);
        actual = r;
        return r;
    }
    
    ll getirandom(int i){
        
        ll g = gcd(p, q);
        ll lcm = p*q/g;
        
        ll exp = 1;
        for(int j = 1; j <= i; ++j) exp = fmod((exp+exp),lcm);
        
        ll x0 = seed*seed;
        ll r = x0;
        
        for(int j = 2; j <= exp; ++j) {
            r = fmod((r*x0),M);
        }
        
        return r;
    }
    
    char ConvertHex(unsigned char x){
        if(x<10) return(char)((x)+'0');
        if(x>=10) return(char)((x - 10)+'A');
        return '0';
    }
    
    string* generateKey(){
            ll p = 65537;
            ll q = 10007;
            ll s = 100140048;
            char key[32];
            string finalKey[16];
            int strkey[4];
            short y;
            int quot;
            int key_index = 0;
            bbs b(p, q, s);
            for(int i = 0; i < 128; ++i) {
                std::bitset<1> y(b.getrandom());
                strkey[i%4]=int(y.to_ulong());
                if(i%4==3){
                    quot=0;
                    for(int j=0; j<4; j++){
                        quot=quot*2+strkey[j];
                    }
                    char hex_quot = bbs::ConvertHex(quot);
                    key[key_index] = hex_quot;
                    key_index++;
                }
            }
            cout<<"Key is: "<<endl;
            for(int i=0; i<32; i+=2){
                string s = "";
                cout<<key[i]<<" "<<key[i+1]<<endl;
                s+=key[i];
                s+=key[i+1];
                finalKey[i/2] = s;
            }
        return finalKey;
    }
    
};

#endif /* keygen_hpp */
