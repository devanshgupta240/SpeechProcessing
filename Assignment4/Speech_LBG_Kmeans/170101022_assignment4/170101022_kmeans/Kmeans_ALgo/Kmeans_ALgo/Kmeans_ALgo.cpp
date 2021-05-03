

#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream> 
#include <iomanip> 
# define M_PI 3.14159265358979323846
#include <math.h> 
#include <stdlib.h> 
#include <assert.h>
using namespace std;
#define P_ORDER 		 12
#define USE_TOKHURA	 	 1
#define WINDOW_SIZE 	 320
#define CODEBOOK_SIZE 	 8
#define KMEANS_ITERATIONS 	 12
#define ll long long int

//for calculating tokhuraDistance
double tokhuraDist(vector <double> cis,vector <double> cis2){
	static double weights[]={1,3,7,13,19,22,25,33,42,50,56,61};
	double d = 0 ;
	for (int i = 1; i < cis.size(); ++i){		
		d += (cis[i]-cis2[i])*(cis[i]-cis2[i])*weights[i-1];
	}
	return d;
}


double vectorDist(vector <double> cis,vector <double> cis2){
	double d = 0 ;
	for (int i = 1; i < cis.size(); ++i){
		d += (cis[i]-cis2[i])*(cis[i]-cis2[i]);
	}
	return d;
}

//for finding avarage distortion
double getDistortion(vector< vector<double> > &means,vector<int> &labels, vector< vector<double> > &universe) {
	double dist=0;
	assert(labels.size()==universe.size());
	int N=universe.size();
	for (int i = 0; i < N; ++i){
		dist += vectorDist(means[labels[i]],universe[i])/N;
	}
	return dist;
}

//random Value generation for initialization 
double rangeFloatRand(double &a, double &b){
	double diff = b-a;
	// RAND_MAX defined in cstdlib header
	return a + diff*rand()/RAND_MAX; 
}

//finding nearest neighbour
int getLabel(vector<double> &item, vector< vector<double> > &means){
	int label; 
	double dist, minDist = INT_MAX;
	for (int i = 0; i < means.size(); ++i) {
		if(USE_TOKHURA)
			dist = tokhuraDist(means[i],item);
		else
			dist = vectorDist(means[i],item);
		if(dist < minDist){
			minDist=dist;
			label=i;
		}
	}
	return label;
}

//updating code vetor
void updateMeansAdd(vector<double> &item, vector<double> &means, double &clusterSize){
	// item being added to this mean
	for (int i = 0; i < means.size(); ++i) {
		means[i] = (means[i]*(clusterSize-1)+item[i])/clusterSize;
	}
}

//updating code vector
void updateMeansRemove(vector<double> &item, vector<double> &means, double &clusterSize){
	// item being removed from this mean
	for (int i = 0; i < means.size(); ++i) {
		means[i] = (means[i]*(clusterSize+1)-item[i])/clusterSize;
	}
}

//function for calculating codebook of size 8 vectors
pair<vector<int>,vector<vector<double> >> k_means(const int K,int iterations, vector< vector<double> >	&universe, double EPSILON=0.30){
	int N = universe.size(), cols=universe[0].size();
	assert(cols==P_ORDER);
	vector< vector<double> > means(K, vector<double>(cols));
	vector<double> clusterSizes(K,0), colmin(cols,(double)INT_MAX), colmax(cols,(double)INT_MIN);
	// calc colwise min maxs
	for (int i = 0; i < N; ++i){
		for (int j = 0; j < cols; ++j){
			colmin[j] = min(colmin[j], universe[i][j]);
			colmax[j] = max(colmax[j], universe[i][j]);
		}
	}
	// initialize means-
	/* For each column, take values randomly between the corresponding minimum and maximum */
	for (int i = 0; i < K; ++i){
		for (int j = 0; j < cols; ++j){
			means[i][j] = rangeFloatRand(colmin[j],colmax[j]);
		}
	}
	vector<int> labels(N,-1);
	int newlabel;
	// do iterations
	while(iterations--){
		bool converged=true;
		// make a copy of current means/centroids
		vector< vector<double> > nextMeans = means; 
		for (int i = 0; i < N; ++i)
		{
			// get distance from original means of current iteration
			newlabel = getLabel(universe[i],means);
			if(newlabel!=labels[i]){
				// some label changed => not yet converged
				converged=false;
				if(labels[i]!=-1){
					clusterSizes[labels[i]]--;
					updateMeansRemove(universe[i],nextMeans[labels[i]],clusterSizes[labels[i]]);
				}
				clusterSizes[newlabel]++;
				updateMeansAdd(universe[i],nextMeans[newlabel],clusterSizes[newlabel]);
				labels[i]=newlabel;
			}
		}
		means = nextMeans;
		if(getDistortion(means,labels,universe) < EPSILON)
			break;
		if(converged)
			break;
	}

	//cout<< "Iterations Done: "<<KMEANS_ITERATIONS - iterations<<endl;
	cout <<"Final distortion: "<< getDistortion(means,labels,universe)<<endl;

	// classify final.
	return make_pair(labels,means);
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
#define FIXED_FLOAT(x) std::fixed <<std::setprecision(6)<<(x) 
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	vector< vector<double> > universe;
	universe = loadCSVFile(6340,"Universe.csv");	
	//INPUT_FS.close();
	//cout<<"U"<<universe.size()<<endl;
	assert(universe.size());
	cout << "Universe Size is " << universe.size() << endl; 
	// Find the K centroids here-
	pair<vector<int>,vector<vector<double> >> p = k_means(CODEBOOK_SIZE, KMEANS_ITERATIONS, universe);
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

