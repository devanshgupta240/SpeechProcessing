========================================================================
    CONSOLE APPLICATION : YesNoDetection Project Overview
========================================================================

Name :- Devansh Gupta
Roll No.:- 170101022

1. Working of the code:-
	a) First I calculated STE for window of every 320 samples.
	b) checked if STE > 5000 
	c) if STE>5000, that means it is not silence, word has started
	d) then I decrease the size of window to 10 to precisely find from where actually word start in that window.
	e) When i find the actual position of starting word, I started storing samples.
	f) I stored samples till I find STE of window <5000.
	g) When I find window with STE < 5000, I repeated above method to find upto which position sample are part of word.
	h) When i find actual position, I store samples in word upto that position.
	i) After storing whole word I called for function to check if word is yes or no.
	j) I decided word on the basis of frecative (s) at the end of word.
	k) I started from end of the word and started calculating STE of window of size 320. 
	l) Idea behind this is I dont want to calculate ZCR for noices becuase noises may present at end of both yes and no.
	m) I check is STE > 50000.
	n) STE > 50000 signifies that it is sound, no noices effect ZCR.
	o) from that position i started calcuting ZCR for window of 100 samples.
	p) I calculated ZCR for 10 windows. We can't take too many windows because we have to find 's' only. If we take more window then it will involve othe letters in the word.
 	q) I took the mean of the ZCR values to ensure there is no effect of noise (or it is consistent).
	r) if mean ZCR>15 per 100 samples . I decide that the word is yes, else no.

2. How to execute
	a) store the voice sample in text format after removing headed from it. Name of the text files should be "sirs_yes_no.txt".
	b) You can change the variables (window size, threshold silence, threshold noise) depending upon sound, Variables are declared globally in cpp file.
	c) I took variables which suited sir's voice.
	d) "Result.txt" file store the words (sequence of yes and no).
	e) "OutputMultipleWords.txt" stores STE of window and from which window word started.
	f) "extractMultipleWords.txt" stores the samples of words extracted.
	
	 
	