#include <iostream>
#include <fstream>
#include <unistd.h>
#include "opencv2/opencv.hpp"
#include "opencv2/plot.hpp"

struct meanStdDev{
    cv::Mat standardizedData;
    cv::Scalar mean;
    cv::Scalar stdDev;
};

class toolbox{
    public:
    toolbox(){
        std::cout<<"toolbox created"<<std::endl;
    }
    
    std::string combine2csv(std::string path, int no1, int no2, std::string nameAdd){
        std::ifstream input1(path + std::to_string(no1) + ".csv");
        std::ifstream input2(path + std::to_string(no2) + ".csv");
        std::string outputPath = path + std::to_string(no1) + std::to_string(no2) + nameAdd + ".csv";
        unlink(outputPath.c_str());
        std::ofstream output(outputPath);

        std::string line;
        while(std::getline(input1, line)){
            output << line << std::endl;
        }
        while(std::getline(input2, line)){
            output << line << std::endl;
        }
        
        output.close();
        return outputPath;
    }
    void printtoCSV(cv::Mat data){
        std::cout << "saving Mat to debug.csv\n"; 
        std::ofstream file("debug.csv");

        for (int i = 0; i < data.rows; ++i) {
        for (int j = 0; j < data.cols; ++j) {
            file << data.at<float>(i, j); // Assuming the matrix contains float values
            if (j < data.cols - 1) {
                file << ","; // Add comma separator except for the last element in a row
            }
        }
        file << std::endl; // Add newline after each row
    }

    file.close();
    }

};

class extractor{
    public:
    extractor(){
        std::cout<<"extractor created" << std::endl;
    }

    std::string extract2numbers(int no1, int no2, int start, int amount, std::string pathOld, std::string pathNew){
        extractNumber(no1, start, amount, pathOld, pathNew);
        extractNumber(no2, start, amount, pathOld, pathNew);
        return pathNew;
    }

    std::string extractNumber(int number, int start, int amount, std::string pathOld, std::string pathNew) {
        std::cout << "trying to extract " << number << " from the original file" << std::endl;

        std::string epathNew = pathNew + std::to_string(number) + ".csv";
        unlink(epathNew.c_str());
        std::ifstream file(pathOld);
        std::ofstream newFile(epathNew);

        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << pathOld << std::endl;
            return "";
        }
        if (!newFile.is_open()) {
            std::cerr << "Error: Could not create file " << epathNew << std::endl;
            return "";
        }

        std::string line;
        int lineCount = 0;
        while (std::getline(file, line) && lineCount < start + amount) {
            if (lineCount >= start) { // Start reading from 'start' line
                if (!line.empty()) {
                    // Find the first number in the line
                    size_t pos = line.find_first_of("0123456789");
                    if (pos != std::string::npos) {
                        // Extract the number from the line
                        std::string firstNumberStr = line.substr(pos);
                        int firstNumber = std::atoi(firstNumberStr.c_str());

                        // Check if the first number matches 'number'
                        if (firstNumber == number) {
                            newFile << line << std::endl;
                        }
                    }
                }
            }
        ++lineCount;
    }
        
        newFile.close();
        return epathNew;
    }

    std::string splitCSV(std::string path, int trainSize, int testSize){
        std::string dataPath = "data/";
        std::string fileTrainPath = dataPath + "Train.csv";
        std::string fileTestPath = dataPath + "Test.csv";
        std::ifstream file(path);
        std::ofstream fileTrain(fileTrainPath);
        std::ofstream fileTest(fileTestPath);

        std::string line;
        for(int i=0; i < trainSize+testSize; i++){
            std::getline(file, line);
            if(i < trainSize){fileTrain << line << std::endl;}
            else if(i < trainSize+testSize){fileTest << line << std::endl;}
        } 
    return fileTrainPath, fileTestPath;
    }
};

