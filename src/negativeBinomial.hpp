#pragma once


#include "EBSeq.hpp"
#include <cmath>
#include <boost/math/special_functions/gamma.hpp>
#include <set>

namespace EBS
{
  
    class NB:public EBSeq
    {
    public:
        
        
        NB(COUNTS& scRNAexpMatrix, std::vector<int>& cellCluster) : EBSeq(scRNAexpMatrix, cellCluster)
        {
            COUNTS _var = aggregate::groupVar(scRNAexpMatrix, _clusinfo);
            
            COUNTS var;
            
            size_t G = scRNAexpMatrix.rows();
            
            var = _var.rowwise().mean();
            
            COUNTS q(G,1), I(G,1), mn;
            
            I.fill(1);
            
            mn = scRNAexpMatrix.rowwise().mean();
            
            for(int i = 0; i < G; i++){
                
                if(abs(var(i,0) - 0) < 0.0001)
                    var(i,0) = 1;
                
                if(var(i,0) <= mn(i,0))
                    q(i,0) = 0.99;
                else
                    q(i,0) = mn(i,0) / var(i,0);
            }
            
            _r = (mn.cwiseProduct(q)).array() / (I - q).array();
        }
        
        void init(std::vector<Float> hyperParam, std::vector<Float> lrate, int UC)
        {
            assert(UC < _sum.cols());
            
            _hp = hyperParam;
            
            _lrate = lrate;
            
            _uncertainty = UC;
        }
        
        // only to be called in init
        void DEpat()
        {
            size_t G = _mean.rows();
            
            size_t K = _sum.cols();
            
            std::vector<Float> abslogRatio(K - 1);
            
            std::vector<int> baseClus(K);
            
            for(size_t i = 0; i < G; i++)
            {
                auto ord = helper::sortIndexes<ROW>(_mean.row(i));
                
                baseClus[0] = 1;
                
                for(size_t j = 1; j < K; j++)
                {
                    Float s1 = _sum(i,ord[j - 1]);
                    
                    Float s2 = _sum(i,ord[j]);
                    
                    Float r1 = _clusinfo.size[ord[j - 1]] * _r(i,0);
                    
                    Float r2 = _clusinfo.size[ord[j]] * _r(i,0);
                    
                    Float tmp = kernel2case(s1,s2,r1,r2);
                    
                    abslogRatio[j - 1] = abs(tmp);
                    
                    //  more favorable for equal mean
                    if(tmp > 0)
                    {
                        baseClus[j] = baseClus[j - 1];
                    }
                    else
                    {
                        // DE start a new cluster
                        baseClus[j] = baseClus[j - 1] + 1;
                    }
                    
                }
                
                auto tmpOrd = helper::sortIndexes<std::vector<Float>>(abslogRatio);
                
                auto baseBit = partition::mapToBit(baseClus);
                
                auto pBit = partition::genBit(_uncertainty);
                
                // get promising DE pattern
                for(auto x:pBit)
                {
                    
                    auto newBit = baseBit;
                    
                    for(int t = 0; t < _uncertainty; t++)
                    {
                        auto tmpJ = tmpOrd[t];
                        
                        newBit[tmpJ] = x[t];
                    }
                    
                    auto newClus = partition::bitToPart(newBit);
                    
                    std::sort(newClus.begin(),newClus.end(),[&ord](size_t i1, size_t i2){return ord[i1] < ord[i2];});
                    
                    auto newClusOrd = partition::reorder(newClus);
                    
                    _dep.insert(newClusOrd);
                    
                    
//                    std::cout << "G " << i << " ";
//
//                    for(auto C : newClusOrd)
//                        std::cout << C << ",";
//
//                    std::cout << "\n";
                    
                }
                
            }
            
        }
        
        size_t DEPsize()
        {
            return _dep.size();
        }
        
        std::set<std::vector<int>> getDEP()
        {
            return _dep;
        }
        
        
        Float kernel(std::vector<int>& pat)
        {
            return 0;
            
            
        }
        
        void gradientAscent()
        {
            
        }
        
        
        Float kernel2case(Float& s1, Float& s2, Float& r1, Float& r2)
        {
            Float alpha = _hp[0];
            
            Float beta = _hp[1];
            
            Float res = lbeta(alpha + r1 + r2, beta + s1 + s2) + lbeta(alpha, beta) - lbeta(alpha + r1, beta + s1) - lbeta(alpha + r2, beta + s2);
            
            return res;
        }
        
        inline Float lbeta(Float x,Float y)
        {
            return boost::math::lgamma(x) + boost::math::lgamma(y) - boost::math::lgamma(x + y);
        }
        
        
    private:
        
        typedef decltype(_mean.row(0)) ROW;
        
        COUNTS _r;
        
        std::vector<Float> _lrate;
        
        // prop of each nonzero pattern
        Eigen::VectorXd _p;
        
        std::unordered_map<int, std::vector<int>> _hash;
        
        int _uncertainty;
        
        std::set<std::vector<int>> _dep;
    };
    
};
