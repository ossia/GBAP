#include "GBAP.hpp"

#include <QDebug>

#include <algorithm>
#include <iostream>

namespace Example
{
void GBAP::operator()(halp::tick t)
{

  bool multi = !inputs.MultiTab.value.empty();


  float minX{0}, maxX{1}, minY{0}, maxY{1};
  int ModmX{0}, ModMX{0}, ModmY{0}, ModMY{0};

  volumes.clear();
  //volumes.resize(multi ? inputs.MultiTab.value.size() : 1);

  const int nSinks = (inputs.nSinksX.value * inputs.nSinksY.value);
  for (int i = 0; i< (multi ? inputs.MultiTab.value.size() : 1); i++){

    volumes.push_back(ossia::value());
    volumes[i]=std::vector<ossia::value>();

    float posX = multi ? inputs.MultiTab.value[i][0] : inputs.pos.value.x;
    float posY = multi ? inputs.MultiTab.value[i][1]  : inputs.pos.value.y;

    //if(static int c=0; c++ % 10 == 0)
    //qDebug() << posX << " / " << posY;

    const auto cursorSize = inputs.cursorSize.value;
    const auto sinkSize = inputs.sinkSize.value;

    if(inputs.nSinksX.value > 1)
    {
      const auto den = (sinkSize.x + intervX);
      if(den <= 0.f)
        return;

      minX = std::clamp(posX - cursorSize.x / 2, 0.f, 0.999999f);
      float ModminX = (float)minX / den;
      ModmX = std::floor(ModminX) * 2;
      if(ModminX - (float)ModmX / 2 >= sinkSize.x / den)
        ModmX++;
      maxX = std::clamp(posX + cursorSize.x / 2, 0.f, 0.999999f);
      float ModmaxX = (float)maxX / den;
      ModMX = std::floor(ModmaxX) * 2;
      if(ModmaxX - (float)ModMX / 2 >= sinkSize.x / den)
        ModMX++;
    }

    if(inputs.nSinksY.value > 1)
    {
      const auto den = (sinkSize.y + intervY);
      if(den <= 0.f)
        return;

      minY = std::clamp(posY - cursorSize.y / 2, 0.f, 0.999999f);
      float ModminY = (float)minY / den;
      ModmY = std::floor(ModminY) * 2;
      if(ModminY - (float)ModmY / 2 >= sinkSize.y / den)
        ModmY++;
      maxY = std::clamp(posY + cursorSize.y / 2, 0.f, 0.999999f);
      float ModmaxY = (float)maxY / den;
      ModMY = std::floor(ModmaxY) * 2;
      if(ModmaxY - (float)ModMY / 2 >= sinkSize.y / den)
        ModMY++;
    }

    auto & vec =  volumes[i].get<std::vector<ossia::value>>();
    if(nSinks <= 1)
    {
      vec.clear();
      nSinksprev = 0;
      return;
    }
    if(vec.size() != nSinks)
    {
      vec.clear();
      vec.resize(nSinks); // They will be initialized to zero
    }

    SCORE_ASSERT(vec.size() == nSinks);
    for (int v = 0; v < vec.size(); v++) {
      vec[v] = float(0);
    }


    for(int y = ModmY; y <= ModMY; y++)
    {
      for(int x = ModmX; x <= ModMX; x++)
      {
        int modType = x % 2 + 2 * (y % 2);
        float area = 0.f;
        float center = 0.5f;
        float Xs, Xe, Ys, Ye, Xm, Ym;

        switch(modType)
        {
          case 0:
            Xs = std::max(minX, (sinkSize.x + intervX) * x / 2);
            Xe = std::min(((sinkSize.x + intervX) * x + 2 * sinkSize.x) / 2, maxX);
            Ys = std::max(minY, (sinkSize.y + intervY) * y / 2);
            Ye = std::min(((sinkSize.y + intervY) * y + 2 * sinkSize.y) / 2, maxY);
            area = (Xe - Xs) / sinkSize.x * (Ye - Ys) / sinkSize.y;
            vec[x / 2 + y / 2 * inputs.nSinksX.value].get<float>() += area;
            break;
          case 1:
            if(inputs.nSinksX.value > 1)
            {
              Xm = ((sinkSize.x + intervX) * (x - 1) + 2 * sinkSize.x) / 2;
              Xs = std::max(minX, ((sinkSize.x + intervX) * (x - 1) + 2 * sinkSize.x) / 2);
              Xe = std::min((sinkSize.x + intervX) * std::floor((x + 1) / 2.f), maxX);
              Ys = std::max(minY, (sinkSize.y + intervY) * y / 2);
              Ye = std::min(((sinkSize.y + intervY) * y + 2 * sinkSize.y) / 2, maxY);
              area = (Xe - Xs) / intervX * (Ye - Ys) / sinkSize.y;
              center = ((Xe + Xs) / 2 - Xm) / intervX;
              vec[std::floor((float)x / 2) + y / 2 * inputs.nSinksX.value].get<float>() += (1 - center) * area;
              vec[std::ceil((float)x / 2) + y / 2 * inputs.nSinksX.value].get<float>() += center * area;
            }
            break;
          case 2:
            if(inputs.nSinksY.value > 1)
            {
              Ym = ((sinkSize.y + intervY) * (y - 1) + 2 * sinkSize.y) / 2;
              Xs = std::max(minX, (sinkSize.x + intervX) * x / 2);
              Xe = std::min(((sinkSize.x + intervX) * x + 2 * sinkSize.x) / 2, maxX);
              Ys = std::max(minY, ((sinkSize.y + intervY) * (y - 1) + 2 * sinkSize.y) / 2);
              Ye = std::min((sinkSize.y + intervY) * std::floor((y + 1) / 2.f), maxY);
              area = (Xe - Xs) / sinkSize.x * (Ye - Ys) / intervY;
              center = ((Ye + Ys) / 2 - Ym) / intervY;
              vec[x / 2 + std::floor((float)y / 2) * inputs.nSinksX.value].get<float>() += (1 - center) * area;
              vec[x / 2 + std::ceil((float)y / 2) * inputs.nSinksX.value].get<float>() += center * area;
            }
            break;
          case 3:
            if(inputs.nSinksY.value > 1 && inputs.nSinksX.value > 1)
            {
              Xm = ((sinkSize.x + intervX) * (x - 1) + 2 * sinkSize.x) / 2;
              Ym = ((sinkSize.y + intervY) * (y - 1) + 2 * sinkSize.y) / 2;
              Xs = std::max(minX, ((sinkSize.x + intervX) * (x - 1) + 2 * sinkSize.x) / 2);
              Xe = std::min((sinkSize.x + intervX) * std::floor((x + 1) / 2.f), maxX);
              Ys = std::max(minY, ((sinkSize.y + intervY) * (y - 1) + 2 * sinkSize.y) / 2);
              Ye = std::min((sinkSize.y + intervY) * std::floor((y + 1) / 2.f), maxY);
              area = (Xe - Xs) / intervX * (Ye - Ys) / intervY;
              float centerX = ((Xe + Xs) / 2 - Xm) / intervX;
              float centerY = ((Ye + Ys) / 2 - Ym) / intervY;
              vec[std::floor((float)x / 2) + std::floor((float)y / 2) * inputs.nSinksX.value].get<float>() += (1.f - centerX) * (1.f - centerY) * area;
              vec[std::ceil((float)x / 2) + std::floor((float)y / 2) * inputs.nSinksX.value].get<float>() += centerX * (1.f - centerY) * area;
              vec[std::floor((float)x / 2) + std::ceil((float)y / 2) * inputs.nSinksX.value].get<float>() += (1.f - centerX) * centerY * area;
              vec[std::ceil((float)x / 2) + std::ceil((float)y / 2) * inputs.nSinksX.value].get<float>() += centerX * centerY * area;
            }
            break;
        }
      }
    }

    rollOffArray(volumes[i].get<std::vector<ossia::value>>());
    if (inputs.normalize){
      normalizeArray(volumes[i].get<std::vector<ossia::value>>());
    }

    const float gain = inputs.gain.value
                       * ((inputs.weights.value.size() > 0)
                              ? inputs.weights.value[inputs.systemNumber.value - 1]
                              : 1.f);
    mult(volumes[i].get<std::vector<ossia::value>>(), gain);
    //vols = vols.concat(volumes);
    //outputs.weights.value[0] = posX;
    //outputs.weights.value[1] = posY;
  }

  nSinksprev = nSinks;
  outputs.weights.value = volumes;

}

void GBAP::rollOffArray(std::vector<ossia::value>& arr)
{
  if(rollOffV <= 0.)
    return;

  float sum{0};

  for(ossia::value& f :arr){
    sum += f.get<float>();
  }

  if(sum != 0)
  {
    for(ossia::value& f : arr)
      f.get<float>()  = std::pow((f.get<float>() / sum), rollOffV / 2.);
  }
}

void GBAP::mult(std::vector<ossia::value>& arr, float scal)
{
  for(ossia::value& f : arr)
    f.get<float>() *= scal;
}

void GBAP::normalizeArray(std::vector<ossia::value>& arr){

  if(arr.empty())
    return;

  float max {0};

  for (ossia::value& f : arr){
    if (f.get<float>() > max){
      max=f.get<float>();
    }
  }

  if (max != 0){
    for(ossia::value& f : arr){
      f.get<float>() /= max;
    }
  }

}
}
