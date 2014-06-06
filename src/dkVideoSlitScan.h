#pragma once
#include "ofMain.h"
class dkVideoSlitScan
{
public:

	dkVideoSlitScan(void);
	~dkVideoSlitScan(void);
	int allocate(size_t width,size_t height,size_t slitCount,float smoothing);
	int close();
	unsigned char* getPixels();

	int update(const unsigned char*buffer);
	void setReverse(bool reverse);
	bool getReverse();
	size_t  getSlitCount();

private:

	unsigned char*mOutBuffer;
	unsigned char*mFrameBuffer;

	int mFrameBufferIndex;
	size_t mWidth;
	size_t mHeight;
	size_t mSlitCnt;
	size_t mFrameSize;
	size_t mBytePerRaw;

	float *mBlendPattern;
	int mBlendRange;
	bool mReverse;

};

