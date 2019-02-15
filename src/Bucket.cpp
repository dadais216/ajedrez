#include "Bucket.h"
#include "assert.h"

Bucket* actualBucket;
Bucket** lastBucket;//bucketPieza o bucketHolder
int bucketSize=100000;
Bucket::Bucket(){
    head=data=new char[bucketSize];
    assert(data&&"no hay memoria");
    next=nullptr;
    actualBucket=this;
    *lastBucket=this;
}


