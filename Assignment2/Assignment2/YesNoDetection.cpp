// YesNoDetection.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>
#include<string>
#include<fstream>
#include<vector>


using namespace std;

//declaring variables depend on voice sample
int upperboundSilence = 50000;
int wSize = 320;
int preciseWsize = 10;
int ZCRpreciseWsize = 100;
int lowerboundSound = 200000;
int NumZCRSampling = 10;
int ZCRforSlower = 10;

//function for finding Yes or No given word
bool ifYesOrNo(const vector<int> &word)
{
	long long int wordLength = word.size();						//word size


	//finding the index from where sound has magnitude sq>50000 from back because there can be noise also at end of word, we are traversing from back becuase we have to find s if s present then yes
	long long int ZCRsampleNumberWindow = wordLength-1;		
	long long int ZCRstartW = ZCRsampleNumberWindow;
	long long int ZCRendW = ZCRsampleNumberWindow - wSize;
	double ZCRmeanSqW=0;

	while(ZCRsampleNumberWindow-wSize >= 0)
	{
		ZCRstartW = ZCRsampleNumberWindow;
		ZCRendW = ZCRsampleNumberWindow - wSize;
		ZCRmeanSqW=0;

		for(long long int i= ZCRstartW; i>ZCRendW; i--)
		{
			ZCRmeanSqW += (word[i]*word[i])/double(wSize);
		}

		ZCRsampleNumberWindow -= wSize;

		if(ZCRmeanSqW > lowerboundSound)
		{
			break;
		}
	}

	long long int ZCRstartPreciseW = ZCRstartW;
	long long int ZCRendPreciseW = ZCRstartW - ZCRpreciseWsize;
	int ZCRnumPreciseW=0;
	double meanZCR =0;

	//calculating ZCR value for window of size 100
	while(ZCRstartW-ZCRpreciseWsize >= 0 && ZCRnumPreciseW < NumZCRSampling)
	{
		ZCRstartPreciseW = ZCRstartW;
		ZCRendPreciseW = ZCRstartW - ZCRpreciseWsize;
		int preciseZCR=0;
		int prev =1;
		for(long long int i= ZCRstartPreciseW; i>ZCRendPreciseW; i--)
		{
			if( (word[i]<0 && prev==1) || (word[i]>0 && prev==0))
			{
				preciseZCR++;
				if(prev==0)
				{
					prev=1;
				}
				else
				{
					prev=0;
				}
			}
		}
		meanZCR += preciseZCR/15.0;							//mean of 15 windows
		ZCRstartW -= ZCRpreciseWsize;
		ZCRnumPreciseW++;
	}
	if(meanZCR>ZCRforSlower)								//comparing ZCR value with lower bound ZCR for S per 100 sample
	{
		return true;										//if ZCR greater then lower bound then it is S and word is yes
	}
	else
	{
		return false;
	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	//Real Time Recording
	system("Recording_Module.exe 5 input_file.wav input_file.txt");

	ifstream infile;								//input sample file
	ofstream outfile;								//stores STE value per 320 and also tell from which window word started
	ofstream outfile1;								//stores only word seperated by four new lines 
	ofstream outfile2;								//stores result
	infile.open("input_file.txt");
	outfile.open("outputMultipleWords.txt");
	outfile1.open("extractMultipleWords.txt");
	outfile2.open("Result.txt");
	int wordNo = 1;
	if(infile.is_open())
	{
		vector<int> word;							//word variable store the sample of word
		int windowNumber=0;
		int WordStarted=0;
		while(!infile.eof())
		{
			double meanSqWindow=0;
			vector<double> WindowSamples;

			while(WindowSamples.size()<wSize && !infile.eof())
			{
				int sampleValue;
				infile >> sampleValue;
				WindowSamples.push_back(sampleValue);
				meanSqWindow += (sampleValue*sampleValue)/double(wSize);
			}

			int currWsize = WindowSamples.size();

			//checking end window which has size less than 320 samples
			if(currWsize < wSize)
			{

				if(WordStarted==1)   //we check if last word recorded full or not
				{

					int sampleNumberWindow=0;
					int startPreciseW = sampleNumberWindow;
					int endPreciseW = sampleNumberWindow + preciseWsize;
					double meanSqPrecise=0;

					//checking is the word is half recorded
					while(sampleNumberWindow < currWsize)
					{
						startPreciseW = sampleNumberWindow;
						endPreciseW = sampleNumberWindow + preciseWsize;
						meanSqPrecise=0;

						for(int i= startPreciseW; i<endPreciseW; i++)
						{
							meanSqPrecise += (WindowSamples[i]*WindowSamples[i])/double(preciseWsize);
						}

						sampleNumberWindow += preciseWsize;

						if(meanSqPrecise < upperboundSilence)
						{
							break;
						}

					}

					//checking if word ended or not
					if(meanSqPrecise < upperboundSilence)             
					{
						for(int i= 0; i<startPreciseW; i++)
						{
							outfile1 << WindowSamples[i] << endl;
							word.push_back(WindowSamples[i]);
						}
					}
					// not ended
					else
					{
						for(int i= 0; i<currWsize; i++)
						{
							outfile1 << WindowSamples[i] << endl;
							word.push_back(WindowSamples[i]);
						}
						outfile << "last word incompletely recorded" << endl;
					}

					outfile1 << endl;
					outfile1 << endl;
					outfile1 << endl;
					outfile1 << endl;
					word.clear();
					WordStarted=0;
					outfile<< "word ended" << endl;
					outfile<< endl;
				}

				break;
			}
			else
			{

				if(WordStarted==0)
				{

					if(meanSqWindow > upperboundSilence)            // word started
					{
						//We tried to find precise  sample where word start by taking window of 10 samples and checking amplitude crossed threshold
						//Refining data
						//outfile1.open("extractMultipleYes.txt");
						int sampleNumberWindow=0;
						int startPreciseW = sampleNumberWindow;
						int endPreciseW = sampleNumberWindow + preciseWsize;

						while(sampleNumberWindow < currWsize)
						{
							startPreciseW = sampleNumberWindow;
							endPreciseW = sampleNumberWindow + preciseWsize;
							double meanSqPrecise=0;

							for(int i= startPreciseW; i<endPreciseW; i++)
							{
								meanSqPrecise += (WindowSamples[i]*WindowSamples[i])/double(preciseWsize);
							}

							sampleNumberWindow += preciseWsize;

							//when we find if threshold is crossed
							if(meanSqPrecise > upperboundSilence)
							{
								break;
							}

						}
						//If threshold is crossed we start storing sample in the word
						for(int i= startPreciseW; i<currWsize; i++)
						{
							outfile1 << WindowSamples[i] << endl;
							word.push_back(WindowSamples[i]);
						}
						outfile<< endl;
						outfile<< "word started" << endl;
						WordStarted=1;

					}
				}

				else
				{

					if(meanSqWindow < upperboundSilence)                     // this signify that word ended inside that window
					{
						//we try to find precise sample where word ended and store samples upto that word only
						int sampleNumberWindow=0;
						int startPreciseW = sampleNumberWindow;
						int endPreciseW = sampleNumberWindow + preciseWsize;
						double meanSqPrecise=0;

						while(sampleNumberWindow < currWsize)
						{
							startPreciseW = sampleNumberWindow;
							endPreciseW = sampleNumberWindow + preciseWsize;
							meanSqPrecise=0;

							for(int i= startPreciseW; i<endPreciseW; i++)
							{
								meanSqPrecise += (WindowSamples[i]*WindowSamples[i])/double(preciseWsize);
							}

							sampleNumberWindow += preciseWsize;
							//check is below threshold
							if(meanSqPrecise < upperboundSilence)
							{
								break;
							}

						}

						//If below threshold then we store samples appeared before going below threshold
						for(int i= 0; i<startPreciseW; i++)
						{
							outfile1 << WindowSamples[i] << endl;
							word.push_back(WindowSamples[i]);
						}

						//check word is yes or no
						if(ifYesOrNo(word))
						{
							outfile2<< wordNo << ": Yes" << endl;
							wordNo++;
						}
						else
						{
							outfile2<< wordNo << ": No" << endl;
							wordNo++;
						}

						outfile1 << endl;
						outfile1 << endl;
						outfile1 << endl;
						outfile1 << endl;
						word.clear();				// empty word so that new word can be stored
						WordStarted=0;
						outfile<< "word ended" << endl;
						outfile<< endl;

					}

					else
					{
						//this signify that whole window is the part of word
						for(int i=0;i<currWsize;i++)
						{
							outfile1 << WindowSamples[i] << endl;
							word.push_back(WindowSamples[i]);
						}

					}
				}

				windowNumber++;
				outfile << windowNumber << "th window:- " << meanSqWindow << endl;
				WindowSamples.clear();

			}

		}

		infile.close();
		outfile.close();
		outfile1.close();
		outfile2.close();

	}
	return 0;
}
