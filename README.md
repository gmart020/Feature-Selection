# Feature-Selection
Finds the subset of attributes that provide the highest accuracy amongst all subsets. The goal is to eliminate the number of attributes used to create a classification model. The accuracy of a subset is calculated by finding the number of correct classifications using the nearest neighbor to label the test object. Euclidean distance is used to measure the "distance" between two data objects.

Assumes there are no missing data points in the provided file. Each row of the input file should be one data object. The first value of each row represents the label of the data object. The rest of the values in the row represent the features of the respective data object. All values in a row should be separated by a comma.