class processor{
public:
processor(){std::cout << "processor started" << std::endl;}

meanStdDev standardize(cv::Mat data, meanStdDev prevMeanStdDev){
    std::cout << "starting standardization" << std::endl;
    cv::Scalar mean(0), stdDev(0);
    for(int i = 0; i < data.cols; i++){
        cv::Mat curCol = data.col(i);
        if(prevMeanStdDev.stdDev[0] == -1){
            cv::meanStdDev(curCol, mean, stdDev);
            //std::cout << mean << std::endl;
        }
        else{
            mean = prevMeanStdDev.mean;
            stdDev = prevMeanStdDev.stdDev;
        }
        curCol -= mean[0];
        //cv::meanStdDev(curCol, mean, stdDev);
        if(stdDev[0] < 1e-6){
            stdDev[0] = 1e-6;
        }

        curCol = curCol / stdDev[0];
        cv::meanStdDev(curCol, mean, stdDev);
        std::cout << "mean: " << mean << "  stdDev: " << stdDev << "\n";
        data.col(i) = curCol;
    }
    //std::cout << globalStdDev << std::endl;
    prevMeanStdDev.standardizedData = data;
    prevMeanStdDev.mean = mean;
    prevMeanStdDev.stdDev = stdDev;

    return prevMeanStdDev;
}
void isStandardized(cv::Mat data){
    for(int i = 0; i < data.cols; i++){
        cv::Mat mean, stdDev;
        cv::meanStdDev(data, mean, stdDev);
        std::cout << "the array currently has a mean of: " << mean << "  and a standard deviation of: " << stdDev << std::endl;
    }
}

cv::Mat eigenStuff(cv::Mat data, int iterations, bool visualize){
    //first for displaying siginficance
    int maxDim = 100;
    cv::PCA pca(data, cv::Mat(), CV_PCA_DATA_AS_ROW, maxDim);
    cv::Mat mean = pca.mean;
    cv::Mat eigenValue = pca.eigenvalues;
    cv::Mat eigenVector = pca.eigenvectors;

    //std::cout << eigenValue;

    //calculate significance
    float sum = 0;
    cv::Mat significance;
    float sumSignificance = 0;
    for(int i = 0; i < eigenValue.rows; i++){
        sum += eigenValue.at<float>(i,0);
    }
    for(int i = 0; i < eigenValue.rows; i++){
        sumSignificance += (eigenValue.at<float>(i,0)/sum)*100;
        if(visualize){std::cout << "significance at " << i+1 << " dimensions = " << sumSignificance << "% compared to " << maxDim << " dimensions\n";
        if(i == iterations - 1){ std::cout << "--------------------------------------------------------------------\n";}}
    }

    //std::cout << "with the current level of dimensions we achieve " << sumSignificance << "% in comparison to 10 dimensions \n";

    //second for calculating with right values
    pca(data, cv::Mat(), CV_PCA_DATA_AS_ROW, iterations);
    mean = pca.mean;
    eigenValue = pca.eigenvalues;
    eigenVector = pca.eigenvectors;

    //apply PCA
    data = eigenVector.rowRange(0, iterations)*data.t();

    return data;
}

};

int main(){
//init
toolbox myToolbox;
extractor myExtractor;
processor myProcessor;
std::string emnistPath = "../data/emnistShuffled.csv";
std::string dataDirectory = "../data/";
int number1 = 14;   //N
int number2 = 7;    //G
int amountTrain = 1000;
int amountTest = 5000;

//process
myExtractor.extract2numbers(number1, number2, 0, amountTrain, emnistPath, dataDirectory);
std::string path = myToolbox.combine2csv(dataDirectory, number1, number2, "Train");
cv::Ptr< cv::ml::TrainData > tdata = cv::ml::TrainData::loadFromCSV( path, 0, 0, 1 ); // First col is the target as a float
cv::Mat sampleTrain = tdata->getTrainSamples();                                                        // Get design matrix
cv::Mat targetTrain = tdata->getTrainResponses();

myExtractor.extract2numbers(number1, number2, amountTrain, amountTest, emnistPath, dataDirectory);
path = myToolbox.combine2csv(dataDirectory, number1, number2, "Test");
tdata = cv::ml::TrainData::loadFromCSV( path, 0, 0, 1 ); // First col is the target as a float
cv::Mat sampleTest = tdata->getTrainSamples();                                                        // Get design matrix
cv::Mat targetTest = tdata->getTrainResponses();
// std::ofstream file("debug.txt");
// file << cv::format(sample, cv::Formatter::FMT_CSV);
// file.close();

meanStdDev emptyMeanStdDev, trainedMeanStdDev, testedMeanStdDev;
emptyMeanStdDev.stdDev = -1;
trainedMeanStdDev = myProcessor.standardize(sampleTrain, emptyMeanStdDev);
cv::Mat trainStd = trainedMeanStdDev.standardizedData;
testedMeanStdDev = myProcessor.standardize(sampleTest, trainedMeanStdDev);
cv::Mat testStd = testedMeanStdDev.standardizedData;
myToolbox.printtoCSV(testStd);
//myProcessor.isStandardized(trainStd);
//myProcessor.isStandardized(testStd);
std::cout << "Rows :" << trainStd.rows << "  Columns: " << trainStd.cols << std::endl;
cv::Mat trainStdComp = myProcessor.eigenStuff(trainStd, 50, false);      //5 is optimal since the eValues are 114, 47, 27, 25, 22, 17, 16, 11, 10, 10
cv::Mat testStdComp = myProcessor.eigenStuff(testStd, 50, false);      //5 is optimal since the eValues are 114, 47, 27, 25, 22, 17, 16, 11, 10, 10
std::cout << "Rows :" << trainStdComp.rows << "  Columns: " << trainStdComp.cols << std::endl;

return 0;
}