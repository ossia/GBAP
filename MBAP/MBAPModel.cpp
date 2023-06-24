#include "MBAP.hpp"
#include <algorithm>
#include <QDebug>>


namespace Example
{
void MBAP::operator()(halp::tick t)
{
  float minX{0},  maxX{1},
        minY{0},  maxY{1};
  int ModmX{0},  ModMX{0},
      ModmY{0},  ModMY{0};

  for (int i = 0; i<inputs.nSources; i++){

    float posX = inputs.pos.value.x;
    float posY = 1.f-inputs.pos.value.y;

    if (inputs.nSinksX.value>1){
      minX = std::min(std::max(0.f,(posX-inputs.cursorSize.value.x/2)),0.999999f);
      float ModminX = (float)minX/(inputs.sinkSize.value.x+intervX);
      ModmX = std::floor(ModminX)*2;
      if (ModminX-(float)ModmX/2 >= inputs.sinkSize.value.x/(inputs.sinkSize.value.x+intervX)) ModmX++;
      maxX = std::min(std::max(0.f, posX+inputs.cursorSize.value.x/2),0.999999f);
      float ModmaxX = (float)maxX/(inputs.sinkSize.value.x+intervX);
      ModMX = std::floor(ModmaxX)*2;
      if (ModmaxX-(float)ModMX/2 >= inputs.sinkSize.value.x/(inputs.sinkSize.value.x+intervX)) ModMX++;
    }

    if (inputs.nSinksY.value>1){
      minY = std::min(std::max(0.f,(posY-inputs.cursorSize.value.y/2)),0.999999f);
      float ModminY = (float)minY/(inputs.sinkSize.value.y+intervY);
      ModmY = std::floor(ModminY)*2;
      if (ModminY-(float)ModmY/2 >= inputs.sinkSize.value.y/(inputs.sinkSize.value.y+intervY)) ModmY++;
      maxY = std::min(std::max(0.f,(posY+inputs.cursorSize.value.y/2)),0.999999f);
      float ModmaxY = (float)maxY/(inputs.sinkSize.value.y+intervY);
      ModMY = std::floor(ModmaxY)*2;
      if (ModmaxY-(float)ModMY/2 >= inputs.sinkSize.value.y/(inputs.sinkSize.value.y+intervY)) ModMY++;
    }

    int nSinks = (inputs.nSinksX.value*inputs.nSinksY.value);
    if (nSinksprev!=nSinks) {
      volumes.resize(nSinks);
      while(nSinks--) volumes[nSinks] = 0;
      nSinksprev=nSinks;
    }

    for (int y = ModmY; y<=ModMY;y++){
      for (int x = ModmX; x<=ModMX;x++){
        int modType = x%2+2*(y%2);
        float area = 0.f;
        float center = 0.5f;
        float Xs, Xe, Ys, Ye, Xm, Ym;

        switch (modType){
          case 0:
            Xs = std::max(minX,(inputs.sinkSize.value.x+intervX)*x/2);
            Xe = std::min(((inputs.sinkSize.value.x+intervX)*x+2*inputs.sinkSize.value.x)/2, maxX);
            Ys = std::max(minY,(inputs.sinkSize.value.y+intervY)*y/2);
            Ye = std::min(((inputs.sinkSize.value.y+intervY)*y+2*inputs.sinkSize.value.y)/2, maxY);
            area = (Xe - Xs) / inputs.sinkSize.value.x * (Ye - Ys ) / inputs.sinkSize.value.y;
            volumes[x/2+y/2*inputs.nSinksX.value] += area;
            break;
          case 1:
            if (inputs.nSinksX.value>1){
              Xm = ((inputs.sinkSize.value.x+intervX)*(x-1)+2*inputs.sinkSize.value.x)/2;
              Xs = std::max(minX,((inputs.sinkSize.value.x+intervX)*(x-1)+2*inputs.sinkSize.value.x)/2);
              Xe = std::min((inputs.sinkSize.value.x+intervX)*std::floor((x+1)/2.f), maxX);
              Ys = std::max(minY,(inputs.sinkSize.value.y+intervY)*y/2);
              Ye = std::min(((inputs.sinkSize.value.y+intervY)*y+2*inputs.sinkSize.value.y)/2, maxY);
              area = (Xe - Xs) / intervX * (Ye - Ys ) / inputs.sinkSize.value.y;
              center = ((Xe + Xs) / 2 - Xm) / intervX;
              volumes[std::floor((float)x/2)+y/2*inputs.nSinksX.value] += (1-center)*area;
              volumes[std::ceil ((float)x/2)+y/2*inputs.nSinksX.value] += center*area;
            }
            break;
          case 2:
            if (inputs.nSinksX.value>1){
              Ym = ((inputs.sinkSize.value.y+intervY)*(y-1)+2*inputs.sinkSize.value.y)/2;
              Xs = std::max(minX,(inputs.sinkSize.value.x+intervX)*x/2);
              Xe = std::min(((inputs.sinkSize.value.x+intervX)*x+2*inputs.sinkSize.value.x)/2, maxX);
              Ys = std::max(minY,((inputs.sinkSize.value.y+intervY)*(y-1)+2*inputs.sinkSize.value.y)/2);
              Ye = std::min((inputs.sinkSize.value.y+intervY)*std::floor((y+1)/2.f), maxY);
              area = (Xe - Xs) / inputs.sinkSize.value.x * (Ye - Ys ) / intervY;
              center = ((Ye + Ys) / 2 - Ym) / intervY;
              volumes[x/2+std::floor((float)y/2)*inputs.nSinksX.value] += (1-center)*area;
              volumes[x/2+std::ceil((float)y/2)*inputs.nSinksX.value] += center*area;
            }
            break;
          case 3:
            if (inputs.nSinksY.value>1 && inputs.nSinksX.value>1){
              Xm = ((inputs.sinkSize.value.x+intervX)*(x-1)+2*inputs.sinkSize.value.x)/2;
              Ym = ((inputs.sinkSize.value.y+intervY)*(y-1)+2*inputs.sinkSize.value.y)/2;
              Xs = std::max(minX,((inputs.sinkSize.value.x+intervX)*(x-1)+2*inputs.sinkSize.value.x)/2);
              Xe = std::min((inputs.sinkSize.value.x+intervX)*std::floor((x+1)/2.f), maxX);
              Ys = std::max(minY,((inputs.sinkSize.value.y+intervY)*(y-1)+2*inputs.sinkSize.value.y)/2);
              Ye = std::min((inputs.sinkSize.value.y+intervY)*std::floor((y+1)/2.f), maxY);
              area = (Xe - Xs) / intervX * (Ye - Ys ) / intervY;
              float centerX = ((Xe + Xs) / 2 - Xm) / intervX;
              float centerY = ((Ye + Ys) / 2 - Ym) / intervY;
              volumes[std::floor((float)x/2)+std::floor((float)y/2)*inputs.nSinksX.value] += (1.f-centerX)*(1.f-centerY)*area;
              volumes[std::ceil((float)x/2)+std::floor((float)y/2)*inputs.nSinksX.value]  += centerX*(1.f-centerY)*area;
              volumes[std::floor((float)x/2)+std::ceil((float)y/2)*inputs.nSinksX.value]  += (1.f-centerX)*centerY*area;
              volumes[std::ceil((float)x/2)+std::ceil((float)y/2)*inputs.nSinksX.value]   += centerX*centerY*area;
            }
            break;
        }
      }
    }
    volumes = rollOffArray(volumes);
    if (inputs.weights.value.size() > 0) volumes = mult(volumes, inputs.weights.value[inputs.systemNumber.value-1]);
    volumes = mult(volumes, inputs.gain.value);
    //vols = vols.concat(volumes);
  }
  outputs.weights.value = volumes;
}
}
