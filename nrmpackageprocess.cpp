#include "nrmpackageprocess.h"

nrmPackageProcess::nrmPackageProcess()
{
    s = SEARCH;

    out = new RADIO_DATAPAC_STR;

    memset(out, 0 , OUT_SIZE);
}

nrmPackageProcess::~nrmPackageProcess()
{
    delete out;
}

void nrmPackageProcess::addRawData(const char *data, const int size)
{
    for(int i=0;i<size;i++) buffer.push_back(data[i]); //add data to buffer

    processBuffer();
}

void nrmPackageProcess::processBuffer()
{
    while(buffer.size()>0) {

        switch (s) {
        case SEARCH:

            for(size_t i=0;i<buffer.size();i++) {
              if(buffer.at(i)==0x55) {

                    s = WRITE;

                    for(size_t j=0;j<i;j++) buffer.pop_front();

                    break;
                }
            }

        case WRITE:

                if(package.size()<STRUCT_SIZE) {
                    package.push_back(buffer.front());
                    buffer.pop_front();
                }
                else {
                    processPackage();
                    s = SEARCH;
                }

            break;

        default:
            break;
        }

    }
}

void nrmPackageProcess::processPackage()
{
    RF_DATA_PACK pkg;

    memcpy(&pkg, package.data(), STRUCT_SIZE);

    //copy packages to temp buffer
    memcpy(tmpOut+pkg.id*100, pkg.data, 100);

    if(pkg.id==29) {//data is ready

        //copy data to out...
        memcpy(out, tmpOut, OUT_SIZE);


        //out structure ready to send

        //---------------------------

        memset(out, 0, OUT_SIZE); //reset at first package
    }

    package.clear();
}


