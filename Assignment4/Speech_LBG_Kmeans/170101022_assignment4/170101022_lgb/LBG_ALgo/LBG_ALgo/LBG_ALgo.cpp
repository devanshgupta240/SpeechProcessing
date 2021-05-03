#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream> 
#include <iomanip> 
# define M_PI    3.14159265358979323846
#include <math.h>
#include <stdlib.h> 
#include <assert.h>
using namespace std;

// Codebook parameters
#define CODEBOOK_SIZE 	 8
#define KMEANS_ITERATIONS 	 12
#define P_ORDER 		 12
#define USE_HAMMING 	 1
#define USE_TOKHURA	 	 1
#define WINDOW_SIZE 	 320
#define ll long long int

double tokhuraDist(vector <double> &cis,vector <double> &cis2){
	static double weights[]={1,3,7,13,19,22,25,33,42,50,56,61};
	double d = 0 ;
	for (int i = (cis.size()>P_ORDER); i < cis.size(); ++i){		
		d += (cis[i]-cis2[i])*(cis[i]-cis2[i])*weights[i-1];
	}
	return d;
}

double euclideanDistSq(vector <double> &cis,vector <double> &cis2){
	double d = 0 ;
	for (int i = (cis.size()>P_ORDER); i < cis.size(); ++i){
		d += (cis[i]-cis2[i])*(cis[i]-cis2[i]);
	}
	return d;
}

double getDistortion(vector< vector<double> > &codebook,vector<int> &labels, vector< vector<double> > &universe) {
	assert(labels.size()==universe.size());
	double dist=0;
	int N=universe.size();
	for (int i = 0; i < N; ++i){
		dist += euclideanDistSq(codebook[labels[i]],universe[i])/N;
	}
	return dist;
}

double rangeFloatRand(double a, double b){
	double diff = b-a;
	return a + diff*rand()/RAND_MAX; 
}
int getLabel(vector<double> &item, vector< vector<double> > &codebook){
	int label=-1; 
	double dist, minDist = INT_MAX;
	for (int i = 0; i < codebook.size(); ++i) {
		if(USE_TOKHURA)
			dist = tokhuraDist(codebook[i],item);
		else
			dist = euclideanDistSq(codebook[i],item);
		if(dist < minDist){
			minDist=dist;
			label=i;
		}
	}
	return label;
}

vector<double> calcSplit(vector<double> &mean, vector<int> &labels, int label, vector< vector<double> >	&universe){
	int N = universe.size(), cols=universe[0].size();
	double sum_dev;
	vector<double> split_epsilon(cols);
	for (int j = 0; j < cols; ++j){
		sum_dev=0;
		for (int i = 0; i < N; ++i){
			if(labels[i]==label)
				sum_dev += pow(universe[i][j]-mean[j],2);
		}		
		split_epsilon[j] = sqrt(sum_dev/N)/1000.0;
	}
	return split_epsilon;
}

