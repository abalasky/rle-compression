#include "RleData.h"

void RleData::Compress(const char* input, size_t inSize)
{

    //Store the result of the first pass
    std::vector<char> dataPass1;

    int counter = 1;
    //Increment counter while we detect repetitive characters
    for(size_t i = 1; i < inSize; i++) {

        if(input[i] == input[i-1]) {
            counter += 1;
        }
        else {

            //Handle > 127 counter, else skip
            for(int k = 0; k < counter / 127; k++) {
                //Say 127 chars in the run
                dataPass1.push_back(127);
                //Of type "the current char"
                dataPass1.push_back(input[i-1]);
            }

            //Get whatevers left over
            dataPass1.push_back(counter % 127);
            dataPass1.push_back(input[i-1]);
            counter = 1;
        }

    }

    //Get the last char... <--should solve my out of bounds problem
    for(int k = 0; k < counter / 127; k++) {
        //Say 127 chars in the run
        dataPass1.push_back(127);
        //Of type "the current char"
        dataPass1.push_back(input[inSize-1]);
    }
    //Get whatevers left over
    dataPass1.push_back(counter % 127);
    dataPass1.push_back(input[inSize-1]);



    //Perform a second pass and place in data_

    //Clear any prior compression
    mData.clear();

    //Consider pairs (runLength, char)
    for(size_t i = 0; i < dataPass1.size()-1; i+=2) {

        //If non-1 run, push the pair
        if(dataPass1[i] != 1) {
            mData.push_back(dataPass1[i]);
            mData.push_back(dataPass1[i+1]);
        }
        else {
            //Search for consecutive run=1 pairs
            counter = 1;

            //While the next pair is also run=1
            int pos = i;
            while(dataPass1[pos+2] == 1) {
                //Increment counter
                counter += 1;
                pos += 2;

                if(pos > dataPass1.size()-2) { //Once past the last pair
                    break;
                }
            }

            //Now we know how many consecutive run=1 pairs
            //there are starting at position i in the vec
            //and ending at pos+1 (the char)

            //Accrue all characters in range [i through pos+1]
            //first character is at i+1, last character at pos+1
            std::vector<char> singleChars;
            for(int j = i+1; j <= pos+1; j+= 2) {
                //+2 iterates only over chars

                //push each single char to storage
                singleChars.push_back(dataPass1[j]);
            }

            //If only 1 single char then dont make negative
            if(singleChars.size() == 1) {
                mData.push_back(1);
                mData.push_back(singleChars[0]);
            }
            else {

                //Push back the negative number enumarating # of single chars
                int numSingleChars = -1 * singleChars.size();

                //Deal with long negative runs seperately
                if(singleChars.size() > 127) {
                    //How many -127s should I push?
                    for(int k = 0; k < singleChars.size() / 127; k++) {

                        //Say 127 chars in the run
                        mData.push_back(-127);

                        //Push back each single char in the negative run
                        //index correctly
                        for(int i = 127 * k; i < 127 * (k+1); i++) {
                            mData.push_back(singleChars[i]);
                        }
                    }

                    int leftOverNegRun = (singleChars.size() % 127);
                    if(leftOverNegRun > 0) { //Only push if there is something left over


                        if(leftOverNegRun == 1) { //Write positive
                            mData.push_back(leftOverNegRun);
                        }
                        else {
                            mData.push_back(-1 * leftOverNegRun);
                        }

                    }


                    //Push back the last leftOverNegRun chars in singleChars
                    for(int i = singleChars.size() - leftOverNegRun;
                    i < singleChars.size(); i++) {
                        mData.push_back(singleChars[i]);
                    }
                }
                else {
                    //Standard routine
                    mData.push_back(numSingleChars);

                    //Push back each single char
                    for(auto letter: singleChars) {
                        mData.push_back(letter);
                    }
                }
            }

            //Move loop forward past the "negative" run
            i = pos;
        }
    }

}

/*
    Decompress char* input and store in data_ vector
*/
void RleData::Decompress(const char* input, size_t inSize, size_t outSize)
{

    //Clear any old contents
    this->mData.clear();

    //Iterate over input and decompress
    int counter = 0;
    while(counter < inSize) {

        //Get length of run
        int runLength = input[counter];

        if(runLength < 0) { //Handle a negative run
            //Make positive for loop
            runLength = runLength * -1;

            //Add the next runLength chars to the buffer
            for(int j = 1; j < runLength+1; j++) {
                mData.push_back(input[counter+j]);
            }

            //Update counter
            counter = counter + runLength + 1;

        }
        else { //Handle a positive run

            //Get the actual char value
            int runChar = input[counter+1];

            for(int j = 0; j < runLength; j++) {
                mData.push_back(runChar);
            }

            //Update counter
            counter += 2;

        }
    }

}

std::ostream& operator<< (std::ostream& stream, const RleData& rhs)
{
	for (int i = 0; i < rhs.mData.size(); i++)
	{
		stream << rhs.mData[i];
	}
	return stream;
}

