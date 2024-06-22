#include <iostream>
#include <fstream>
#include <unistd.h>
#include "opencv2/opencv.hpp"
#include "opencv2/plot.hpp"

struct meanStdDev{
    cv::Mat standardizedData = cv::Mat();
    std::vector<float> mean;
    std::vector<float> stdDev;
};

struct PCAmessage{
    cv::Mat data;
    cv::Mat mean;
    cv::Mat eigenValue;
    cv::Mat eigenVector;
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
    void printtoCSV(cv::Mat data, std::string name){
        name = name + ".csv";
        std::cout << "saving Mat to " << name << std::endl; 
        std::ofstream file(name);

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
    void printMat(const cv::Mat& mat) {
        for (int i = 0; i < mat.rows; ++i) {
            for (int j = 0; j < mat.cols; ++j) {
                std::cout << mat.at<float>(i, j); // Assuming the matrix contains float values
                if (j < mat.cols - 1) {
                    std::cout << ", "; // Add comma separator except for the last element in a row
                }
            }
            std::cout << std::endl; // Add newline after each row
        }
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
        std::vector<float> meanVec(data.cols, -1.0f);
        std::vector<float> stdDevVec(data.cols, -1.0f);
        cv::Scalar newMean, newStdDev;
        meanStdDev returnMsg;

        if (!prevMeanStdDev.mean.empty() && !prevMeanStdDev.stdDev.empty()) {
            meanVec = prevMeanStdDev.mean;
            stdDevVec = prevMeanStdDev.stdDev;
        }

        for(int i = 0; i < data.cols; i++){
            cv::Mat curCol = data.col(i);

            if(prevMeanStdDev.stdDev.empty()){
                stdDevVec[i] = newStdDev[0];    //prov
                cv::meanStdDev(curCol, newMean, newStdDev);
                if(newStdDev[0] < 1e-6){
                    newStdDev[0] = 1e-6;
                }
                //std::cout << "newMean: " << newMean[0] << "  newStdDev: " << newStdDev[0] << std::endl;
                meanVec[i] = newMean[0];
                stdDevVec[i] = newStdDev[0];
                //std::cout << i <<" newMean " << meanVec[411] << std::endl; 
            //std::cout << std::endl << meanVec.size() << std::endl << std::endl;
            }
            else{
                //std::cout << "already predefined mean/stdDev}\n";
            }
            //calculate standardization
            curCol = (curCol - meanVec[i]) / stdDevVec[i];
            data.col(i) = curCol;
        }
        returnMsg.standardizedData = data;
        returnMsg.mean = meanVec;
        returnMsg.stdDev = stdDevVec;
        //std::cout << "returnMsg has the size: " << meanVec.size() << std::endl;
        // for(int i = 0; i < meanVec.size(); i++){
        //     std::cout << meanVec[i] << std::endl;
        // }
        std::cout << "standardisation finished\n";
        return returnMsg;
    }
    void isStandardized(cv::Mat data, std::string name, bool advancedAnalysis){
        int standardizedMean = 0, standardizedStdDev = 0, NOTstandardizedMean = 0, NOTstandardizedStdDev = 0;
        for(int i = 0; i < data.cols; i++){
            cv::Scalar mean, stdDev;
            cv::meanStdDev(data.col(i), mean, stdDev);
            //std::cout << "the array currently has a mean of: " << mean << "  and a standard deviation of: " << stdDev << std::endl;
            (mean[0] < 1e-4) ? (standardizedMean++, 0) : (NOTstandardizedMean++, 1);
            ((stdDev[0] < 1.1 && stdDev[0] > 0.9) || stdDev[0] == 0) ? (standardizedStdDev++, 0) : (NOTstandardizedStdDev++, 1);
            if(advancedAnalysis){
                std::cout << "Line" << i << ":  mean=" << mean[0] << "  stdDev=" << stdDev[0] << std::endl;
            }
        }
        std::cout << "after standardizing there are " << standardizedMean << "/" << data.cols << " lines with a mean of 1\n"
        "and " << standardizedStdDev << "/" << data.cols << " lines with a standard deviation of 0 in the " << name << " dataset\n\n";
    }

};

class PCA{
    public:
    PCA(){}

    cv::PCA fitPCA(cv::Mat input, float goal){
        std::cout << "the goal is an accuracy of " << goal << "% compared to " << input.rows << " dimensions\n";
        cv::PCA pca(input, cv::Mat(), CV_PCA_DATA_AS_ROW, input.rows);
        float sum = cv::sum(pca.eigenvalues.col(0))[0];
        float sumSignificance = 0;
        int dimensions = input.rows;
        for(int i = 0; i < pca.eigenvalues.rows; i++){
            sumSignificance += (pca.eigenvalues.at<float>(i,0));
            if((sumSignificance/sum) > (goal/100)){
                std::cout << "an accuracy of more than " << goal << "% can be achieved with " << i << " dimensions\n";
                dimensions = i;
                break;
            }
        }
        cv::PCA pca2(input, cv::Mat(), CV_PCA_DATA_AS_ROW, dimensions);
        return pca2;
    }

