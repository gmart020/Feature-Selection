#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <math.h>
#include <limits>
#include <algorithm>
#include "Instance.h"
using namespace std;

/*
 * Prints a vector of numbers as a set ({num1, num2, ...}).
 */
void printSetNotation(vector<int> &v){
    cout << "{";
    for (vector<int>::iterator i = v.begin(); i != v.end(); ++i){
        if (i != --v.end()){
            cout << *i << ", "; 
        }
        else {
            cout << *i;
        }
    }
    cout << "}";
}

/*
 * Given a set of features, returns all possible combinations of features by 
 * removing at least one feaature. Used by Forward Selection function.
 */
vector<vector<int>> getSetsBackwards(vector<int> &v){
    vector<vector<int>> final;
    vector<vector<int>> setElements;
    for (int i = 0; i < v.size(); ++i){
        vector<int> temp; 
        temp.push_back(v.at(i));
        setElements.push_back(temp);
    }
    for (int i = 0; i < setElements.size(); ++i){
        vector<int> vec(setElements.at(i).size() + v.size());  
        sort(v.begin(), v.end());
        sort(setElements.at(i).begin(), setElements.at(i).end());
        vector<int>::iterator it;
        it = set_difference (v.begin(), v.end(), setElements.at(i).begin(), setElements.at(i).end(), vec.begin());
        vec.resize(it-vec.begin());
        final.push_back(vec);
    }
    return final;
}

/*
 * Given a set of features, returns all possible combinations of features by 
 * adding at least one feaature. Used by Forward Selection function.
 */
vector<vector<int>> getCombinations(vector<int> &initialSet, vector<int> &fullSet){
    vector<int> v(fullSet.size() + initialSet.size());  
    sort(initialSet.begin(), initialSet.end());
    sort(fullSet.begin(), fullSet.end());
    vector<int>::iterator it;
    it = set_difference (fullSet.begin(), fullSet.end(), initialSet.begin(), initialSet.end(), v.begin());
    v.resize(it-v.begin());                      
    vector<vector<int>> newSets;
    for (it=v.begin(); it!=v.end(); ++it){
        initialSet.push_back(*it);
        newSets.push_back(initialSet);
        initialSet.pop_back();
    }
    return newSets;
}

/*  
 * Calculates the Euclidian Distance between two Instance objects by squaring 
 * the sum of all differences of the respective values for each feature and 
 * taking the square root. Better to google what it means beacause this 
 * explanation is no good.
 */
double findEuclidianDistance(Instance &subject, Instance &neighbor, vector<int> &features){
    double takeRootOf = 0;
    for (int feature : features){
        double subjectValue = subject.getFeatureValue(feature - 1);
        double neighborValue = neighbor.getFeatureValue(feature - 1);
        takeRootOf += pow((subjectValue - neighborValue), 2);
    }
    return sqrt(takeRootOf);
}

/*
 * Finds nearest neighbor to target Instance. Uses Euclidian Distance to 
 * accomplish this. Returns the class label of the nearest neighbor.
 */
double nearestNeighbor(vector<Instance>::iterator iter, vector<Instance> &data, vector<int> &feature){
    double shortestDistance = numeric_limits<double>::infinity();   // Set default distance to infinity.
    double classLabelToReturn = -1;  
    for (vector<Instance>::iterator iter2 = data.begin(); iter2 != data.end(); ++iter2){
        if (iter != iter2){
            double distance = findEuclidianDistance(*iter, *iter2, feature);
            if (shortestDistance > distance){
                shortestDistance = distance;
                classLabelToReturn = iter2->getClassification();
            }
        }
    }
    return classLabelToReturn;
}

/*
 *  Given a set of data and a subset of features, use nearest neighbor algorithm
 *  to classify new instance. Returns the accuracy of the classification.
 */
double leaveOneOutValidation(vector<Instance> &data, vector<int> &features){
    int instances = data.size(); 
    double correctGuesses = 0;         
    for (vector<Instance>::iterator iter = data.begin(); iter != data.end(); ++iter){
        // Use nearest neighhbor to find class.
        int classLabelReturned = nearestNeighbor(iter, data, features);
        if (iter->getClassification() == classLabelReturned){
            correctGuesses++;
        }
    }
    return (correctGuesses / instances) * 100;
}

vector<int> forwardSelection(vector<Instance> &data, vector<int> &fullSet){
    vector<int> initialSet(0);
    double mostAccuratePercentageGlobal = 0;  
    vector<int> mostAccurateSetGlobal = initialSet;
    double mostAccuratePercentageLocal = 0;
    vector<int> mostAccurateSetLocal = initialSet;
    vector <int> testSet = fullSet;
    double firstAccuracyCheck = leaveOneOutValidation(data, testSet);
    mostAccuratePercentageGlobal = firstAccuracyCheck;
    mostAccurateSetGlobal = testSet;
    cout << "The dataset has " << fullSet.size() << " features, using \"leaving-one-out\" evaluation, I get an accuracy of " << firstAccuracyCheck << "%\n\n";
    cout << "Beginning search.\n\n";
    for (int i = 0; i < fullSet.size() - 1; ++i){
        vector<vector<int>> nextSetsToTest = getCombinations(mostAccurateSetLocal, fullSet);
        mostAccurateSetLocal.clear();
        mostAccuratePercentageLocal = 0;
        for (int j = 0; j < nextSetsToTest.size(); ++j){
            double accuracyOfSet = leaveOneOutValidation(data, nextSetsToTest.at(j));
            cout << "\tUsing features(s) ";
            printSetNotation(nextSetsToTest.at(j));
            cout << " accuracy is " << accuracyOfSet << "%\n"; 
            if (accuracyOfSet > mostAccuratePercentageLocal){
                mostAccuratePercentageLocal = accuracyOfSet;
                mostAccurateSetLocal = nextSetsToTest.at(j);
            }
        }
        cout << endl;
       
        if (mostAccuratePercentageLocal > mostAccuratePercentageGlobal){
            mostAccuratePercentageGlobal = mostAccuratePercentageLocal;
            mostAccurateSetGlobal = mostAccurateSetLocal;
        }
        else if(mostAccuratePercentageLocal < mostAccuratePercentageGlobal){
            cout << "(Warning, accuracy has decreased! Continuiung search in case of local maxima)\n";
        }
        cout << "Feature set "; 
        printSetNotation(mostAccurateSetLocal); 
        cout << " was best, accuracy is " << mostAccuratePercentageLocal << "%\n\n";
    }
    cout << "Finished search!! The best feature subset is ";
    printSetNotation(mostAccurateSetGlobal); 
    cout << ", which has an accuracy of " << mostAccuratePercentageGlobal << "%\n"; 
    return mostAccurateSetGlobal;
}

