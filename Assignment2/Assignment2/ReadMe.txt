========================================================================
    CONSOLE APPLICATION : Assignment2 Project Overview
========================================================================
Name :- Devansh Gupta
Roll No.:- 170101022

1. Working of the code:-
	a) First I calculated STE for window of every 320 samples.
	b) checked if STE > 50000 
	c) if STE>50000, that means it is not silence, word has started
	d) then I decrease the size of window to 10 to precisely find from where actually word start in that window.
	e) When i find the actual position of starting word, I started storing samples.
	f) I stored samples till I find STE of window <50000.
	g) When I find window with STE < 50000, I repeated above method to find upto which position sample are part of word.
	h) When i find actual position, I store samples in word upto that position.
	i) After storing whole word I called for function to check if word is yes or no.
	j) I decided word on the basis of frecative (s) at the end of word.
	k) I started from end of the word and started calculating STE of window of size 320. 
	l) Idea behind this is I dont want to calculate ZCR for noices becuase noises may present at end of both yes and no.
	m) I check is STE > 200000.
	n) STE > 200000 signifies that it is sound, no noices effect ZCR.
	o) from that position i started calcuting ZCR for window of 100 samples.
	p) I calculated ZCR for 10 windows. We can't take too many windows because we have to find 's' only. If we take more window then it will involve othe letters in the word.
 	q) I took the mean of the ZCR values to ensure there is no effect of noise (or it is consistent).
	r) if mean ZCR>10 per 100 samples . I decide that the word is yes, else no.

2. How to execute
	a) Build the solution. Voice recoding will start for 12 sec, after voice is recorded, press "Enter" program will take some time 1 sec to recognize "yes" and "no" . Then black window will close automatically.
		1. First Line of "int _tmain()" contain statement "system("Recording_Module.exe 12 input_file.wav input_file.txt");" in this 12 is the time(in seconds) for recording. You can edit this as you want.  
	b) You can change the variables (window size, threshold silence, threshold noise) depending upon sound, Variables are declared globally in cpp file.
		1. I had kept threshold 50000 (STE) for sound below it, it is considered silence.
		2. I had also kept threshold 200000 (STE) for sound to be considered for calculating ZCR at back of the window. Because below this STE there can be noise same for both "yes" and "no" so we will not find any difference if we consider that data.
	c) I took variables which suited sir's voice as well as my voice.
		1. I had recorded multiple test samples you can find them in the folder, also "sirs_yes_no" is the test file with sir's voice.
		2. To run the already(by me in real time) recorded samples: -
			- Comment the 1st statement of "_tmain()" function i.e. "system("Recording_Module.exe 12 input_file.wav input_file.txt");".
			- Also in line no. 104 change the file name of input voice sample ("input_file.txt") according to the test file you want to recognize.
	d) "Result.txt" file store the words (sequence of yes and no).
	e) "OutputMultipleWords.txt" stores STE of window and from which window word started.
	f) "extractMultipleWords.txt" stores the samples of words extracted.
	