    cv::Mat transformPCA(cv::Mat input, cv::PCA pca){
        cv::Mat output = pca.project(input);
        return output;
    }
};

class SVM{
    public:
    SVM() {
        std::cout << "initialising SVM\n";
        svm = cv::ml::SVM::create();
        svm->setType(cv::ml::SVM::C_SVC);
        svm->setKernel(cv::ml::SVM::RBF);
        svm->setC(1.0); 
        svm->setGamma(0.01);
        svm->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 100, 1e-6)); 
}

    void trainSVM(cv::Mat trainData, cv::Mat trainLabels) {
        svm->train(trainData, cv::ml::ROW_SAMPLE, trainLabels);
        std::cout << "SVM trained\n";
    }

    int accuracySVM(cv::Mat data, cv::Mat labels, bool augmentation){
        cv::Mat predictions;
        svm->predict(data, predictions);

        predictions.convertTo(predictions, CV_32S);
        labels.convertTo(labels, CV_32S);
        float success = 0;
        float failure = 0;

        for(int i = 0; i < predictions.rows; i++){
            if(augmentation){std::cout << i << "  prediction: " << predictions.at<int>(i, 0) << "  reality: " << labels.at<int>(i,0) << std::endl;}
            int curPrediction =predictions.at<int>(i, 0);
            int curTruth =labels.at<int>(i,0);
            if(curPrediction == curTruth){
                success++;
                std::cout << "Success! Success count is now at: " << success << std::endl;
            }
            else{
                failure++;
                std::cout << "Failure! Failure count is now at: " << failure << std::endl;
            }
        }
        //std::cout << "final success: " << success << "  final failure: " << failure << "  final accuracy: " << success/(success+failure) << std::endl;
        float accuracy = (success/(success+failure))*100;

        std::cout << "success: " << success << "  failures: " << failure << std::endl;
        std::cout << "accuracy: " << accuracy << "%" <<std::endl;

        return accuracy;
    }

    private:
    cv::Ptr<cv::ml::SVM> svm;  
};

int main(){
//init
toolbox myToolbox;
extractor myExtractor;
processor myProcessor;
PCA myPCA;
SVM mySVM;
std::string emnistPath = "../data/emnistShuffled.csv";
std::string dataDirectory = "../data/";
int number1 = 14;   //N
int number2 = 7;    //G
int amountTrain = 1000;
int amountTest = 5000;

//prepare data
myExtractor.extract2numbers(number1, number2, 0, amountTrain, emnistPath, dataDirectory);
std::string path = myToolbox.combine2csv(dataDirectory, number1, number2, "Train");
cv::Ptr<cv::ml::TrainData> tdata = cv::ml::TrainData::loadFromCSV( path, 0, 0, 1 ); 
cv::Mat trainData = tdata->getTrainSamples();                                                        
cv::Mat trainLabels = tdata->getTrainResponses();
int labelType = trainLabels.type() & CV_MAT_DEPTH_MASK;
std::cout << labelType  << std::endl;

myExtractor.extract2numbers(number1, number2, amountTrain, amountTest, emnistPath, dataDirectory);
path = myToolbox.combine2csv(dataDirectory, number1, number2, "Test");
tdata = cv::ml::TrainData::loadFromCSV( path, 0, 0, 1 ); 
cv::Mat testData = tdata->getTrainSamples();                                                     
cv::Mat testLabels = tdata->getTrainResponses();

//Standardize
meanStdDev emptyMeanStdDev, trainedMeanStdDev, testedMeanStdDev;
myToolbox.printtoCSV(trainData, "beforeTrainStd");
trainedMeanStdDev = myProcessor.standardize(trainData, emptyMeanStdDev);
testedMeanStdDev = myProcessor.standardize(testData, trainedMeanStdDev);
cv::Mat trainStd = trainedMeanStdDev.standardizedData;
cv::Mat testStd = testedMeanStdDev.standardizedData;
myProcessor.isStandardized(trainStd, "trainStd", false);
myProcessor.isStandardized(testStd, "testStd", false);
myToolbox.printtoCSV(trainStd, "trainStd");
std::cout << "Rows :" << trainStd.rows << "  Columns: " << trainStd.cols << std::endl;

//PCA
cv::PCA pca = myPCA.fitPCA(trainStd, 95);
cv::Mat trainComp = myPCA.transformPCA(trainStd, pca);
cv::Mat testComp = myPCA.transformPCA(testStd, pca);
std::cout << "Rows :" << trainComp.rows << "  Columns: " << trainComp.cols << std::endl;
std::cout << "Rows :" << testComp.rows << "  Columns: " << testComp.cols << std::endl;

//SVM
trainLabels.convertTo(trainLabels, CV_32S);
std::cout << trainLabels.type() << std::endl;
mySVM.trainSVM(trainComp, trainLabels);
mySVM.accuracySVM(testComp, testLabels, false);
return 0;
};