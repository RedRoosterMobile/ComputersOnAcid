#include "dkVideoSlitScan.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

dkVideoSlitScan::dkVideoSlitScan(void){

	mFrameBuffer=NULL;
	mOutBuffer=NULL;

	mFrameBufferIndex=0;
	mWidth=0;
	mHeight=0;
	mSlitCnt=0;
	mFrameSize=0;
	mBytePerRaw=0;

	mBlendPattern=NULL;
	mBlendRange=-1;
	mReverse=false;

}

dkVideoSlitScan::~dkVideoSlitScan(void){
	close();
}

int dkVideoSlitScan::close(){
	if(mFrameBuffer!=NULL)free(mFrameBuffer);
	if(mOutBuffer!=NULL)free(mOutBuffer);
	if(mBlendPattern!=NULL)free(mBlendPattern);
	return 1;
}

void dkVideoSlitScan::setReverse(bool reverse){
	mReverse=reverse;
}

bool dkVideoSlitScan::getReverse(){
	return mReverse;
}

size_t dkVideoSlitScan::getSlitCount(){
	return mSlitCnt;
}

int dkVideoSlitScan::allocate(size_t width,size_t height,size_t slitCount,float smoothing){
	unsigned int i,a;
	float*p;

	for(i=1,a=1;i<height+1;i++){
		if((int)(height/i)*i==height){
			if(slitCount>i){
				a=i;
			}else{
				slitCount=(i-slitCount<slitCount-a)?i:a;
				break;
			}
		}
	}

	if(mWidth!=width||mHeight!=height||mSlitCnt!=slitCount){
		mFrameBufferIndex=0;
		mWidth=width;
		mHeight=height;
		mSlitCnt=slitCount;
		mBytePerRaw=mWidth*3;
		mFrameSize=mBytePerRaw*mHeight;
		if(mFrameBuffer!=NULL)free(mFrameBuffer);
		if(mOutBuffer!=NULL)free(mOutBuffer);
		mFrameBuffer=(unsigned char*)malloc(sizeof(unsigned char)*mFrameSize*mSlitCnt);
		if(mFrameBuffer==NULL)return 0;
		memset(mFrameBuffer,0,sizeof(unsigned char)*mFrameSize*mSlitCnt);
		mOutBuffer=(unsigned char*)malloc(sizeof(unsigned char)*mFrameSize);
		if(mOutBuffer==NULL){
			free(mFrameBuffer);
			return 0;
		}
		memset(mOutBuffer,0,sizeof(unsigned char)*mFrameSize);
	}

	smoothing=smoothing<.0f?.0f:(smoothing>1.0f?1.0f:smoothing);
	a=(size_t)((mHeight/mSlitCnt)*smoothing);

	if(mBlendRange!=a){
		mBlendRange=a;
		if(mBlendPattern!=NULL)free(mBlendPattern);
		mBlendPattern=(float*)malloc(sizeof(float)*mBlendRange);
		p=mBlendPattern;
		for(i=0;i<(unsigned int)mBlendRange;i++)*p++=i/(float)mBlendRange;
		if(mBlendPattern==NULL){
			free(mFrameBuffer);
			free(mOutBuffer);
			return 0;
		}
	}
	return 1;
}

int dkVideoSlitScan::update(const unsigned char*buffer){
	unsigned int i;
	int cIndex;
	size_t h,w,sh,eh,sd,y;
	float v;
	unsigned char *p1;
	const unsigned char *p2;
	const unsigned char *t;

	t= buffer + mFrameSize;
	p1=mFrameBuffer+mFrameSize*mFrameBufferIndex;
	do *p1++ = *buffer++;while (buffer != t);

	if(mReverse){
		cIndex=mFrameBufferIndex+1;
		if(cIndex==mSlitCnt)cIndex=0;
	}else{
		cIndex=mFrameBufferIndex;
	}
	w=mHeight/mSlitCnt;
	for(i=0;i<mSlitCnt;i++){
		h=i*w;
		eh=h+w;
		if(i==0){
			sh=h;
			sd=eh-mBlendRange;
		}else if(i==mSlitCnt-1){
			sh=h-mBlendRange;
			sd=eh;
		}else{
			sh=h-mBlendRange;
			sd=eh-mBlendRange;
		}
		for(y=sh;y<eh;y++){
			p1=mOutBuffer+y*mBytePerRaw;
			p2=mFrameBuffer+mFrameSize*cIndex+y*mBytePerRaw;
			t= p2 + mBytePerRaw;
			if(y<h){
				v=*(mBlendPattern+y-sh);
				do *p1++ += (unsigned char)(*p2++*v);while (p2 != t);
			}else if(sd<y){
				v=*(mBlendPattern+eh-y);
				do *p1++ = (unsigned char)(*p2++*v);while (p2 != t);
			}else{
				do *p1++ = *p2++;while (p2 != t);
			}
		}
		if(mReverse){
			cIndex++;
			if(cIndex==mSlitCnt)cIndex=0;
		}else{
			cIndex--;
			if(cIndex<0)cIndex=mSlitCnt-1;
		}
	}
	mFrameBufferIndex++;
	if(mFrameBufferIndex==mSlitCnt)mFrameBufferIndex=0;
	return 1;
}

unsigned char* dkVideoSlitScan::getPixels(){
	return mOutBuffer;
}