vector<vector<double> > handleEmptyCells(vector<vector<double> > &codebook, vector<double> &is_empty, vector<int> &labels){
	vector<vector<double> > newMeans;
	map<int,int>labelMap;
	int counter=0;
	for (int i = 0; i < codebook.size(); ++i){
		if(!is_empty[i]){
			newMeans.push_back(codebook[i]);								
			labelMap[i]=counter++;
		}
	}
	for (int i = 0; i < labels.size(); ++i) {
		assert(labelMap.find(labels[i])!=labelMap.end());
		labels[i]=labelMap[labels[i]];
	}
	return newMeans;
}
pair<vector<int>,vector<vector<double>>> LBG(int K,int iterations, vector< vector<double> >	&universe, double DISTORTION_EPSILON=0.1){
	K = pow(2,ceil(log(K*1.0)/log(2.0)));	
	const int N = universe.size(), cols=universe[0].size();
	assert(cols==P_ORDER);
	vector<double> col_averages(cols,0);
	vector< vector<double> > codebook;
	
	// calc colwise averages
	for (int j = 0; j < cols; ++j){
		col_averages[j]=0;
		for (int i = 0; i < N; ++i){
			// use as a sum
			col_averages[j] += universe[i][j];
		}
		col_averages[j]/=N;
	}

	codebook.push_back(col_averages);
	vector<int> labels(N,0);			
	vector<double> split_epsilon;
	
	double minDistortion=INT_MAX, currDistortion;
	while(codebook.size()<K){
		// make a copy of current codebook/centroids
		vector< vector<double> > nextCodebook(codebook.begin(),codebook.end());
		// Do the splitting for every centroid 
		for (int i = 0; i < codebook.size(); ++i){
			split_epsilon = calcSplit(codebook[i],labels,i,universe);
			for (int j = 0; j < cols; ++j){
				nextCodebook[i][j] -= split_epsilon[j];
				codebook[i][j] += split_epsilon[j];
			}
		}
		// double the size
		codebook.insert(codebook.end(),nextCodebook.begin(),nextCodebook.end());

		bool nochange;
		int iter = iterations, newlabel;
		do
		{
			nochange=true;
			for (int i = 0; i < N; ++i)
			{
				newlabel = getLabel(universe[i],codebook);
				nochange = nochange && (labels[i]==newlabel);
				labels[i] = newlabel;
			}
			vector<vector<double> > nextColSums(codebook.size(),std::vector<double>(cols,0));
			vector<double> labelCount(codebook.size(),0);
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < cols; ++j){
					nextColSums[labels[i]][j] += universe[i][j];
				}
				labelCount[labels[i]]++;
			}
		// update to new col wise averages for each centroid
			for (int i = 0; i < codebook.size(); ++i){
				if(labelCount[i]==0){
					continue;
				}
				for (int j = 0; j < cols; ++j){
					codebook[i][j]= nextColSums[i][j]/labelCount[i];
				}
			}						
			currDistortion = getDistortion(codebook,labels,universe);
			minDistortion = min(minDistortion,currDistortion);

			if(nochange){
				break;
			}
		}
		while(0<--iter && currDistortion > DISTORTION_EPSILON);
	}
	cout<< "Final codebook size: "<<codebook.size()<<endl;
	cout <<"Final distortion: "<< getDistortion(codebook,labels,universe)<<endl;
	// classify final.
	return make_pair(labels,codebook);
}

vector<vector<double>> loadCSVFile(int n,string filename)
{
	std::vector<std::vector<double>> result(n);

    std::ifstream input(filename);

    std::string s;
    for (int i = 0; i < n; i++)
    {
        std::getline(input, s);
        std::istringstream iss(s);

        std::string num;
        while (std::getline(iss, num, ','))
            result[i].push_back(std::stod(num));
    }

    return result;
}
int main(int argc, char const *argv[]) {
	vector< vector<double> > universe;
	universe = loadCSVFile(6340,"Universe.csv");	
	assert(universe.size());
	// Find the K centroids here-
	pair<vector<int>,vector<vector<double>>> p = LBG(CODEBOOK_SIZE, KMEANS_ITERATIONS, universe);	
	vector<vector<double> > codebook = p.second;
	vector<int> vectorIndex = p.first;
	cout << "Codebook :-" << endl;
	ofstream outfile;
	outfile.open("codebook.txt");
	for(int i=0;i<codebook.size();i++)
	{
		for(int j=0;j<codebook[i].size();j++)
		{
			cout << codebook[i][j] << " ";
			outfile << codebook[i][j] << " ";
		}
		cout << endl;
		outfile << endl;
	}
	outfile.close();

	//cout << vectorIndex.size() << endl;
	ofstream outfile1;
	outfile1.open("vectorIndexOfEachRecord.txt");
	for(int i=0;i<vectorIndex.size();i++)
	{
		outfile1 << vectorIndex[i] << endl;
	}
	outfile1.close();
	cout << "labels of each record got stored in file successfully" << endl;
	system("pause");
	return 0;
}