vector<int> backwardSelection(vector<Instance> &data, vector<int> &fullSet){
    double mostAccuratePercentageGlobal = 0;  // Holds the highest accuracy of the the most accurate feature subset.
    vector<int> mostAccurateSetGlobal = {}; // Holds the feature set with the highest accuracy of all possible combinations.
    double mostAccuratePercentageLocal = 0;
    vector<int> mostAccurateSetLocal = {};
    vector <int> testSet = fullSet;
    double firstAccuracyCheck = leaveOneOutValidation(data, testSet);
    mostAccuratePercentageGlobal = firstAccuracyCheck;
    mostAccuratePercentageLocal = firstAccuracyCheck;
    mostAccurateSetGlobal = testSet;
    mostAccurateSetLocal = testSet;
    int n = fullSet.size();
    cout << "The dataset has " << n << " features, using \"leaving-one-out\" evaluation, I get an accuracy of " << firstAccuracyCheck << "%\n\n";
    cout << "Beginning search.\n\n";
    for (int i = 0; i < n - 1; ++i){
        vector<vector<int>> nextSetsToTest = getSetsBackwards(mostAccurateSetLocal);
        mostAccurateSetLocal.clear();
        mostAccuratePercentageLocal = 0;
        for (int j = 0; j < nextSetsToTest.size(); ++j){
            double accuracyOfSet = leaveOneOutValidation(data, nextSetsToTest.at(j));
            cout << "\tUsing features(s) ";
            printSetNotation(nextSetsToTest.at(j));
            cout << " accuracy is " << accuracyOfSet << "%\n";
            if (accuracyOfSet > mostAccuratePercentageLocal){
                mostAccuratePercentageLocal = accuracyOfSet;
                mostAccurateSetLocal = nextSetsToTest.at(j);
            }
        }
        cout << endl;
        if (mostAccuratePercentageLocal > mostAccuratePercentageGlobal){
            mostAccuratePercentageGlobal = mostAccuratePercentageLocal;
            mostAccurateSetGlobal = mostAccurateSetLocal;
        }
        else if(mostAccuratePercentageLocal < mostAccuratePercentageGlobal){
            cout << "(Warning, accuracy has decreased! Continuiung search in case of local maxima)\n";
        }
        cout << "Feature set "; 
        printSetNotation(mostAccurateSetLocal); 
        cout << " was best, accuracy is " << mostAccuratePercentageLocal << "%\n\n";
    }
    cout << "Finished search!! The best feature subset is ";
    printSetNotation(mostAccurateSetGlobal); 
    cout << ", which has an accuracy of " << mostAccuratePercentageGlobal << "%\n"; 
    return mostAccurateSetGlobal;
}

/*
 * Instance class created to hold a line of input representing an object. Mostly used
 * to make it easier to not get lost.
 */
int main(){
    cout << "Welcome to Gerardo Martinez's Feature Selection Algorithm.\n" << endl;
    cout << "Type the name of the file to test: ";
    string file;
    cin >> file;
    cout << endl;
    cout << "Type the number of the algorithm you want to run.\n\n";
    int input;
    cout << "\t1) Forward Selection\n";
    cout << "\t2) Backward Elimination\n\n\t\t";
    cin >>  input;
    cout << endl;
    vector<Instance> data;
    ifstream inputFile;
    inputFile.open(file);
    string line;
    while (getline(inputFile, line)){
        stringstream ss;
        ss.str(line);
        double number, classification;
        vector<double> featureValues;
        ss >> classification;
        while (ss >> number){
            featureValues.push_back(number);
        }
        Instance tuple(classification, featureValues);
        data.push_back(tuple);
    }
    vector<int> fullSet;
    int numFeatures = data[0].getNumFeatures();
    for (int i = 1; i <= numFeatures; ++i){
        fullSet.push_back(i);
    }
    cout << "The dataset has " << numFeatures << " (not including the class attribute), with " << data.size() << " instances.\n\n";
    cout << "Please wait while I normalize the data... Done!\n\n";
    vector<int> bestSet;
    if (input == 1){
        bestSet = forwardSelection(data, fullSet);
    }
    else if (input == 2){
        bestSet = backwardSelection(data, fullSet);
    }
    else {
        cout << "Wow, you smart!";    
    }
    cout << endl;
    return 0;   
}