#ifndef INSTANCE_H
#define INSTANCE_H

#include <iostream>
#include <vector>
#include <iomanip>
using namespace std;

class Instance{
    private:
        int classification;
        vector<double> featureValues;
        
    public:
        Instance(int newClass, vector<double> values){
            classification = newClass;
            featureValues = values;
        }
        
        int getClassification() const{
            return classification;
        }
        
        double getFeatureValue(int index) const{
            return featureValues.at(index);
        }
        
        int getNumFeatures() const{
            return featureValues.size();
        }
};

#endif